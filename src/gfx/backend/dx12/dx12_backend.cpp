// Copyright (c) 2025 Inan Evin

#include "dx12_backend.hpp"
#include "dx12_common.hpp"
#include "gfx/common/descriptions.hpp"
#include "gfx/backend/dx12/sdk/D3D12MemAlloc.h"
#include "gfx/common/commands.hpp"
#include "gfx/common/gfx_common.hpp"
#include "data/string_util.hpp"
#include "data/vector_util.hpp"
#include "io/log.hpp"
#include "memory/memory.hpp"
#include "math/math_common.hpp"

#define SERIALIZE_DEBUG_INFORMATION 0
#ifdef SERIALIZE_DEBUG_INFORMATION
#include <fstream>
#endif

using Microsoft::WRL::ComPtr;

namespace SFG
{

	Microsoft::WRL::ComPtr<IDxcLibrary> dx12_backend::s_idxcLib;

#define DX12_THROW(exception, ...)                                                                                                                                                                                                                                 \
	SFG_FATAL(__VA_ARGS__);                                                                                                                                                                                                                                        \
	throw exception;

#ifdef _DEBUG
#define NAME_DX12_OBJECT_CSTR(x, NAME)                                                                                                                                                                                                                             \
	auto wcharConverted = string_util::char_to_wchar(NAME);                                                                                                                                                                                                        \
	x->SetName(wcharConverted);                                                                                                                                                                                                                                    \
	delete[] wcharConverted;

#define NAME_DX12_OBJECT(x, NAME) x->SetName(NAME)
#else
#define NAME_DX12_OBJECT_CSTR(x, NAME)
#define NAME_DX12_OBJECT(x, NAME)
#endif

	namespace
	{
		D3D12_TEXTURE_ADDRESS_MODE get_address_mode(bitmask<uint16> sampler_flags)
		{
			if (sampler_flags.is_set(saf_address_mode_repeat))
				return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			else if (sampler_flags.is_set(saf_address_mode_border))
				return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			else if (sampler_flags.is_set(saf_address_mode_clamp))
				return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			else if (sampler_flags.is_set(saf_address_mode_mirrored_clamp))
				return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
			else if (sampler_flags.is_set(saf_address_mode_mirrored_repeat))
				return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;

			return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		}

		D3D12_FILTER get_filter(bitmask<uint16> sampler_flags)
		{

			if (sampler_flags.is_set(saf_min_anisotropic) && sampler_flags.is_set(saf_mag_anisotropic))
				return D3D12_FILTER_ANISOTROPIC;
			else if (sampler_flags.is_set(saf_mip_linear) && sampler_flags.is_set(saf_min_linear) && sampler_flags.is_set(saf_mag_linear))
				return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			else if (sampler_flags.is_set(saf_mip_nearest) && sampler_flags.is_set(saf_min_linear) && sampler_flags.is_set(saf_mag_linear))
				return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			else if (sampler_flags.is_set(saf_mip_linear) && sampler_flags.is_set(saf_min_linear) && sampler_flags.is_set(saf_mag_nearest))
				return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
			else if (sampler_flags.is_set(saf_mip_nearest) && sampler_flags.is_set(saf_min_linear) && sampler_flags.is_set(saf_mag_nearest))
				return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
			else if (sampler_flags.is_set(saf_mip_linear) && sampler_flags.is_set(saf_min_nearest) && sampler_flags.is_set(saf_mag_linear))
				return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
			else if (sampler_flags.is_set(saf_mip_nearest) && sampler_flags.is_set(saf_min_nearest) && sampler_flags.is_set(saf_mag_linear))
				return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
			else if (sampler_flags.is_set(saf_mip_linear) && sampler_flags.is_set(saf_min_nearest) && sampler_flags.is_set(saf_mag_nearest))
				return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
			else if (sampler_flags.is_set(saf_mip_nearest) && sampler_flags.is_set(saf_min_nearest) && sampler_flags.is_set(saf_mag_nearest))
				return D3D12_FILTER_MIN_MAG_MIP_POINT;

			return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		}

		void border_color(bitmask<uint16> flags, float* color)
		{
			if (flags.is_set(saf_border_transparent))
				color[0] = color[1] = color[2] = color[3] = 0.0f;
			else
				color[0] = color[1] = color[2] = color[3] = 1.0f;
		}

		D3D12_COMMAND_LIST_TYPE get_command_type(command_type type)
		{
			switch (type)
			{
			case command_type::graphics:
				return D3D12_COMMAND_LIST_TYPE_DIRECT;
			case command_type::transfer:
				return D3D12_COMMAND_LIST_TYPE_COPY;
			case command_type::compute:
				return D3D12_COMMAND_LIST_TYPE_COMPUTE;
			default:
				return D3D12_COMMAND_LIST_TYPE_DIRECT;
			}
		}

		DXGI_FORMAT get_format(format format)
		{
			switch (format)
			{
			case format::undefined:
				return DXGI_FORMAT_UNKNOWN;

				// 8 bit
			case format::r8_sint:
				return DXGI_FORMAT_R8_SINT;
			case format::r8_uint:
				return DXGI_FORMAT_R8_UINT;
			case format::r8_unorm:
				return DXGI_FORMAT_R8_UNORM;
			case format::r8_snorm:
				return DXGI_FORMAT_R8_SNORM;

			case format::r8g8_sint:
				return DXGI_FORMAT_R8G8_SINT;
			case format::r8g8_uint:
				return DXGI_FORMAT_R8G8_UINT;
			case format::r8g8_unorm:
				return DXGI_FORMAT_R8G8_UNORM;
			case format::r8g8_snorm:
				return DXGI_FORMAT_R8G8_SNORM;

			case format::r8g8b8a8_sint:
				return DXGI_FORMAT_R8G8B8A8_SINT;
			case format::r8g8b8a8_uint:
				return DXGI_FORMAT_R8G8B8A8_UINT;
			case format::r8g8b8a8_unorm:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			case format::r8g8b8a8_snorm:
				return DXGI_FORMAT_R8G8B8A8_SNORM;
			case format::r8g8b8a8_srgb:
				return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

			case format::b8g8r8a8_unorm:
				return DXGI_FORMAT_B8G8R8A8_UNORM;
			case format::b8g8r8a8_srgb:
				return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

				// 16 bit
			case format::r16_sint:
				return DXGI_FORMAT_R16_SINT;
			case format::r16_uint:
				return DXGI_FORMAT_R16_UINT;
			case format::r16_unorm:
				return DXGI_FORMAT_R16_UNORM;
			case format::r16_snorm:
				return DXGI_FORMAT_R16_SNORM;
			case format::r16_sfloat:
				return DXGI_FORMAT_R16_FLOAT;

			case format::r16g16_sint:
				return DXGI_FORMAT_R16G16_SINT;
			case format::r16g16_uint:
				return DXGI_FORMAT_R16G16_UINT;
			case format::r16g16_unorm:
				return DXGI_FORMAT_R16G16_UNORM;
			case format::r16g16_snorm:
				return DXGI_FORMAT_R16G16_SNORM;
			case format::r16g16_sfloat:
				return DXGI_FORMAT_R16G16_FLOAT;

			case format::r16g16b16a16_sint:
				return DXGI_FORMAT_R16G16B16A16_SINT;
			case format::r16g16b16a16_uint:
				return DXGI_FORMAT_R16G16B16A16_UINT;
			case format::r16g16b16a16_unorm:
				return DXGI_FORMAT_R16G16B16A16_UNORM;
			case format::r16g16b16a16_snorm:
				return DXGI_FORMAT_R16G16B16A16_SNORM;
			case format::r16g16b16a16_sfloat:
				return DXGI_FORMAT_R16G16B16A16_FLOAT;

				// 32 bit
			case format::r32_sint:
				return DXGI_FORMAT_R32_SINT;
			case format::r32_uint:
				return DXGI_FORMAT_R32_UINT;
			case format::r32_sfloat:
				return DXGI_FORMAT_R32_FLOAT;

			case format::r32g32_sint:
				return DXGI_FORMAT_R32G32_SINT;
			case format::r32g32_uint:
				return DXGI_FORMAT_R32G32_UINT;
			case format::r32g32_sfloat:
				return DXGI_FORMAT_R32G32_FLOAT;

			case format::r32g32b32_sfloat:
				return DXGI_FORMAT_R32G32B32_FLOAT;
			case format::r32g32b32_sint:
				return DXGI_FORMAT_R32G32B32_SINT;
			case format::r32g32b32_uint:
				return DXGI_FORMAT_R32G32B32_UINT;

			case format::r32g32b32a32_sint:
				return DXGI_FORMAT_R32G32B32A32_SINT;
			case format::r32g32b32a32_uint:
				return DXGI_FORMAT_R32G32B32A32_UINT;
			case format::r32g32b32a32_sfloat:
				return DXGI_FORMAT_R32G32B32A32_FLOAT;

				// depth-stencil
			case format::d32_sfloat:
				return DXGI_FORMAT_D32_FLOAT;
			case format::d24_unorm_s8_uint:
				return DXGI_FORMAT_D24_UNORM_S8_UINT;
			case format::d16_unorm:
				return DXGI_FORMAT_D16_UNORM;

				// misc
			case format::r11g11b10_sfloat:
				return DXGI_FORMAT_R11G11B10_FLOAT;
			case format::r10g0b10a2_int:
				return DXGI_FORMAT_R10G10B10A2_UINT;
			case format::bc3_block_srgb:
				return DXGI_FORMAT_BC3_UNORM_SRGB;
			case format::bc3_block_unorm:
				return DXGI_FORMAT_BC3_UNORM;

			default:
				return DXGI_FORMAT_UNKNOWN;
			}

			return DXGI_FORMAT_UNKNOWN;
		}

		uint8 get_color_mask(bitmask<uint8> comp_flags)
		{
			return comp_flags.value(); // 1-1 map
		}
		D3D12_BLEND get_blend(blend_factor factor)
		{
			switch (factor)
			{
			case blend_factor::zero:
				return D3D12_BLEND_ZERO;
			case blend_factor::one:
				return D3D12_BLEND_ONE;
			case blend_factor::src_color:
				return D3D12_BLEND_SRC_COLOR;
			case blend_factor::one_minus_src_color:
				return D3D12_BLEND_INV_SRC_COLOR;
			case blend_factor::dst_color:
				return D3D12_BLEND_DEST_COLOR;
			case blend_factor::one_minus_dst_color:
				return D3D12_BLEND_INV_DEST_COLOR;
			case blend_factor::src_alpha:
				return D3D12_BLEND_SRC_ALPHA;
			case blend_factor::one_minus_src_alpha:
				return D3D12_BLEND_INV_SRC_ALPHA;
			case blend_factor::dst_alpha:
				return D3D12_BLEND_DEST_ALPHA;
			case blend_factor::one_minus_dst_alpha:
				return D3D12_BLEND_INV_DEST_ALPHA;
			default:
				return D3D12_BLEND_ZERO;
			}
		}

		D3D12_BLEND_OP get_blend_op(blend_op op)
		{
			switch (op)
			{
			case blend_op::add:
				return D3D12_BLEND_OP_ADD;
			case blend_op::subtract:
				return D3D12_BLEND_OP_SUBTRACT;
			case blend_op::reverse_subtract:
				return D3D12_BLEND_OP_REV_SUBTRACT;
			case blend_op::min:
				return D3D12_BLEND_OP_MIN;
			case blend_op::max:
				return D3D12_BLEND_OP_MAX;
			default:
				return D3D12_BLEND_OP_ADD;
			}
		}

		D3D12_LOGIC_OP get_logic_op(logic_op op)
		{
			switch (op)
			{
			case logic_op::clear:
				return D3D12_LOGIC_OP_CLEAR;
			case logic_op::and_:
				return D3D12_LOGIC_OP_AND;
			case logic_op::and_reverse:
				return D3D12_LOGIC_OP_AND_REVERSE;
			case logic_op::copy:
				return D3D12_LOGIC_OP_COPY;
			case logic_op::and_inverted:
				return D3D12_LOGIC_OP_AND_INVERTED;
			case logic_op::no_op:
				return D3D12_LOGIC_OP_NOOP;
			case logic_op::xor_:
				return D3D12_LOGIC_OP_XOR;
			case logic_op::or_:
				return D3D12_LOGIC_OP_OR;
			case logic_op::nor:
				return D3D12_LOGIC_OP_NOR;
			case logic_op::equivalent:
				return D3D12_LOGIC_OP_EQUIV;
			default:
				return D3D12_LOGIC_OP_CLEAR;
			}
		}

		D3D12_CULL_MODE get_cull_mode(cull_mode cm)
		{
			switch (cm)
			{
			case cull_mode::none:
				return D3D12_CULL_MODE_NONE;
			case cull_mode::front:
				return D3D12_CULL_MODE_FRONT;
			case cull_mode::back:
				return D3D12_CULL_MODE_BACK;
			default:
				return D3D12_CULL_MODE_NONE;
			}
		}

		D3D12_COMPARISON_FUNC get_compare_op(compare_op op)
		{
			switch (op)
			{
			case compare_op::never:
				return D3D12_COMPARISON_FUNC_NEVER;
			case compare_op::less:
				return D3D12_COMPARISON_FUNC_LESS;
			case compare_op::equal:
				return D3D12_COMPARISON_FUNC_EQUAL;
			case compare_op::lequal:
				return D3D12_COMPARISON_FUNC_LESS_EQUAL;
			case compare_op::greater:
				return D3D12_COMPARISON_FUNC_GREATER;
			case compare_op::nequal:
				return D3D12_COMPARISON_FUNC_NOT_EQUAL;
			case compare_op::gequal:
				return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
			case compare_op::always:
				return D3D12_COMPARISON_FUNC_ALWAYS;
			default:
				return D3D12_COMPARISON_FUNC_ALWAYS;
			}
		}

		D3D12_STENCIL_OP get_stencil_op(stencil_op op)
		{
			switch (op)
			{
			case stencil_op::keep:
				return D3D12_STENCIL_OP_KEEP;
			case stencil_op::zero:
				return D3D12_STENCIL_OP_ZERO;
			case stencil_op::replace:
				return D3D12_STENCIL_OP_REPLACE;
			case stencil_op::increment_clamp:
				return D3D12_STENCIL_OP_INCR_SAT;
			case stencil_op::decrement_clamp:
				return D3D12_STENCIL_OP_DECR_SAT;
			case stencil_op::invert:
				return D3D12_STENCIL_OP_INVERT;
			case stencil_op::increment_wrap:
				return D3D12_STENCIL_OP_INCR;
			case stencil_op::decrement_wrap:
				return D3D12_STENCIL_OP_DECR;
			default:
				return D3D12_STENCIL_OP_KEEP;
			}
		}

		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE get_load_op(load_op op)
		{
			switch (op)
			{
			case load_op::load:
				return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
			case load_op::clear:
				return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
			case load_op::dont_care:
				return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
			case load_op::none:
			default:
				return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS;
			}
		}

		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE get_store_op(store_op op)
		{
			switch (op)
			{
			case store_op::store:
				return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
			case store_op::dont_care:
				return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
			case store_op::none:
			default:
				return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS;
			}
		}

		D3D12_PRIMITIVE_TOPOLOGY_TYPE get_topology_type(topology tp)
		{
			switch (tp)
			{
			case topology::point_list:
				return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
			case topology::line_list:
			case topology::line_strip:
				return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
			case topology::triangle_list:
			case topology::triangle_strip:
			case topology::triangle_fan:
			default:
				return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			}
		}

		D3D12_PRIMITIVE_TOPOLOGY get_topology(topology tp)
		{
			switch (tp)
			{
			case topology::point_list:
				return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
			case topology::line_list:
				return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			case topology::line_strip:
				return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
			case topology::triangle_list:
				return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			case topology::triangle_strip:
				return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			case topology::triangle_fan:
				return D3D_PRIMITIVE_TOPOLOGY_TRIANGLEFAN;
			default:
				return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			}
		}

		D3D12_RESOURCE_STATES get_resource_state(resource_state state)
		{
			switch (state)
			{
			case SFG::resource_state::common:
				return D3D12_RESOURCE_STATE_COMMON;
			case SFG::resource_state::vertex_cbv:
				return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			case SFG::resource_state::index_buffer:
				return D3D12_RESOURCE_STATE_INDEX_BUFFER;
			case SFG::resource_state::render_target:
				return D3D12_RESOURCE_STATE_RENDER_TARGET;
			case SFG::resource_state::uav:
				return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			case SFG::resource_state::depth_write:
				return D3D12_RESOURCE_STATE_DEPTH_WRITE;
			case SFG::resource_state::depth_read:
				return D3D12_RESOURCE_STATE_DEPTH_READ;
			case SFG::resource_state::non_ps_resource:
				return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			case SFG::resource_state::ps_resource:
				return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			case SFG::resource_state::indirect_arg:
				return D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
			case SFG::resource_state::copy_dest:
				return D3D12_RESOURCE_STATE_COPY_DEST;
			case SFG::resource_state::copy_source:
				return D3D12_RESOURCE_STATE_COPY_SOURCE;
			case SFG::resource_state::resolve_dest:
				return D3D12_RESOURCE_STATE_RESOLVE_DEST;
			case SFG::resource_state::resolve_source:
				return D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
			case SFG::resource_state::generic_read:
				return D3D12_RESOURCE_STATE_GENERIC_READ;
			case SFG::resource_state::present:
				return D3D12_RESOURCE_STATE_PRESENT;
			default:
				return D3D12_RESOURCE_STATE_COMMON;
			}
		}

		D3D12_SHADER_VISIBILITY get_visibility(shader_stage stage)
		{
			if (stage == shader_stage::vertex)
				return D3D12_SHADER_VISIBILITY_VERTEX;
			else if (stage == shader_stage::fragment)
				return D3D12_SHADER_VISIBILITY_PIXEL;
			else
				return D3D12_SHADER_VISIBILITY_ALL;
		}

		void get_hw_adapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, uint32 gpu_type)
		{
			*ppAdapter = nullptr;

			ComPtr<IDXGIAdapter1> adapter;

			const unsigned int NVIDIA_VENDOR_ID = 0x10DE;
			const unsigned int AMD_VENDOR_ID	= 0x1002;
			const unsigned int INTEL_VENDOR_ID	= 0x8086;

			auto find = [&](bool agressive) {
				ComPtr<IDXGIFactory6> factory6;
				if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
				{
					for (UINT index = 0; SUCCEEDED(factory6->EnumAdapterByGpuPreference(index, gpu_type == 0 ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_MINIMUM_POWER, IID_PPV_ARGS(&adapter))); ++index)
					{
						DXGI_ADAPTER_DESC1 desc;
						adapter->GetDesc1(&desc);

						if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
							continue;

						if (agressive && desc.VendorId != NVIDIA_VENDOR_ID && desc.VendorId != AMD_VENDOR_ID && gpu_type == 0)
							continue;

						if (agressive && (desc.VendorId == NVIDIA_VENDOR_ID || desc.VendorId == AMD_VENDOR_ID) && gpu_type == 1)
							continue;

						// Check to see whether the adapter supports Direct3D 12, but don't create the
						// actual device yet.
						if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
						{
							char* buf = string_util::wchar_to_char(desc.Description);
							SFG_TRACE("DX12 -> Selected hardware adapter {0}, dedicated video memory {1} mb", buf, desc.DedicatedVideoMemory * 0.000001);
							delete[] buf;
							break;
						}
					}
				}
			};

			find(true);
			if (adapter.Get() == nullptr)
				find(false);

			if (adapter.Get() == nullptr)
			{
				SFG_FATAL("DX12 -> Failed finding a suitable device!");
				return;
			}

			*ppAdapter = adapter.Detach();
		}

		void msg_callback(D3D12_MESSAGE_CATEGORY msg_type, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID messageId, LPCSTR pDesc, void* pContext)
		{
			if (pDesc != NULL)
			{
				if (severity == D3D12_MESSAGE_SEVERITY_MESSAGE)
				{
					SFG_TRACE("DX12 -> {0}", pDesc);
				}
				else if (severity == D3D12_MESSAGE_SEVERITY_INFO)
				{
					// LOGV("Backend -> {0}, pDesc);
				}
				else
				{
					SFG_ERR("Backend -> {0}", pDesc);
				}
			}
		}

	}

	dx12_backend* dx12_backend::s_instance = nullptr;

	DWORD msgcallback = 0;
	void  dx12_backend::init()
	{
		UINT dxgiFactoryFlags = 0;

#ifdef SFG_DEBUG
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();

			// Enable additional debug layers.
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
		else
		{
			SFG_ERR("DX12 -> Failed enabling debug layers!");
		}
#endif

		throw_if_failed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&_factory)));

		ComPtr<IDXGIFactory5> factory5;
		HRESULT				  facres	   = _factory.As(&factory5);
		BOOL				  allowTearing = FALSE;
		if (SUCCEEDED(facres))
			facres = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));

		// Choose gpu & create device
		{
			get_hw_adapter(_factory.Get(), &_adapter, GPU_DEVICE);
			throw_if_failed(D3D12CreateDevice(_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device)));
		}

		// Dbg callback
		{
			ID3D12InfoQueue1* infoQueue = nullptr;
			if (SUCCEEDED(_device->QueryInterface<ID3D12InfoQueue1>(&infoQueue)))
			{
				infoQueue->RegisterMessageCallback(&msg_callback, D3D12_MESSAGE_CALLBACK_IGNORE_FILTERS, nullptr, &msgcallback);
			}
		}

		// Allocator
		{
			D3D12MA::ALLOCATOR_DESC allocatorDesc;
			allocatorDesc.pDevice			   = _device.Get();
			allocatorDesc.PreferredBlockSize   = 0;
			allocatorDesc.Flags				   = D3D12MA::ALLOCATOR_FLAG_NONE;
			allocatorDesc.pAdapter			   = _adapter.Get();
			allocatorDesc.pAllocationCallbacks = NULL;
			throw_if_failed(D3D12MA::CreateAllocator(&allocatorDesc, &_allocator));
		}

		// DXC
		{
			HRESULT hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&s_idxcLib));
			if (FAILED(hr))
			{
				SFG_FATAL("DX12 -> Failed to create DXC library!");
			}
		}

		_queue_graphics = create_queue({
			.type		= command_type::graphics,
			.debug_name = {"GfxQueue"},
		});
		_queue_transfer = create_queue({
			.type		= command_type::transfer,
			.debug_name = {"TransferQueue"},
		});
		_queue_compute	= create_queue({
			 .type		 = command_type::compute,
			 .debug_name = {"CmpQueue"},
		 });

		const uint32 size_cbv_srv_uav = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		const uint32 size_dsv		  = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		const uint32 size_rtv		  = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		const uint32 size_sampler	  = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

		_heap_buffer.init(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024, size_cbv_srv_uav, false);
		_heap_texture.init(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024, size_cbv_srv_uav, false);
		_heap_dsv.init(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1024, size_dsv, false);
		_heap_rtv.init(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1024, size_rtv, false);
		_heap_sampler.init(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1024, size_sampler, false);
		_heap_gpu_buffer.init(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024, size_cbv_srv_uav, true);
		_heap_gpu_sampler.init(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1024, size_sampler, true);

		_reuse_dest_descriptors_buffer.reserve(100);
		_reuse_dest_descriptors_sampler.reserve(100);
		_reuse_src_descriptors_buffer.reserve(100);
		_reuse_src_descriptors_sampler.reserve(100);
		_reuse_barriers.reserve(100);
		_reuse_color_attachments.reserve(100);
		_reuse_root_params.reserve(100);
		_reuse_root_ranges.reserve(100);
		_reuse_subresource_data.reserve(100);
		_reuse_lists.reserve(100);
		_reuse_fences.reserve(100);
		_reuse_values.reserve(100);
		_reuse_static_samplers.reserve(100);
	}

	void dx12_backend::uninit()
	{
		destroy_queue(_queue_graphics);
		destroy_queue(_queue_transfer);
		destroy_queue(_queue_compute);

		_resources.verify_uninit();
		_textures.verify_uninit();
		_samplers.verify_uninit();
		_swapchains.verify_uninit();
		_semaphores.verify_uninit();
		_shaders.verify_uninit();
		_bind_groups.verify_uninit();
		_command_buffers.verify_uninit();
		_command_allocators.verify_uninit();
		_queues.verify_uninit();
		_indirect_signatures.verify_uninit();
		_descriptors.verify_uninit();
		_bind_layouts.verify_uninit();

		_heap_buffer.uninit();
		_heap_texture.uninit();
		_heap_dsv.uninit();
		_heap_rtv.uninit();
		_heap_sampler.uninit();
		_heap_gpu_buffer.uninit();
		_heap_gpu_sampler.uninit();

#ifdef SFG_DEBUG
		ID3D12InfoQueue1* infoQueue = nullptr;
		if (SUCCEEDED(_device->QueryInterface<ID3D12InfoQueue1>(&infoQueue)))
		{
			infoQueue->UnregisterMessageCallback(msgcallback);
		}
#endif

		_allocator->Release();
		s_idxcLib.Reset();
		_device.Reset();
	}

	void dx12_backend::reset_command_buffer(gfx_id cmd_buffer)
	{
		command_buffer&				cmd_buf	  = _command_buffers.get(cmd_buffer);
		ID3D12GraphicsCommandList4* cmd_list  = cmd_buf.ptr.Get();
		ID3D12CommandAllocator*		cmd_alloc = _command_allocators.get(cmd_buf.allocator).ptr.Get();
		throw_if_failed(cmd_alloc->Reset());
		throw_if_failed(cmd_list->Reset(cmd_alloc, nullptr));
		if (cmd_buf.is_transfer == 0)
		{
			ID3D12DescriptorHeap* heaps[] = {_heap_gpu_buffer.get_heap(), _heap_gpu_sampler.get_heap()};
			cmd_list->SetDescriptorHeaps(_countof(heaps), heaps);
		}
	}

	void dx12_backend::close_command_buffer(gfx_id cmd_buffer)
	{
		command_buffer&				cmd_buf	 = _command_buffers.get(cmd_buffer);
		ID3D12GraphicsCommandList4* cmd_list = cmd_buf.ptr.Get();
		throw_if_failed(cmd_list->Close());
	}

	void dx12_backend::submit_commands(gfx_id queue_id, const gfx_id* commands, uint8 commands_count)
	{
		queue& q = _queues.get(queue_id);
		_reuse_lists.resize(0);

		for (uint8 i = 0; i < commands_count; i++)
		{
			command_buffer& cb = _command_buffers.get(commands[i]);
			_reuse_lists.push_back(cb.ptr.Get());
		}

		q.ptr->ExecuteCommandLists(static_cast<uint32>(commands_count), _reuse_lists.data());
	}

	void dx12_backend::queue_wait(gfx_id queue_id, const gfx_id* semaphores, const uint64* semaphore_values, uint8 semaphore_count)
	{
		queue& q = _queues.get(queue_id);

		for (uint8 i = 0; i < semaphore_count; i++)
			q.ptr->Wait(_semaphores.get(semaphores[i]).ptr.Get(), semaphore_values[i]);
	}

	void dx12_backend::queue_signal(gfx_id queue_id, const gfx_id* semaphores, const uint64* semaphore_values, uint8 semaphore_count)
	{
		queue& q = _queues.get(queue_id);

		for (uint8 i = 0; i < semaphore_count; i++)
		{
			q.ptr->Signal(_semaphores.get(semaphores[i]).ptr.Get(), semaphore_values[i]);
		}
	}

	void dx12_backend::present(const gfx_id* swapchains, uint8 swapchain_count)
	{
		for (uint8 i = 0; i < swapchain_count; i++)
		{
			swapchain& swp = _swapchains.get(swapchains[i]);
			throw_if_failed(swp.ptr->Present(swp.vsync, 0));
			swp.image_index = swp.ptr->GetCurrentBackBufferIndex();
		}
	}

	gfx_id dx12_backend::create_resource(const resource_desc& desc)
	{
		const gfx_id id	 = _resources.add();
		resource&	 res = _resources.get(id);

		const uint32 aligned_size = ALIGN_SIZE_POW(desc.size, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
		const uint32 final_size	  = desc.flags.is_set(resource_flags::rf_constant_buffer) ? aligned_size : desc.size;
		res.size				  = final_size;

		const D3D12_RESOURCE_DESC resource_desc = {
			.Dimension		  = D3D12_RESOURCE_DIMENSION_BUFFER,
			.Alignment		  = 0,
			.Width			  = static_cast<uint64>(final_size),
			.Height			  = 1,
			.DepthOrArraySize = 1,
			.MipLevels		  = 1,
			.Format			  = DXGI_FORMAT_UNKNOWN,
			.SampleDesc =
				{
					.Count	 = 1,
					.Quality = 0,
				},
			.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
			.Flags	= desc.flags.is_set(resource_flags::rf_gpu_write) ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE,
		};

		D3D12MA::ALLOCATION_DESC allocation_desc = {};
		D3D12_RESOURCE_STATES	 state			 = D3D12_RESOURCE_STATE_GENERIC_READ;

		if (desc.flags.is_set(resource_flags::rf_gpu_only))
			allocation_desc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		else if (desc.flags.is_set(resource_flags::rf_cpu_visible))
		{
			allocation_desc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

			if (desc.flags.is_set(resource_flags::rf_vertex_buffer) || desc.flags.is_set(resource_flags::rf_constant_buffer))
				state = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			else if (desc.flags.is_set(resource_flags::rf_index_buffer))
				state = D3D12_RESOURCE_STATE_INDEX_BUFFER;
			else
				state = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		}
		else if (desc.flags.is_set(resource_flags::rf_gpu_write))
		{
			allocation_desc.HeapType = D3D12_HEAP_TYPE_READBACK;
			state					 = D3D12_RESOURCE_STATE_COPY_DEST;
		}

		throw_if_failed(_allocator->CreateResource(&allocation_desc, &resource_desc, state, NULL, &res.ptr, IID_NULL, NULL));

		if (desc.flags.is_set(resource_flags::rf_constant_buffer))
		{
			const D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {
				.BufferLocation = res.ptr->GetResource()->GetGPUVirtualAddress(),
				.SizeInBytes	= static_cast<UINT>(final_size),
			};
			res.descriptor_index  = static_cast<int16>(_descriptors.add());
			descriptor_handle& dh = _descriptors.get(res.descriptor_index);
			dh					  = _heap_buffer.get_heap_handle_block(1);
			_device->CreateConstantBufferView(&desc, {dh.cpu});
		}
		else if (desc.flags.is_set(resource_flags::rf_storage_buffer))
		{
			const D3D12_SHADER_RESOURCE_VIEW_DESC desc = {
				.Format					 = DXGI_FORMAT_R32_TYPELESS,
				.ViewDimension			 = D3D12_SRV_DIMENSION_BUFFER,
				.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
				.Buffer =
					{
						.FirstElement		 = 0,
						.NumElements		 = static_cast<UINT>(final_size / 4),
						.StructureByteStride = 0,
						.Flags				 = D3D12_BUFFER_SRV_FLAG_RAW,
					},

			};

			res.descriptor_index  = static_cast<int16>(_descriptors.add());
			descriptor_handle& dh = _descriptors.get(res.descriptor_index);
			dh					  = _heap_buffer.get_heap_handle_block(1);
			_device->CreateShaderResourceView(res.ptr->GetResource(), &desc, {dh.cpu});
		}
		else if (desc.flags.is_set(resource_flags::rf_gpu_write))
		{
			const D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {
				.Format		   = DXGI_FORMAT_R32_TYPELESS,
				.ViewDimension = D3D12_UAV_DIMENSION_BUFFER,
				.Buffer =
					{
						.FirstElement		  = 0,
						.NumElements		  = static_cast<UINT>(final_size / 4),
						.StructureByteStride  = 0,
						.CounterOffsetInBytes = 0,
						.Flags				  = D3D12_BUFFER_UAV_FLAG_RAW,
					},
			};
			res.descriptor_index  = static_cast<int16>(_descriptors.add());
			descriptor_handle& dh = _descriptors.get(res.descriptor_index);
			dh					  = _heap_buffer.get_heap_handle_block(1);
			_device->CreateUnorderedAccessView(res.ptr->GetResource(), NULL, &desc, {dh.cpu});
		}

		NAME_DX12_OBJECT_CSTR(res.ptr->GetResource(), desc.debug_name);

		return id;
	}

	void dx12_backend::map_resource(gfx_id id, uint8*& ptr) const
	{
		const resource& res = _resources.get(id);
		CD3DX12_RANGE	range(0, 0);
		throw_if_failed(res.ptr->GetResource()->Map(0, &range, reinterpret_cast<void**>(&ptr)));
	}

	void dx12_backend::unmap_resource(gfx_id id) const
	{
		const resource& res = _resources.get(id);
		CD3DX12_RANGE	range(0, 0);
		res.ptr->GetResource()->Unmap(0, &range);
	}

	HANDLE dx12_backend::get_shared_handle_for_texture(gfx_id id)
	{
		return _texture_shared_handles.get(_textures.get(id).shared_handle).handle;
	}

	void dx12_backend::destroy_resource(gfx_id id)
	{
		resource& res = _resources.get(id);
		if (res.descriptor_index != -1)
		{
			descriptor_handle& dh = _descriptors.get(static_cast<gfx_id>(res.descriptor_index));
			_heap_buffer.remove_handle(dh);
			_descriptors.remove(res.descriptor_index);
		}

		res.descriptor_index = -1;
		res.ptr->Release();
		_resources.remove(id);
	}

	gfx_id dx12_backend::create_texture(const texture_desc& desc)
	{
		const gfx_id id	 = _textures.add();
		texture&	 txt = _textures.get(id);

		const DXGI_FORMAT color_format = get_format(desc.texture_format);
		const DXGI_FORMAT depth_format = get_format(desc.depth_stencil_format);
		txt.format					   = static_cast<uint8>(color_format);

		D3D12_RESOURCE_DESC resource_desc = {};
		resource_desc.Dimension			  = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resource_desc.Alignment			  = 0;
		resource_desc.Width				  = static_cast<uint64>(desc.size.x);
		resource_desc.Height			  = static_cast<uint64>(desc.size.y);
		resource_desc.DepthOrArraySize	  = static_cast<uint16>(desc.array_length);
		resource_desc.MipLevels			  = static_cast<uint16>(desc.mip_levels);
		resource_desc.SampleDesc.Count	  = static_cast<uint32>(desc.samples);
		resource_desc.SampleDesc.Quality  = 0;
		resource_desc.Layout			  = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resource_desc.Flags				  = D3D12_RESOURCE_FLAG_NONE;
		resource_desc.Format			  = color_format;
		resource_desc.Dimension			  = desc.flags.is_set(texture_flags::tf_is_1d) ? D3D12_RESOURCE_DIMENSION_TEXTURE1D : (desc.flags.is_set(texture_flags::tf_is_3d) ? D3D12_RESOURCE_DIMENSION_TEXTURE3D : D3D12_RESOURCE_DIMENSION_TEXTURE2D);

		D3D12_CLEAR_VALUE  clear_value	   = {};
		D3D12_CLEAR_VALUE* clear_value_ptr = nullptr;

		if (desc.flags.is_set(texture_flags::tf_depth_texture) || desc.flags.is_set(texture_flags::tf_stencil_texture))
		{
			clear_value.Format				 = depth_format;
			clear_value.DepthStencil.Depth	 = desc.clear_values[0];
			clear_value.DepthStencil.Stencil = desc.clear_values[1];
			clear_value_ptr					 = &clear_value;
			resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		}

		if (desc.flags.is_set(texture_flags::tf_render_target))
		{
			resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			clear_value.Format	 = color_format;
			clear_value.Color[0] = desc.clear_values[0];
			clear_value.Color[1] = desc.clear_values[1];
			clear_value.Color[2] = desc.clear_values[2];
			clear_value.Color[3] = desc.clear_values[3];
			clear_value_ptr		 = &clear_value;
		}

		if (desc.samples == 1 && !desc.flags.is_set(texture_flags::tf_sampled) && !desc.flags.is_set(texture_flags::tf_sampled_outside_fragment) && (desc.flags.is_set(texture_flags::tf_depth_texture) || desc.flags.is_set(texture_flags::tf_stencil_texture)))
			resource_desc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

		const D3D12MA::ALLOCATION_DESC allocation_desc = {
			.HeapType		= D3D12_HEAP_TYPE_DEFAULT,
			.ExtraHeapFlags = desc.flags.is_set(texture_flags::tf_shared) ? D3D12_HEAP_FLAG_SHARED : D3D12_HEAP_FLAG_NONE,
		};

		const D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

		const D3D12_RESOURCE_ALLOCATION_INFO& allocation_info = _device->GetResourceAllocationInfo(0, 1, &resource_desc);

		throw_if_failed(_allocator->CreateResource(&allocation_desc, &resource_desc, state, clear_value_ptr, &txt.ptr, IID_NULL, NULL));
		NAME_DX12_OBJECT_CSTR(txt.ptr->GetResource(), desc.debug_name);

		if (desc.flags.is_set(texture_flags::tf_shared))
		{
			txt.shared_handle			  = _texture_shared_handles.add();
			texture_shared_handle& handle = _texture_shared_handles.get(txt.shared_handle);

			HANDLE sharedHandle;

			ID3D12Resource* resource = txt.ptr->GetResource();

			HRESULT hr = _device->CreateSharedHandle(resource, NULL, GENERIC_ALL, NULL, &sharedHandle);

			if (SUCCEEDED(hr))
			{
				handle.handle = sharedHandle;
				SFG_INFO("Created shared handle for: {0}", desc.debug_name);
			}
			else
			{
				throw_if_failed(hr);
			}
		}

		auto create_srv = [&](DXGI_FORMAT format, bool createForCubemap, uint32 baseArrayLayer, uint32 layerCount, uint32 baseMipLevel, uint32 mipLevels, const descriptor_handle& targetDescriptor) {
			D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
			srv_desc.Shader4ComponentMapping		 = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srv_desc.Format							 = format;

			if (createForCubemap)
			{
				srv_desc.ViewDimension				 = D3D12_SRV_DIMENSION_TEXTURECUBE;
				srv_desc.TextureCube.MipLevels		 = desc.mip_levels;
				srv_desc.TextureCube.MostDetailedMip = baseMipLevel;
				srv_desc.TextureCube.MipLevels		 = mipLevels;
			}
			else if (desc.flags.is_set(texture_flags::tf_is_1d) && desc.array_length == 1)
			{
				srv_desc.ViewDimension			   = D3D12_SRV_DIMENSION_TEXTURE1D;
				srv_desc.Texture1D.MipLevels	   = mipLevels;
				srv_desc.Texture1D.MostDetailedMip = baseMipLevel;
			}
			else if (desc.flags.is_set(texture_flags::tf_is_1d) && desc.array_length > 1)
			{
				srv_desc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
				srv_desc.Texture1DArray.MipLevels		= mipLevels;
				srv_desc.Texture1DArray.MostDetailedMip = baseMipLevel;
				srv_desc.Texture1DArray.ArraySize		= layerCount;
				srv_desc.Texture1DArray.FirstArraySlice = baseArrayLayer;
			}
			else if (desc.flags.is_set(texture_flags::tf_is_2d) && desc.array_length == 1)
			{
				srv_desc.ViewDimension			   = desc.samples > 1 ? D3D12_SRV_DIMENSION_TEXTURE2DMS : D3D12_SRV_DIMENSION_TEXTURE2D;
				srv_desc.Texture2D.MipLevels	   = mipLevels;
				srv_desc.Texture2D.MostDetailedMip = baseMipLevel;
			}
			else if (desc.flags.is_set(texture_flags::tf_is_2d) && desc.array_length > 1)
			{
				srv_desc.ViewDimension					= desc.samples > 1 ? D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
				srv_desc.Texture2DArray.MipLevels		= mipLevels;
				srv_desc.Texture2DArray.MostDetailedMip = baseMipLevel;
				srv_desc.Texture2DArray.ArraySize		= layerCount;
				srv_desc.Texture2DArray.FirstArraySlice = baseArrayLayer;
			}
			else if (desc.flags.is_set(texture_flags::tf_is_3d))
			{
				srv_desc.ViewDimension			   = D3D12_SRV_DIMENSION_TEXTURE3D;
				srv_desc.Texture3D.MipLevels	   = mipLevels;
				srv_desc.Texture3D.MostDetailedMip = baseMipLevel;
			}

			_device->CreateShaderResourceView(txt.ptr->GetResource(), &srv_desc, {targetDescriptor.cpu});
		};

		auto create_rtv = [&](DXGI_FORMAT format, uint32 baseArrayLayer, uint32 layerCount, uint32 baseMipLevel, uint32 mipLevels, const descriptor_handle& targetDescriptor) {
			D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
			rtv_desc.Format						   = format;

			if (desc.flags.is_set(texture_flags::tf_is_1d) && desc.array_length == 1)
			{
				rtv_desc.ViewDimension		= D3D12_RTV_DIMENSION_TEXTURE1D;
				rtv_desc.Texture1D.MipSlice = baseMipLevel;
			}
			else if (desc.flags.is_set(texture_flags::tf_is_1d) && desc.array_length == 1)
			{
				rtv_desc.ViewDimension					= D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
				rtv_desc.Texture1DArray.ArraySize		= layerCount;
				rtv_desc.Texture1DArray.FirstArraySlice = baseArrayLayer;
				rtv_desc.Texture1DArray.MipSlice		= baseMipLevel;
			}
			else if (desc.flags.is_set(texture_flags::tf_is_2d) && desc.array_length == 1)
			{
				rtv_desc.ViewDimension		= desc.samples > 1 ? D3D12_RTV_DIMENSION_TEXTURE2DMS : D3D12_RTV_DIMENSION_TEXTURE2D;
				rtv_desc.Texture2D.MipSlice = baseMipLevel;
			}
			else if (desc.flags.is_set(texture_flags::tf_is_2d) && desc.array_length > 1)
			{
				rtv_desc.ViewDimension					= desc.samples > 1 ? D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY : D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
				rtv_desc.Texture2DArray.ArraySize		= layerCount;
				rtv_desc.Texture2DArray.FirstArraySlice = baseArrayLayer;
				rtv_desc.Texture2DArray.MipSlice		= baseMipLevel;
			}
			else if (desc.flags.is_set(texture_flags::tf_is_3d))
			{
				rtv_desc.ViewDimension		= D3D12_RTV_DIMENSION_TEXTURE3D;
				rtv_desc.Texture3D.MipSlice = baseMipLevel;
			}

			_device->CreateRenderTargetView(txt.ptr->GetResource(), &rtv_desc, {targetDescriptor.cpu});
		};

		auto create_dsv = [&](DXGI_FORMAT format, uint32 baseArrayLayer, uint32 layerCount, uint32 baseMipLevel, uint32 mipLevels, const descriptor_handle& targetDescriptor) {
			D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
			depthStencilDesc.Format						   = format;

			if (desc.flags.is_set(texture_flags::tf_is_1d) && desc.array_length == 1)
			{
				depthStencilDesc.ViewDimension		= D3D12_DSV_DIMENSION_TEXTURE1D;
				depthStencilDesc.Texture1D.MipSlice = baseMipLevel;
			}
			else if (desc.flags.is_set(texture_flags::tf_is_1d) && desc.array_length == 1)
			{
				depthStencilDesc.ViewDimension					= D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
				depthStencilDesc.Texture1DArray.ArraySize		= layerCount;
				depthStencilDesc.Texture1DArray.FirstArraySlice = baseArrayLayer;
				depthStencilDesc.Texture1DArray.MipSlice		= baseMipLevel;
			}
			else if (desc.flags.is_set(texture_flags::tf_is_2d) && desc.array_length == 1)
			{
				depthStencilDesc.ViewDimension		= desc.samples > 1 ? D3D12_DSV_DIMENSION_TEXTURE2DMS : D3D12_DSV_DIMENSION_TEXTURE2D;
				depthStencilDesc.Texture2D.MipSlice = baseMipLevel;
			}
			else if (desc.flags.is_set(texture_flags::tf_is_2d) && desc.array_length > 1)
			{
				depthStencilDesc.ViewDimension					= desc.samples > 1 ? D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
				depthStencilDesc.Texture2DArray.ArraySize		= layerCount;
				depthStencilDesc.Texture2DArray.FirstArraySlice = baseArrayLayer;
				depthStencilDesc.Texture2DArray.MipSlice		= baseMipLevel;
			}
			else if (desc.flags.is_set(texture_flags::tf_is_3d))
			{
				SFG_ASSERT(false, "Can't create a depth Texture 3D!");
			}

			_device->CreateDepthStencilView(txt.ptr->GetResource(), &depthStencilDesc, {targetDescriptor.cpu});
		};

		const DXGI_FORMAT srv_format = (desc.flags.is_set(texture_flags::tf_depth_texture) || desc.flags.is_set(texture_flags::tf_stencil_texture)) ? depth_format : color_format;

		for (uint8 i = 0; i < static_cast<uint8>(desc.views.size()); i++)
		{
			const view_desc& view			 = desc.views[i];
			const uint32	 base_level		 = view.base_arr_level;
			const uint32	 remaining_level = view.level_count == 0 ? (desc.array_length - base_level) : view.level_count;
			const uint32	 base_mip		 = view.base_mip_level;
			const uint32	 remaining_mip	 = view.mip_count == 0 ? (desc.mip_levels - base_mip) : view.mip_count;

			if (desc.flags.is_set(texture_flags::tf_sampled) || desc.flags.is_set(texture_flags::tf_sampled_outside_fragment))
			{
				txt.srv_count++;
				txt.srvs[i]			  = _descriptors.add();
				descriptor_handle& dh = _descriptors.get(txt.srvs[i]);
				dh					  = _heap_texture.get_heap_handle_block(1);
				create_srv(srv_format, view.is_cubemap, base_level, remaining_level, base_mip, remaining_mip, dh);
			}
			if (desc.flags.is_set(texture_flags::tf_depth_texture) || desc.flags.is_set(texture_flags::tf_stencil_texture))
			{
				txt.dsv_count++;
				txt.dsvs[i]			  = _descriptors.add();
				descriptor_handle& dh = _descriptors.get(txt.dsvs[i]);
				dh					  = _heap_dsv.get_heap_handle_block(1);
				create_dsv(depth_format, base_level, remaining_level, base_mip, remaining_mip, dh);
			}
			if (desc.flags.is_set(texture_flags::tf_render_target))
			{
				txt.rtv_count++;
				txt.rtvs[i]			  = _descriptors.add();
				descriptor_handle& dh = _descriptors.get(txt.rtvs[i]);
				dh					  = _heap_rtv.get_heap_handle_block(1);
				create_rtv(color_format, base_level, remaining_level, base_mip, remaining_mip, dh);
			}
		}

		return id;
	}

	void dx12_backend::destroy_texture(gfx_id id)
	{
		texture& txt = _textures.get(id);

		texture_shared_handle& handle = _texture_shared_handles.get(txt.shared_handle);
		if (handle.handle != 0)
			CloseHandle(handle.handle);
		handle.handle = 0;

		for (uint8 i = 0; i < txt.rtv_count; i++)
		{
			const descriptor_handle& dh = _descriptors.get(txt.rtvs[i]);
			_heap_rtv.remove_handle(dh);
			_descriptors.remove(txt.rtvs[i]);
		}

		for (uint8 i = 0; i < txt.srv_count; i++)
		{
			const descriptor_handle& dh = _descriptors.get(txt.srvs[i]);
			_heap_texture.remove_handle(dh);
			_descriptors.remove(txt.srvs[i]);
		}

		for (uint8 i = 0; i < txt.dsv_count; i++)
		{
			const descriptor_handle& dh = _descriptors.get(txt.dsvs[i]);
			_heap_dsv.remove_handle(dh);
			_descriptors.remove(txt.dsvs[i]);
		}

		txt.ptr->Release();
		txt.ptr = NULL;
		_textures.remove(id);
	}

	gfx_id dx12_backend::create_sampler(const sampler_desc& desc)
	{
		const D3D12_TEXTURE_ADDRESS_MODE address_mode = get_address_mode(desc.flags);

		D3D12_SAMPLER_DESC samplerDesc = {
			.Filter			= get_filter(desc.flags),
			.AddressU		= address_mode,
			.AddressV		= address_mode,
			.AddressW		= address_mode,
			.MipLODBias		= static_cast<FLOAT>(desc.lod_bias),
			.MaxAnisotropy	= desc.anisotropy,
			.ComparisonFunc = D3D12_COMPARISON_FUNC_NONE,
			.MinLOD			= desc.min_lod,
			.MaxLOD			= desc.max_lod,
		};
		border_color(desc.flags, samplerDesc.BorderColor);

		const gfx_id id	 = _samplers.add();
		sampler&	 smp = _samplers.get(id);

		smp.descriptor_index  = _descriptors.add();
		descriptor_handle& dh = _descriptors.get(smp.descriptor_index);
		dh					  = _heap_sampler.get_heap_handle_block(1);

		_device->CreateSampler(&samplerDesc, {dh.cpu});
		return id;
	}

	void dx12_backend::destroy_sampler(gfx_id id)
	{
		sampler&		   smp = _samplers.get(id);
		descriptor_handle& dh  = _descriptors.get(smp.descriptor_index);
		_heap_sampler.remove_handle(dh);
		_descriptors.remove(smp.descriptor_index);
		_samplers.remove(id);
	}

	gfx_id dx12_backend::create_swapchain(const swapchain_desc& desc)
	{
		const gfx_id id	 = _swapchains.add();
		swapchain&	 swp = _swapchains.get(id);

		if (desc.flags.is_set(swapchain_flags::sf_vsync_every_v_blank))
			swp.vsync = 1;
		else if (desc.flags.is_set(swapchain_flags::sf_vsync_every_2v_blank))
			swp.vsync = 2;
		else
			swp.vsync = 0;

		DXGI_FORMAT swap_format = DXGI_FORMAT_B8G8R8A8_UNORM;
		if (desc.format == format::r16g16b16a16_sfloat)
		{
			swap_format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		}
		else if (desc.format == format::r8g8b8a8_unorm)
		{
			swap_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		else if (desc.format == format::r8g8b8a8_srgb)
		{
			swap_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		}

		// this is view(rtv) format
		swp.format = static_cast<uint8>(get_format(desc.format));

		const DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {
			.Width	= static_cast<UINT>(desc.size.x),
			.Height = static_cast<UINT>(desc.size.y),
			.Format = swap_format,
			.SampleDesc =
				{
					.Count = 1,
				},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = BACK_BUFFER_COUNT,
			.SwapEffect	 = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.Flags		 = desc.flags.is_set(swapchain_flags::sf_allow_tearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : (UINT)0,
		};

		ComPtr<IDXGISwapChain1> swapchain;

		throw_if_failed(_factory->CreateSwapChainForHwnd(_queues.get(_queue_graphics).ptr.Get(), (HWND)desc.window, &swapchain_desc, nullptr, nullptr, &swapchain));
		throw_if_failed(swapchain.As(&swp.ptr));

		{
			for (uint32 i = 0; i < BACK_BUFFER_COUNT; i++)
			{
				throw_if_failed(swp.ptr->GetBuffer(i, IID_PPV_ARGS(&swp.textures[i])));
				swp.rtv_indices[i]	  = _descriptors.add();
				descriptor_handle& dh = _descriptors.get(swp.rtv_indices[i]);
				dh					  = _heap_rtv.get_heap_handle_block(1);

				const D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {
					.Format		   = static_cast<DXGI_FORMAT>(swp.format),
					.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
				};

				_device->CreateRenderTargetView(swp.textures[i].Get(), &rtv_desc, {dh.cpu});
				NAME_DX12_OBJECT_CSTR(swp.textures[i], "Swapchain RTV");
			}
		}

		return id;
	}

	gfx_id dx12_backend::recreate_swapchain(const swapchain_recreate_desc& desc)
	{
		swapchain& swp = _swapchains.get(desc.swapchain);

		DXGI_SWAP_CHAIN_DESC swp_desc = {};
		swp.ptr->GetDesc(&swp_desc);

		for (uint32 i = 0; i < BACK_BUFFER_COUNT; i++)
		{
			swp.textures[i].Reset();
		}

		throw_if_failed(swp.ptr->ResizeBuffers(BACK_BUFFER_COUNT, static_cast<UINT>(desc.size.x), static_cast<UINT>(desc.size.y), swp_desc.BufferDesc.Format, desc.flags.is_set(swapchain_flags::sf_allow_tearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : (UINT)0));
		swp.image_index = swp.ptr->GetCurrentBackBufferIndex();

		for (uint32 i = 0; i < BACK_BUFFER_COUNT; i++)
		{
			descriptor_handle& dh = _descriptors.get(swp.rtv_indices[i]);

			throw_if_failed(swp.ptr->GetBuffer(i, IID_PPV_ARGS(&swp.textures[i])));

			const D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {
				.Format		   = static_cast<DXGI_FORMAT>(swp.format),
				.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
			};
			_device->CreateRenderTargetView(swp.textures[i].Get(), &rtv_desc, {dh.cpu});
			NAME_DX12_OBJECT_CSTR(swp.textures[i], "Swapchain RTV");
		}

		return desc.swapchain;
	}

	void dx12_backend::destroy_swapchain(gfx_id id)
	{
		swapchain& swp = _swapchains.get(id);

		for (uint32 i = 0; i < BACK_BUFFER_COUNT; i++)
		{
			descriptor_handle& dh = _descriptors.get(swp.rtv_indices[i]);
			_heap_rtv.remove_handle(dh);
			_descriptors.remove(swp.rtv_indices[i]);
			swp.textures[i].Reset();
		}

		swp.ptr.Reset();
		_swapchains.remove(id);
	}

	gfx_id dx12_backend::create_semaphore()
	{
		const gfx_id id	 = _semaphores.add();
		semaphore&	 sem = _semaphores.get(id);
		throw_if_failed(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&sem.ptr)));
		return id;
	}

	void dx12_backend::wait_semaphore(gfx_id id, uint64 value) const
	{
		const semaphore& sem = _semaphores.get(id);
		wait_for_fence(sem.ptr.Get(), value);
	}

	void dx12_backend::destroy_semaphore(gfx_id id)
	{
		semaphore& sem = _semaphores.get(id);
		sem.ptr.Reset();
		_semaphores.remove(id);
	}

	bool dx12_backend::compile_shader_vertex_pixel(
		const string& source, const vector<string>& defines, const char* source_path, const char* vertex_entry, const char* pixel_entry, span<uint8>& vertex_out, span<uint8>& pixel_out, bool compile_root_sig, span<uint8>& out_signature_data) const
	{
		Microsoft::WRL::ComPtr<IDxcCompiler3> idxc_compiler;
		throw_if_failed(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&idxc_compiler)));

		ComPtr<IDxcUtils> utils;
		throw_if_failed(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf())));

		UINT32					 code_page = CP_UTF8;
		ComPtr<IDxcBlobEncoding> source_blob;
		const char*				 shader_source = source.c_str();
		s_idxcLib->CreateBlobWithEncodingFromPinned((const BYTE*)shader_source, static_cast<UINT>(source.size()), code_page, &source_blob);

		DxcBuffer source_buffer;
		source_buffer.Ptr	   = source_blob->GetBufferPointer();
		source_buffer.Size	   = source_blob->GetBufferSize();
		source_buffer.Encoding = 0;

		const wchar_t* include_path = string_util::char_to_wchar(source_path);

		auto compile = [&](const wchar_t* target_profile, const wchar_t* target_entry, span<uint8>& out, bool root_sig) -> bool {
			vector<LPCWSTR> arguments = {L"-T", target_profile, L"-E", target_entry, DXC_ARG_WARNINGS_ARE_ERRORS, L"-HV 2021", L"-I", include_path};
#ifdef SFG_DEBUG
			arguments.push_back(DXC_ARG_DEBUG);
			arguments.push_back(DXC_ARG_PREFER_FLOW_CONTROL);
			arguments.push_back(DXC_ARG_SKIP_OPTIMIZATIONS);
#else
			arguments.push_back(L"-Qstrip_debug");
			arguments.push_back(L"-Qstrip_reflect");
			arguments.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
#endif

			for (const auto& def : defines)
			{
				std::wstring wstr = L"-D" + string_util::to_wstr(def);
				arguments.push_back(wcscpy(new wchar_t[wstr.size() + 1], wstr.c_str()));
			}

			ComPtr<IDxcIncludeHandler> include_handler;
			throw_if_failed(utils->CreateDefaultIncludeHandler(&include_handler));

			ComPtr<IDxcResult> result;
			throw_if_failed(idxc_compiler->Compile(&source_buffer, arguments.data(), static_cast<uint32>(arguments.size()), include_handler.Get(), IID_PPV_ARGS(result.GetAddressOf())));

#if SERIALIZE_DEBUG_INFORMATION
			ComPtr<IDxcBlob>	  debug_data;
			ComPtr<IDxcBlobUtf16> debug_data_path;
			result->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(debug_data.GetAddressOf()), debug_data_path.GetAddressOf());

			if (debug_data != NULL && debug_data_path != NULL)
			{
				const wchar_t* path = reinterpret_cast<const wchar_t*>(debug_data_path->GetBufferPointer());

				if (debug_data && path)
				{
					std::ofstream out_file(path, std::ios::binary);
					out_file.write(reinterpret_cast<const char*>(debug_data->GetBufferPointer()), debug_data->GetBufferSize());
					out_file.close();
				}
			}

#endif
			ComPtr<IDxcBlobUtf8> errors;
			result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errors.GetAddressOf()), nullptr);

			if (errors && errors->GetStringLength() > 0)
			{
				SFG_ERR("DX12 -> {0}", (char*)errors->GetStringPointer());
				return false;
			}

			HRESULT hr;
			result->GetStatus(&hr);

			if (FAILED(hr))
			{
				if (result)
				{
					ComPtr<IDxcBlobEncoding> errorsBlob;
					hr = result->GetErrorBuffer(&errorsBlob);
					if (SUCCEEDED(hr) && errorsBlob)
					{
						SFG_FATAL("DX12 -> Shader compilation failed:{0}", (const char*)errorsBlob->GetBufferPointer());
						return false;
					}
				}
			}

			if (root_sig)
			{
				ComPtr<IDxcBlob> root_sig_blob;
				result->GetOutput(DXC_OUT_ROOT_SIGNATURE, IID_PPV_ARGS(&root_sig_blob), nullptr);

				if (root_sig_blob && root_sig_blob->GetBufferSize() > 0)
				{
					out_signature_data.size = root_sig_blob->GetBufferSize();
					out_signature_data.data = new uint8[out_signature_data.size];
					SFG_MEMCPY(out_signature_data.data, root_sig_blob->GetBufferPointer(), out_signature_data.size);
				}
			}
			ComPtr<IDxcBlob> code;
			result->GetResult(&code);

			if (code.Get() != NULL)
			{
				const SIZE_T sz = code->GetBufferSize();
				out.size		= code->GetBufferSize();
				out.data		= new uint8[sz];
				SFG_MEMCPY(out.data, code->GetBufferPointer(), out.size);
			}
			else
			{
				SFG_FATAL("DX12 -> Failed compiling IDXC blob!");
				return false;
			}

			return true;
		};

		const wchar_t* target_entry_vertex = string_util::char_to_wchar(vertex_entry);
		const wchar_t* target_entry_pixel  = string_util::char_to_wchar(pixel_entry);

		auto clean = [&]() {
			source_blob.Reset();
			delete[] target_entry_vertex;
			delete[] target_entry_pixel;
			delete[] include_path;
		};

		if (!compile(L"vs_6_0", target_entry_vertex, vertex_out, compile_root_sig))
		{
			clean();
			return false;
		}

		if (!compile(L"ps_6_0", target_entry_pixel, pixel_out, false))
		{
			clean();
			return false;
		}

		clean();
		return true;
	}

	bool dx12_backend::compile_shader_compute(const string& source, const char* source_path, const char* entry, span<uint8>& out, bool compile_layout, span<uint8>& out_layout) const
	{
		Microsoft::WRL::ComPtr<IDxcCompiler3> idxc_compiler;
		throw_if_failed(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&idxc_compiler)));

		ComPtr<IDxcUtils> utils;
		throw_if_failed(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf())));

		UINT32					 code_page = CP_UTF8;
		ComPtr<IDxcBlobEncoding> source_blob;
		const char*				 shader_source = source.c_str();
		s_idxcLib->CreateBlobWithEncodingFromPinned((const BYTE*)shader_source, static_cast<UINT>(source.size()), code_page, &source_blob);

		DxcBuffer source_buffer;
		source_buffer.Ptr	   = source_blob->GetBufferPointer();
		source_buffer.Size	   = source_blob->GetBufferSize();
		source_buffer.Encoding = 0;

		const wchar_t* include_path = string_util::char_to_wchar(source_path);
		const wchar_t* entry_point	= string_util::char_to_wchar(entry);

		vector<LPCWSTR> arguments = {L"-T", L"cs_6_0", L"-E", entry_point, DXC_ARG_WARNINGS_ARE_ERRORS, L"-HV 2021", L"-I", include_path};

#ifdef SFG_DEBUG
		arguments.push_back(DXC_ARG_DEBUG);
		arguments.push_back(DXC_ARG_PREFER_FLOW_CONTROL);
		arguments.push_back(DXC_ARG_SKIP_OPTIMIZATIONS);
#else
		arguments.push_back(L"-Qstrip_debug");
		arguments.push_back(L"-Qstrip_reflect");
		arguments.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
#endif

		ComPtr<IDxcResult> result;
		throw_if_failed(idxc_compiler->Compile(&source_buffer, arguments.data(), static_cast<uint32>(arguments.size()), NULL, IID_PPV_ARGS(result.GetAddressOf())));

		delete[] entry_point;
		delete[] include_path;
#if SERIALIZE_DEBUG_INFORMATION
		ComPtr<IDxcBlob>	  debug_data;
		ComPtr<IDxcBlobUtf16> debug_data_path;
		result->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(debug_data.GetAddressOf()), debug_data_path.GetAddressOf());

		if (debug_data != NULL && debug_data_path != NULL)
		{
			const wchar_t* path = reinterpret_cast<const wchar_t*>(debug_data_path->GetBufferPointer());

			if (debug_data && path)
			{
				std::ofstream out_file(path, std::ios::binary);
				out_file.write(reinterpret_cast<const char*>(debug_data->GetBufferPointer()), debug_data->GetBufferSize());
				out_file.close();
			}
		}

#endif
		ComPtr<IDxcBlobUtf8> errors;
		result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errors.GetAddressOf()), nullptr);
		if (errors && errors->GetStringLength() > 0)
		{
			SFG_ERR("DX12 -> {0}", (char*)errors->GetStringPointer());
			return false;
		}

		HRESULT hr;
		result->GetStatus(&hr);

		if (FAILED(hr))
		{
			if (result)
			{
				ComPtr<IDxcBlobEncoding> errorsBlob;
				hr = result->GetErrorBuffer(&errorsBlob);
				if (SUCCEEDED(hr) && errorsBlob)
				{
					SFG_FATAL("DX12 -> Shader compilation failed:{0}", (const char*)errorsBlob->GetBufferPointer());
					return false;
				}
			}
		}

		if (compile_layout)
		{
			ComPtr<IDxcBlob> root_sig_blob;
			result->GetOutput(DXC_OUT_ROOT_SIGNATURE, IID_PPV_ARGS(&root_sig_blob), nullptr);

			if (root_sig_blob && root_sig_blob->GetBufferSize() > 0)
			{
				out_layout.size = root_sig_blob->GetBufferSize();
				out_layout.data = new uint8[out_layout.size];
				SFG_MEMCPY(out_layout.data, root_sig_blob->GetBufferPointer(), out_layout.size);
			}
		}
		ComPtr<IDxcBlob> code;
		result->GetResult(&code);

		if (code.Get() != NULL)
		{
			const SIZE_T sz = code->GetBufferSize();
			out.size		= code->GetBufferSize();
			out.data		= new uint8[sz];
			SFG_MEMCPY(out.data, code->GetBufferPointer(), out.size);
		}
		else
		{
			SFG_FATAL("DX12 -> Failed compiling IDXC blob!");
			return false;
		}

		source_blob.Reset();

		return true;
	}

	gfx_id dx12_backend::create_shader(const shader_desc& desc)
	{
		const gfx_id id = _shaders.add();
		shader&		 sh = _shaders.get(id);
		sh.topology		= static_cast<uint8>(get_topology(desc.topo));

		if (desc.flags.is_set(shader_flags::shf_use_embedded_layout))
		{
			throw_if_failed(_device->CreateRootSignature(0, desc.layout_data.data, desc.layout_data.size, IID_PPV_ARGS(&sh.root_signature)));
			sh.owns_root_sig = true;
		}
		else
			sh.root_signature = _bind_layouts.get(desc.layout).root_signature;

		/* Early out if compute */
		const auto it = vector_util::find_if(desc.blobs, [](const shader_blob& b) -> bool { return b.stage == shader_stage::compute; });
		if (it != desc.blobs.end())
		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC cpsd = {};
			cpsd.pRootSignature					   = sh.root_signature.Get();
			cpsd.CS								   = {it->data.data, it->data.size};
			cpsd.NodeMask						   = 0;
			_device->CreateComputePipelineState(&cpsd, IID_PPV_ARGS(&sh.ptr));
			NAME_DX12_OBJECT_CSTR(sh.ptr, desc.debug_name);
			return id;
		}

		vector<D3D12_INPUT_ELEMENT_DESC> input_layout;

		for (size_t i = 0; i < desc.inputs.size(); i++)
		{
			const vertex_input& inp = desc.inputs[i];
			input_layout.push_back({
				.SemanticName		  = inp.name.c_str(),
				.SemanticIndex		  = inp.index,
				.Format				  = get_format(inp.format),
				.InputSlot			  = inp.location,
				.AlignedByteOffset	  = static_cast<uint32>(inp.offset),
				.InputSlotClass		  = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			});
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {
			.pRootSignature	 = sh.root_signature.Get(),
			.BlendState		 = {},
			.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
			.InputLayout	 = {input_layout.data(), static_cast<UINT>(input_layout.size())},
		};

		pso_desc.BlendState						   = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pso_desc.BlendState.AlphaToCoverageEnable  = desc.flags.is_set(shader_flags::shf_enable_alpha_to_cov);
		pso_desc.BlendState.IndependentBlendEnable = false;

		const uint32 attachment_count = static_cast<uint32>(desc.attachments.size());

		for (uint32 i = 0; i < attachment_count; i++)
		{
			const shader_color_attachment& att						  = desc.attachments[i];
			pso_desc.BlendState.RenderTarget[i].BlendEnable			  = att.blend_attachment.blend_enabled;
			pso_desc.BlendState.RenderTarget[i].SrcBlend			  = get_blend(att.blend_attachment.src_color_blend_factor);
			pso_desc.BlendState.RenderTarget[i].DestBlend			  = get_blend(att.blend_attachment.dst_color_blend_factor);
			pso_desc.BlendState.RenderTarget[i].SrcBlendAlpha		  = get_blend(att.blend_attachment.src_alpha_blend_factor);
			pso_desc.BlendState.RenderTarget[i].DestBlendAlpha		  = get_blend(att.blend_attachment.dst_alpha_blend_factor);
			pso_desc.BlendState.RenderTarget[i].BlendOp				  = get_blend_op(att.blend_attachment.color_blend_op);
			pso_desc.BlendState.RenderTarget[i].BlendOpAlpha		  = get_blend_op(att.blend_attachment.alpha_blend_op);
			pso_desc.BlendState.RenderTarget[i].LogicOpEnable		  = desc.flags.is_set(shader_flags::shf_enable_blend_logic_op);
			pso_desc.BlendState.RenderTarget[i].LogicOp				  = get_logic_op(desc.blend_logic_op);
			pso_desc.BlendState.RenderTarget[i].RenderTargetWriteMask = get_color_mask(att.blend_attachment.color_comp_flags);
			pso_desc.RTVFormats[i]									  = get_format(att.format);
		}

		pso_desc.DepthStencilState.DepthEnable		   = desc.depth_stencil_desc.flags.is_set(depth_stencil_flags::dsf_depth_write);
		pso_desc.DepthStencilState.DepthWriteMask	   = desc.depth_stencil_desc.flags.is_set(depth_stencil_flags::dsf_depth_write) ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
		pso_desc.DepthStencilState.DepthFunc		   = get_compare_op(desc.depth_stencil_desc.depth_compare);
		pso_desc.DepthStencilState.StencilEnable	   = desc.depth_stencil_desc.flags.is_set(depth_stencil_flags::dsf_enable_stencil);
		pso_desc.DepthStencilState.StencilReadMask	   = desc.depth_stencil_desc.stencil_compare_mask;
		pso_desc.DepthStencilState.StencilWriteMask	   = desc.depth_stencil_desc.stencil_write_mask;
		pso_desc.DepthStencilState.FrontFace		   = {get_stencil_op(desc.depth_stencil_desc.front_stencil_state.fail_op),
														  get_stencil_op(desc.depth_stencil_desc.front_stencil_state.depth_fail_op),
														  get_stencil_op(desc.depth_stencil_desc.front_stencil_state.pass_op),
														  get_compare_op(desc.depth_stencil_desc.front_stencil_state.compare_op)};
		pso_desc.DepthStencilState.BackFace			   = {get_stencil_op(desc.depth_stencil_desc.front_stencil_state.fail_op),
														  get_stencil_op(desc.depth_stencil_desc.front_stencil_state.depth_fail_op),
														  get_stencil_op(desc.depth_stencil_desc.front_stencil_state.pass_op),
														  get_compare_op(desc.depth_stencil_desc.front_stencil_state.compare_op)};
		pso_desc.SampleMask							   = UINT_MAX;
		pso_desc.SampleDesc.Count					   = desc.samples;
		pso_desc.PrimitiveTopologyType				   = get_topology_type(desc.topo);
		pso_desc.NumRenderTargets					   = attachment_count;
		pso_desc.RasterizerState.FillMode			   = D3D12_FILL_MODE_SOLID;
		pso_desc.RasterizerState.CullMode			   = get_cull_mode(desc.cull);
		pso_desc.RasterizerState.FrontCounterClockwise = desc.front == front_face::ccw;
		pso_desc.RasterizerState.DepthBias			   = static_cast<uint32>(desc.depth_bias_constant);
		pso_desc.RasterizerState.DepthBiasClamp		   = desc.depth_bias_clamp;
		pso_desc.RasterizerState.SlopeScaledDepthBias  = desc.depth_bias_slope;
		pso_desc.DSVFormat							   = get_format(desc.depth_stencil_desc.attachment_format);

		for (const shader_blob& bl : desc.blobs)
		{
			const void*	 byte_code = (void*)bl.data.data;
			const SIZE_T length	   = static_cast<SIZE_T>(bl.data.size);

			if (bl.stage == shader_stage::vertex)
			{
				pso_desc.VS.pShaderBytecode = byte_code;
				pso_desc.VS.BytecodeLength	= length;
			}
			else if (bl.stage == shader_stage::fragment)
			{
				pso_desc.PS.pShaderBytecode = byte_code;
				pso_desc.PS.BytecodeLength	= length;
			}
			else
			{
				SFG_ASSERT(false);
			}
		}

		throw_if_failed(_device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&sh.ptr)));
		NAME_DX12_OBJECT_CSTR(sh.ptr, desc.debug_name);
		return id;
	}

	void dx12_backend::destroy_shader(gfx_id id)
	{
		shader& sh = _shaders.get(id);

		if (sh.owns_root_sig)
			sh.root_signature.Reset();
		sh.ptr.Reset();
		_shaders.remove(id);
	}

	gfx_id dx12_backend::create_empty_bind_group()
	{
		const gfx_id id = _bind_groups.add();
		return id;
	}

	void dx12_backend::bind_group_add_descriptor(gfx_id group, uint8 root_param_index, uint8 type)
	{
		bind_group& bind_group = _bind_groups.get(group);
		bind_group.bindings.push_back({});
		group_binding& gbinding	  = bind_group.bindings.back();
		gbinding.descriptor_index = _descriptors.add();
		gbinding.root_param_index = root_param_index;
		gbinding.count			  = 1;
		gbinding.binding_type	  = type;

		const binding_type tp = static_cast<binding_type>(type);
		SFG_ASSERT(tp != binding_type::pointer && tp != binding_type::sampler);
	}

	void dx12_backend::bind_group_add_constant(gfx_id group, uint8 root_param_index, uint8* data, uint8 count)
	{
		bind_group& bind_group = _bind_groups.get(group);
		bind_group.bindings.push_back({});
		group_binding& gbinding	  = bind_group.bindings.back();
		gbinding.descriptor_index = _descriptors.add();
		gbinding.root_param_index = root_param_index;
		gbinding.count			  = count;
		gbinding.binding_type	  = binding_type::constant;
		gbinding.constants		  = data;
	}

	void dx12_backend::bind_group_add_pointer(gfx_id group, uint8 root_param_index, uint8 count, bool is_sampler)
	{
		bind_group& bind_group = _bind_groups.get(group);
		bind_group.bindings.push_back({});
		group_binding& gbinding	  = bind_group.bindings.back();
		gbinding.descriptor_index = _descriptors.add();
		gbinding.root_param_index = root_param_index;
		gbinding.count			  = count;
		gbinding.binding_type	  = binding_type::pointer;

		if (is_sampler)
		{
			descriptor_handle& dh = _descriptors.get(gbinding.descriptor_index);
			dh					  = _heap_gpu_sampler.get_heap_handle_block(count);
		}
		else
		{
			descriptor_handle& dh = _descriptors.get(gbinding.descriptor_index);
			dh					  = _heap_gpu_buffer.get_heap_handle_block(count);
		}
	}

	void dx12_backend::bind_group_update_constants(gfx_id id, uint8 binding_index, uint8* constants, uint8 count)
	{
		bind_group&	   group   = _bind_groups.get(id);
		group_binding& binding = group.bindings[binding_index];
		binding.constants	   = constants;
		binding.count		   = count;
	}

	void dx12_backend::bind_group_update_descriptor(gfx_id id, uint8 binding_index, gfx_id res_id)
	{
		bind_group&	   group   = _bind_groups.get(id);
		group_binding& binding = group.bindings[binding_index];

		descriptor_handle& dh  = _descriptors.get(binding.descriptor_index);
		const resource&	   res = _resources.get(res_id);
		dh.gpu				   = res.ptr->GetResource()->GetGPUVirtualAddress();
	}

	void dx12_backend::bind_group_update_pointer(gfx_id id, uint8 binding_index, const vector<bind_group_pointer>& updates)
	{
		bind_group&	   group   = _bind_groups.get(id);
		group_binding& binding = group.bindings[binding_index];

		_reuse_dest_descriptors_buffer.resize(0);
		_reuse_dest_descriptors_sampler.resize(0);
		_reuse_src_descriptors_buffer.resize(0);
		_reuse_src_descriptors_sampler.resize(0);

		descriptor_handle& binding_dh = _descriptors.get(binding.descriptor_index);

		for (const bind_group_pointer& p : updates)
		{
			if (p.type == binding_type::texture_binding)
			{
				const texture& txt = _textures.get(p.resource);
				SFG_ASSERT(txt.srv_count > p.view);
				const descriptor_handle& dh = _descriptors.get(txt.srvs[p.view]);
				_reuse_src_descriptors_buffer.push_back({dh.cpu});
				_reuse_dest_descriptors_buffer.push_back({binding_dh.cpu + p.pointer_index * _heap_gpu_buffer.get_descriptor_size()});
			}
			else if (p.type == binding_type::sampler)
			{
				const sampler&			 smp = _samplers.get(p.resource);
				const descriptor_handle& dh	 = _descriptors.get(smp.descriptor_index);
				_reuse_src_descriptors_sampler.push_back({dh.cpu});
				_reuse_dest_descriptors_sampler.push_back({binding_dh.cpu + p.pointer_index * _heap_gpu_sampler.get_descriptor_size()});
			}
			else if (p.type == binding_type::ubo || p.type == binding_type::ssbo || p.type == binding_type::uav)
			{
				const resource&	   res = _resources.get(p.resource);
				descriptor_handle& dh  = _descriptors.get(res.descriptor_index);
				_reuse_src_descriptors_buffer.push_back({dh.cpu});
				_reuse_dest_descriptors_buffer.push_back({binding_dh.cpu + p.pointer_index * _heap_gpu_buffer.get_descriptor_size()});
			}
			else
			{
				SFG_ASSERT(false);
			}
		}

		const uint32 descriptor_count_buffer  = static_cast<uint32>(_reuse_dest_descriptors_buffer.size());
		const uint32 descriptor_count_sampler = static_cast<uint32>(_reuse_dest_descriptors_sampler.size());

		if (descriptor_count_buffer != 0)
			_device->CopyDescriptors(descriptor_count_buffer, _reuse_dest_descriptors_buffer.data(), NULL, descriptor_count_buffer, _reuse_src_descriptors_buffer.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		if (descriptor_count_sampler != 0)
			_device->CopyDescriptors(descriptor_count_sampler, _reuse_dest_descriptors_sampler.data(), NULL, descriptor_count_sampler, _reuse_src_descriptors_sampler.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	}

	void dx12_backend::destroy_bind_group(gfx_id id)
	{
		bind_group& group = _bind_groups.get(id);

		const uint8 sz = static_cast<uint8>(group.bindings.size());
		for (uint8 i = 0; i < sz; i++)
		{
			const group_binding& binding = group.bindings[i];

			const binding_type type = static_cast<binding_type>(binding.binding_type);

			if (type == binding_type::sampler)
			{
				const descriptor_handle& dh = _descriptors.get(binding.descriptor_index);
				_heap_gpu_sampler.remove_handle(dh);
			}
			else if (type == binding_type::pointer)
			{
				const descriptor_handle& dh = _descriptors.get(binding.descriptor_index);
				_heap_gpu_buffer.remove_handle(dh);
			}

			_descriptors.remove(binding.descriptor_index);
		}

		_bind_groups.remove(id);
	}

	gfx_id dx12_backend::create_command_buffer(const command_buffer_desc& desc)
	{
		const gfx_id	   id		= _command_buffers.add();
		const gfx_id	   alloc_id = create_command_allocator(static_cast<uint8>(desc.type));
		command_allocator& alloc	= _command_allocators.get(alloc_id);
		command_buffer&	   cmd		= _command_buffers.get(id);
		throw_if_failed(_device->CreateCommandList(0, get_command_type(desc.type), alloc.ptr.Get(), nullptr, IID_PPV_ARGS(cmd.ptr.GetAddressOf())));
		cmd.allocator	= alloc_id;
		cmd.is_transfer = desc.type == command_type::transfer;
		cmd.ptr->Close();
		NAME_DX12_OBJECT_CSTR(cmd.ptr, desc.debug_name);
		return id;
	}

	void dx12_backend::destroy_command_buffer(gfx_id id)
	{
		command_buffer& cmd = _command_buffers.get(id);
		cmd.ptr.Reset();
		destroy_command_allocator(cmd.allocator);
		_command_buffers.remove(id);
	}

	gfx_id dx12_backend::create_command_allocator(uint8 type)
	{
		const gfx_id	   id	 = _command_allocators.add();
		command_allocator& alloc = _command_allocators.get(id);
		throw_if_failed(_device->CreateCommandAllocator(get_command_type(static_cast<command_type>(type)), IID_PPV_ARGS(alloc.ptr.GetAddressOf())));
		return id;
	}

	void dx12_backend::destroy_command_allocator(gfx_id id)
	{
		command_allocator& alloc = _command_allocators.get(id);
		alloc.ptr.Reset();
		_command_allocators.remove(id);
	}

	gfx_id dx12_backend::create_queue(const queue_desc& desc)
	{
		const gfx_id id = _queues.add();
		queue&		 q	= _queues.get(id);

		const D3D12_COMMAND_QUEUE_DESC queue_desc = {
			.Type  = get_command_type(desc.type),
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
		};
		throw_if_failed(_device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&q.ptr)));
		NAME_DX12_OBJECT_CSTR(q.ptr, desc.debug_name);
		return id;
	}

	gfx_id dx12_backend::create_empty_bind_layout()
	{
		const gfx_id id = _bind_layouts.add();
		_reuse_root_params.resize(0);
		_reuse_static_samplers.resize(0);
		_reuse_root_ranges.resize(0);
		return id;
	}

	void dx12_backend::bind_layout_add_constant(gfx_id layout, uint32 count, uint32 set, uint32 binding, uint8 vis)
	{
		const D3D12_SHADER_VISIBILITY visibility = get_visibility(static_cast<shader_stage>(vis));
		_reuse_root_params.push_back({});
		CD3DX12_ROOT_PARAMETER1& param = _reuse_root_params.back();
		param.InitAsConstants(count, binding, set, visibility);
	}

	void dx12_backend::bind_layout_add_descriptor(gfx_id layout, uint8 type, uint32 set, uint32 binding, uint8 vis)
	{
		const D3D12_SHADER_VISIBILITY visibility = get_visibility(static_cast<shader_stage>(vis));
		_reuse_root_params.push_back({});
		CD3DX12_ROOT_PARAMETER1& param = _reuse_root_params.back();

		const binding_type tp = static_cast<binding_type>(type);

		if (tp == binding_type::ubo)
			param.InitAsConstantBufferView(binding, set, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, visibility);
		else if (tp == binding_type::ssbo)
			param.InitAsShaderResourceView(binding, set, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, visibility);
		else if (tp == binding_type::uav)
			param.InitAsUnorderedAccessView(binding, set, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, visibility);
		else
			SFG_ASSERT(false);
	}

	void dx12_backend::bind_layout_add_pointer(gfx_id layout, const vector<bind_layout_pointer_param>& pointer_params, uint8 vis)
	{
		const uint32 start = static_cast<uint32>(_reuse_root_ranges.size());

		UINT offset = 0;

		for (const bind_layout_pointer_param& p : pointer_params)
		{
			_reuse_root_ranges.push_back({});
			CD3DX12_DESCRIPTOR_RANGE1& range = _reuse_root_ranges.back();

			const D3D12_DESCRIPTOR_RANGE_FLAGS flags = p.is_volatile ? D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE : D3D12_DESCRIPTOR_RANGE_FLAG_NONE;

			if (p.type == binding_type::ubo)
			{
				range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, p.count, p.binding, p.set, flags, offset);
			}
			else if (p.type == binding_type::ssbo || p.type == binding_type::texture_binding)
			{
				range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, p.count, p.binding, p.set, flags, offset);
			}
			else if (p.type == binding_type::uav)
			{
				range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, p.count, p.binding, p.set, flags, offset);
			}
			else if (p.type == binding_type::sampler)
			{
				range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, p.count, p.binding, p.set, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, offset);
			}

			offset += p.count;
		}

		const uint32				  size_now	 = static_cast<uint32>(_reuse_root_ranges.size());
		const D3D12_SHADER_VISIBILITY visibility = get_visibility(static_cast<shader_stage>(vis));
		_reuse_root_params.push_back({});
		CD3DX12_ROOT_PARAMETER1& param = _reuse_root_params.back();
		param.InitAsDescriptorTable(size_now - start, &_reuse_root_ranges[start], visibility);
	}

	void dx12_backend::bind_layout_add_immutable_sampler(gfx_id layout, uint32 set, uint32 binding, const sampler_desc& desc, uint8 vis)
	{
		const D3D12_TEXTURE_ADDRESS_MODE address_mode = get_address_mode(desc.flags);
		const D3D12_SHADER_VISIBILITY	 visibility	  = get_visibility(static_cast<shader_stage>(vis));

		_reuse_static_samplers.push_back({
			.Filter			  = get_filter(desc.flags),
			.AddressU		  = address_mode,
			.AddressV		  = address_mode,
			.AddressW		  = address_mode,
			.MipLODBias		  = static_cast<FLOAT>(desc.lod_bias),
			.MaxAnisotropy	  = desc.anisotropy,
			.ComparisonFunc	  = D3D12_COMPARISON_FUNC_NONE,
			.MinLOD			  = desc.min_lod,
			.MaxLOD			  = desc.max_lod,
			.ShaderRegister	  = binding,
			.RegisterSpace	  = set,
			.ShaderVisibility = visibility,
		});
	}

	void dx12_backend::finalize_bind_layout(gfx_id id, bool is_compute, const char* name)
	{
		bind_layout&								layout = _bind_layouts.get(id);
		const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSig(static_cast<uint32>(_reuse_root_params.size()),
															_reuse_root_params.data(),
															static_cast<uint32>(_reuse_static_samplers.size()),
															_reuse_static_samplers.data(),
															is_compute ? D3D12_ROOT_SIGNATURE_FLAG_NONE : D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		ComPtr<ID3DBlob>							signature = nullptr;
		ComPtr<ID3DBlob>							error	  = nullptr;
		const HRESULT								res		  = D3D12SerializeVersionedRootSignature(&rootSig, &signature, &error);

		if (FAILED(res) && error)
		{
			const char* error_msg = static_cast<const char*>(error->GetBufferPointer());
			SFG_ERR("{0}", error_msg);
			throw_if_failed(res);
		}

		throw_if_failed(_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&layout.root_signature)));
		NAME_DX12_OBJECT_CSTR(layout.root_signature, name);
		signature.Reset();
		error.Reset();
		_reuse_root_params.resize(0);
		_reuse_static_samplers.resize(0);
		_reuse_root_ranges.resize(0);
	}

	void dx12_backend::destroy_bind_layout(gfx_id id)
	{
		bind_layout& layout = _bind_layouts.get(id);
		layout.root_signature.Reset();
		_bind_layouts.remove(id);
	}

	void dx12_backend::destroy_queue(gfx_id id)
	{
		queue& q = _queues.get(id);
		q.ptr.Reset();
		_queues.remove(id);
	}

	void dx12_backend::wait_for_fence(ID3D12Fence* fence, uint64 value) const
	{
		const UINT64 last_fence = fence->GetCompletedValue();

		if (last_fence < value)
		{
			try
			{
				HANDLE event_handle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
				if (event_handle == nullptr)
				{
					throw_if_failed(HRESULT_FROM_WIN32(GetLastError()));
				}
				else
				{
					throw_if_failed(fence->SetEventOnCompletion(value, event_handle));
					WaitForSingleObject(event_handle, INFINITE);
					CloseHandle(event_handle);
				}
			}
			catch (HrException e)
			{
				SFG_ERR("Error while waiting for fence! {0}", e.what());
			}
		}
	}

	void dx12_backend::cmd_begin_render_pass(gfx_id cmd_id, const command_begin_render_pass& cmd)
	{
		command_buffer&				buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();

		_reuse_color_attachments.resize(cmd.color_attachment_count);

		for (uint32 i = 0; i < cmd.color_attachment_count; i++)
		{
			const render_pass_color_attachment& att = cmd.color_attachments[i];
			const texture&						txt = _textures.get(att.texture);
			const descriptor_handle&			dh	= _descriptors.get(txt.rtvs[att.view_index]);

			CD3DX12_CLEAR_VALUE cv;
			cv.Format	= static_cast<DXGI_FORMAT>(txt.format);
			cv.Color[0] = att.clear_color.x;
			cv.Color[1] = att.clear_color.y;
			cv.Color[2] = att.clear_color.z;
			cv.Color[3] = att.clear_color.w;
			const D3D12_RENDER_PASS_BEGINNING_ACCESS color_begin{get_load_op(att.load_op), {cv}};
			const D3D12_RENDER_PASS_ENDING_ACCESS	 color_end{get_store_op(att.store_op), {}};

			_reuse_color_attachments[i] = {dh.cpu, color_begin, color_end};
		}

		cmd_list->BeginRenderPass(cmd.color_attachment_count, _reuse_color_attachments.data(), NULL, D3D12_RENDER_PASS_FLAG_NONE);
	}

	void dx12_backend::cmd_begin_render_pass_depth(gfx_id cmd_id, const command_begin_render_pass_depth& cmd)
	{
		command_buffer&				buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		_reuse_color_attachments.resize(cmd.color_attachment_count);

		for (uint32 i = 0; i < cmd.color_attachment_count; i++)
		{
			const render_pass_color_attachment& att = cmd.color_attachments[i];
			const texture&						txt = _textures.get(att.texture);
			const descriptor_handle&			dh	= _descriptors.get(txt.rtvs[att.view_index]);

			CD3DX12_CLEAR_VALUE cv;
			cv.Format	= static_cast<DXGI_FORMAT>(txt.format);
			cv.Color[0] = att.clear_color.x;
			cv.Color[1] = att.clear_color.y;
			cv.Color[2] = att.clear_color.z;
			cv.Color[3] = att.clear_color.w;
			const D3D12_RENDER_PASS_BEGINNING_ACCESS color_begin{get_load_op(att.load_op), {cv}};
			const D3D12_RENDER_PASS_ENDING_ACCESS	 color_end{get_store_op(att.store_op), {}};

			_reuse_color_attachments[i] = {dh.cpu, color_begin, color_end};
		}

		const texture&							   depth_txt = _textures.get(cmd.depth_stencil_attachment.texture);
		const descriptor_handle&				   depth_dh	 = _descriptors.get(depth_txt.dsvs[cmd.depth_stencil_attachment.view_index]);
		const CD3DX12_CLEAR_VALUE				   clear_depth_stencil{static_cast<DXGI_FORMAT>(depth_txt.format), cmd.depth_stencil_attachment.clear_depth, cmd.depth_stencil_attachment.clear_stencil};
		const D3D12_RENDER_PASS_BEGINNING_ACCESS   depth_begin{get_load_op(cmd.depth_stencil_attachment.depth_load_op), {clear_depth_stencil}};
		const D3D12_RENDER_PASS_BEGINNING_ACCESS   stencil_begin{get_load_op(cmd.depth_stencil_attachment.stencil_load_op), {clear_depth_stencil}};
		const D3D12_RENDER_PASS_ENDING_ACCESS	   depth_end{get_store_op(cmd.depth_stencil_attachment.depth_store_op), {}};
		const D3D12_RENDER_PASS_ENDING_ACCESS	   stencil_end{get_store_op(cmd.depth_stencil_attachment.stencil_store_op), {}};
		const D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depth_stencil_desc{depth_dh.cpu, depth_begin, stencil_begin, depth_end, stencil_end};
		cmd_list->BeginRenderPass(cmd.color_attachment_count, _reuse_color_attachments.data(), &depth_stencil_desc, D3D12_RENDER_PASS_FLAG_NONE);
	}

	void dx12_backend::cmd_begin_render_pass_swapchain(gfx_id cmd_id, const command_begin_render_pass_swapchain& cmd)
	{
		command_buffer&				buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		_reuse_color_attachments.resize(cmd.color_attachment_count);

		for (uint32 i = 0; i < cmd.color_attachment_count; i++)
		{
			const render_pass_color_attachment& att = cmd.color_attachments[i];
			const swapchain&					swp = _swapchains.get(att.texture);
			const descriptor_handle&			dh	= _descriptors.get(swp.rtv_indices[swp.image_index]);

			CD3DX12_CLEAR_VALUE cv;
			cv.Format	= static_cast<DXGI_FORMAT>(swp.format);
			cv.Color[0] = att.clear_color.x;
			cv.Color[1] = att.clear_color.y;
			cv.Color[2] = att.clear_color.z;
			cv.Color[3] = att.clear_color.w;
			const D3D12_RENDER_PASS_BEGINNING_ACCESS color_begin{get_load_op(att.load_op), {cv}};
			const D3D12_RENDER_PASS_ENDING_ACCESS	 color_end{get_store_op(att.store_op), {}};

			_reuse_color_attachments[i] = {dh.cpu, color_begin, color_end};
		}

		cmd_list->BeginRenderPass(cmd.color_attachment_count, _reuse_color_attachments.data(), NULL, D3D12_RENDER_PASS_FLAG_NONE);
	}

	void dx12_backend::cmd_begin_render_pass_swapchain_depth(gfx_id cmd_id, const command_begin_render_pass_swapchain_depth& cmd)
	{
		command_buffer&				buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		_reuse_color_attachments.resize(cmd.color_attachment_count);

		for (uint32 i = 0; i < cmd.color_attachment_count; i++)
		{
			const render_pass_color_attachment& att = cmd.color_attachments[i];
			const swapchain&					swp = _swapchains.get(att.texture);
			const descriptor_handle&			dh	= _descriptors.get(swp.rtv_indices[swp.image_index]);

			CD3DX12_CLEAR_VALUE cv;
			cv.Format	= static_cast<DXGI_FORMAT>(swp.format);
			cv.Color[0] = att.clear_color.x;
			cv.Color[1] = att.clear_color.y;
			cv.Color[2] = att.clear_color.z;
			cv.Color[3] = att.clear_color.w;
			const D3D12_RENDER_PASS_BEGINNING_ACCESS color_begin{get_load_op(att.load_op), {cv}};
			const D3D12_RENDER_PASS_ENDING_ACCESS	 color_end{get_store_op(att.store_op), {}};

			_reuse_color_attachments[i] = {dh.cpu, color_begin, color_end};
		}

		const texture&							   depth_txt = _textures.get(cmd.depth_stencil_attachment.texture);
		const descriptor_handle&				   depth_dh	 = _descriptors.get(depth_txt.dsvs[cmd.depth_stencil_attachment.view_index]);
		const CD3DX12_CLEAR_VALUE				   clear_depth_stencil{static_cast<DXGI_FORMAT>(depth_txt.format), cmd.depth_stencil_attachment.clear_depth, cmd.depth_stencil_attachment.clear_stencil};
		const D3D12_RENDER_PASS_BEGINNING_ACCESS   depth_begin{get_load_op(cmd.depth_stencil_attachment.depth_load_op), {clear_depth_stencil}};
		const D3D12_RENDER_PASS_BEGINNING_ACCESS   stencil_begin{get_load_op(cmd.depth_stencil_attachment.stencil_load_op), {clear_depth_stencil}};
		const D3D12_RENDER_PASS_ENDING_ACCESS	   depth_end{get_store_op(cmd.depth_stencil_attachment.depth_store_op), {}};
		const D3D12_RENDER_PASS_ENDING_ACCESS	   stencil_end{get_store_op(cmd.depth_stencil_attachment.stencil_store_op), {}};
		const D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depth_stencil_desc{depth_dh.cpu, depth_begin, stencil_begin, depth_end, stencil_end};
		cmd_list->BeginRenderPass(cmd.color_attachment_count, _reuse_color_attachments.data(), &depth_stencil_desc, D3D12_RENDER_PASS_FLAG_NONE);
	}

	void dx12_backend::cmd_end_render_pass(gfx_id cmd_id, const command_end_render_pass& cmd) const
	{
		const command_buffer&		buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		cmd_list->EndRenderPass();
	}

	void dx12_backend::cmd_set_scissors(gfx_id cmd_id, const command_set_scissors& cmd) const
	{
		const command_buffer&		buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		D3D12_RECT					sc;
		sc.left	  = static_cast<LONG>(cmd.x);
		sc.top	  = static_cast<LONG>(cmd.y);
		sc.right  = static_cast<LONG>(cmd.x + cmd.width);
		sc.bottom = static_cast<LONG>(cmd.y + cmd.height);
		cmd_list->RSSetScissorRects(1, &sc);
	}

	void dx12_backend::cmd_set_viewport(gfx_id cmd_id, const command_set_viewport& cmd) const
	{
		const command_buffer&		buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		D3D12_VIEWPORT				vp;
		vp.MinDepth = cmd.min_depth;
		vp.MaxDepth = cmd.max_depth;
		vp.Height	= static_cast<float>(cmd.height);
		vp.Width	= static_cast<float>(cmd.width);
		vp.TopLeftX = cmd.x;
		vp.TopLeftY = cmd.y;
		cmd_list->RSSetViewports(1, &vp);
	}

	void dx12_backend::cmd_bind_pipeline(gfx_id cmd_id, const command_bind_pipeline& cmd) const
	{
		const command_buffer&		buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		const shader&				sh		 = _shaders.get(cmd.pipeline);

		cmd_list->SetPipelineState(sh.ptr.Get());
		cmd_list->IASetPrimitiveTopology(static_cast<D3D12_PRIMITIVE_TOPOLOGY>(sh.topology));
	}

	void dx12_backend::cmd_bind_pipeline_compute(gfx_id cmd_id, const command_bind_pipeline_compute& cmd) const
	{
		const command_buffer&		buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		const shader&				sh		 = _shaders.get(cmd.pipeline);
		cmd_list->SetPipelineState(sh.ptr.Get());
	}

	void dx12_backend::cmd_draw_instanced(gfx_id cmd_id, const command_draw_instanced& cmd) const
	{
		const command_buffer&		buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		cmd_list->DrawInstanced(cmd.vertex_count_per_instance, cmd.instance_count, cmd.start_vertex_location, cmd.start_instance_location);
	}

	void dx12_backend::cmd_draw_indexed_instanced(gfx_id cmd_id, const command_draw_indexed_instanced& cmd) const
	{
		const command_buffer&		buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		cmd_list->DrawIndexedInstanced(cmd.index_count_per_instance, cmd.instance_count, cmd.start_index_location, cmd.base_vertex_location, cmd.start_instance_location);
	}

	void dx12_backend::cmd_draw_indexed_indirect(gfx_id cmd_id, const command_draw_indexed_indirect& cmd) const
	{
		const command_buffer&		buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		cmd_list->ExecuteIndirect(_indirect_signatures.get(cmd.indirect_signature).signature.Get(), cmd.count, _resources.get(cmd.indirect_buffer).ptr->GetResource(), cmd.indirect_buffer_offset, NULL, 0);
	}

	void dx12_backend::cmd_draw_indirect(gfx_id cmd_id, const command_draw_indirect& cmd) const
	{
		const command_buffer&		buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		cmd_list->ExecuteIndirect(_indirect_signatures.get(cmd.indirect_signature).signature.Get(), cmd.count, _resources.get(cmd.indirect_buffer).ptr->GetResource(), cmd.indirect_buffer_offset, NULL, 0);
	}

	void dx12_backend::cmd_bind_vertex_buffers(gfx_id cmd_id, const command_bind_vertex_buffers& cmd) const
	{
		const command_buffer&		   buffer	= _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4*	   cmd_list = buffer.ptr.Get();
		const resource&				   res		= _resources.get(cmd.buffer);
		const D3D12_VERTEX_BUFFER_VIEW view		= {
				.BufferLocation = res.ptr->GetResource()->GetGPUVirtualAddress(),
				.SizeInBytes	= static_cast<uint32>(res.size),
				.StrideInBytes	= static_cast<uint32>(cmd.vertex_size),
		};

		cmd_list->IASetVertexBuffers(cmd.slot, 1, &view);
	}

	void dx12_backend::cmd_bind_index_buffers(gfx_id cmd_id, const command_bind_index_buffers& cmd) const
	{
		const command_buffer&		buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		const resource&				res		 = _resources.get(cmd.buffer);

		const D3D12_INDEX_BUFFER_VIEW view = {
			.BufferLocation = res.ptr->GetResource()->GetGPUVirtualAddress(),
			.SizeInBytes	= static_cast<uint32>(res.size),
			.Format			= cmd.bit_depth == 16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT,
		};

		cmd_list->IASetIndexBuffer(&view);
	}

	void dx12_backend::cmd_copy_resource(gfx_id cmd_id, const command_copy_resource& cmd) const
	{
		const command_buffer&		buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		const resource&				src_res	 = _resources.get(cmd.source);
		const resource&				dest_res = _resources.get(cmd.destination);
		cmd_list->CopyResource(dest_res.ptr->GetResource(), src_res.ptr->GetResource());
	}

	uint32 dx12_backend::get_texture_size(uint32 width, uint32 height, uint32 bpp) const
	{
		const uint32 row_pitch	 = static_cast<uint32>((width * bpp + (D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1)) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1));
		const uint32 slice_pitch = row_pitch * height;
		return slice_pitch;
	}

	uint32 dx12_backend::align_texture_size(uint32 size) const
	{
		return (size + D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - 1) & ~(D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - 1);
	}

	void* dx12_backend::adjust_buffer_pitch(void* data, uint32 width, uint32 height, uint8 bpp, uint32& out_total_size) const
	{
		const uint32 _bpp	   = static_cast<uint32>(bpp);
		const uint32 alignment = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
		const uint32 row_pitch = (width * _bpp + (alignment - 1)) & ~(alignment - 1);
		out_total_size		   = row_pitch * height;
		char* buffer		   = reinterpret_cast<char*>(new uint8[out_total_size]);
		char* src			   = reinterpret_cast<char*>(data);
		char* dst			   = buffer;

		if (dst != 0)
		{
			for (uint32 i = 0; i < height; ++i)
			{
				SFG_MEMCPY(dst, src, width * _bpp);
				dst += row_pitch;
				src += width * _bpp;
			}
		}

		return buffer;
	}

	void dx12_backend::cmd_copy_buffer_to_texture(gfx_id cmd_id, const command_copy_buffer_to_texture& cmd)
	{
		command_buffer&				buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		_reuse_subresource_data.resize(0);
		for (uint8 i = 0; i < cmd.mip_levels; i++)
		{
			const texture_buffer& tb		= cmd.textures[i];
			const LONG_PTR		  row_pitch = static_cast<LONG_PTR>((tb.size.x * tb.bpp + (D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1)) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1));

			const D3D12_SUBRESOURCE_DATA texture_data = {
				.pData		= tb.pixels,
				.RowPitch	= row_pitch,
				.SlicePitch = row_pitch * static_cast<LONG_PTR>(tb.size.y),
			};

			_reuse_subresource_data.push_back(texture_data);
		}

		resource& res = _resources.get(cmd.intermediate_buffer);
		texture&  txt = _textures.get(cmd.destination_texture);
		UpdateSubresources(cmd_list, txt.ptr->GetResource(), res.ptr->GetResource(), 0, cmd.mip_levels * cmd.destination_slice, cmd.mip_levels, _reuse_subresource_data.data());
	}

	void dx12_backend::cmd_copy_texture_to_buffer(gfx_id cmd_id, const command_copy_texture_to_buffer& cmd) const
	{
		const command_buffer&			  buffer		= _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4*		  cmd_list		= buffer.ptr.Get();
		const texture&					  txt			= _textures.get(cmd.src_texture);
		const resource&					  res			= _resources.get(cmd.dest_buffer);
		const D3D12_TEXTURE_COPY_LOCATION dest_location = {
			.pResource = res.ptr->GetResource(),
			.Type	   = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
			.PlacedFootprint =
				{
					.Offset = 0,
					.Footprint =
						{
							.Format	  = static_cast<DXGI_FORMAT>(txt.format),
							.Width	  = static_cast<UINT>(cmd.size.x),
							.Height	  = static_cast<UINT>(cmd.size.y),
							.Depth	  = 1,
							.RowPitch = static_cast<UINT>(cmd.size.x * cmd.bpp),
						},
				},
		};

		const D3D12_TEXTURE_COPY_LOCATION src_location = {
			.pResource		  = txt.ptr->GetResource(),
			.Type			  = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
			.SubresourceIndex = D3D12CalcSubresource(cmd.src_mip, cmd.src_layer, 0, 1, 1),
		};

		cmd_list->CopyTextureRegion(&dest_location, 0, 0, 0, &src_location, NULL);
	}

	void dx12_backend::cmd_copy_texture_to_texture(gfx_id cmd_id, const command_copy_texture_to_texture& cmd) const
	{
		const command_buffer&		buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		const texture&				src		 = _textures.get(cmd.source);
		const texture&				dst		 = _textures.get(cmd.destination);

		const D3D12_TEXTURE_COPY_LOCATION src_location = {
			.pResource		  = src.ptr->GetResource(),
			.Type			  = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
			.SubresourceIndex = static_cast<UINT>(cmd.source_layer * cmd.source_total_mips + cmd.source_mip),
		};

		const D3D12_TEXTURE_COPY_LOCATION dst_location = {
			.pResource		  = dst.ptr->GetResource(),
			.Type			  = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
			.SubresourceIndex = static_cast<UINT>(cmd.destination_layer * cmd.destination_total_mips + cmd.destination_mip),
		};

		cmd_list->CopyTextureRegion(&dst_location, 0, 0, 0, &src_location, nullptr);
	}

	void dx12_backend::cmd_bind_constants(gfx_id cmd_id, const command_bind_constants& cmd) const
	{
		const command_buffer&		buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		cmd_list->SetGraphicsRoot32BitConstants(static_cast<uint32>(cmd.param_index), static_cast<uint32>(cmd.count), cmd.data, cmd.offset);
	}

	void dx12_backend::cmd_bind_layout(gfx_id cmd_id, const command_bind_layout& cmd) const
	{
		const command_buffer&		buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		const bind_layout&			layout	 = _bind_layouts.get(cmd.layout);
		cmd_list->SetGraphicsRootSignature(layout.root_signature.Get());
	}

	void dx12_backend::cmd_bind_layout_compute(gfx_id cmd_id, const command_bind_layout_compute& cmd) const
	{
		const command_buffer&		buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		const bind_layout&			layout	 = _bind_layouts.get(cmd.layout);
		cmd_list->SetComputeRootSignature(layout.root_signature.Get());
	}

	void dx12_backend::cmd_bind_group(gfx_id cmd_id, const command_bind_group& cmd) const
	{
		const command_buffer&		buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		const bind_group&			group	 = _bind_groups.get(cmd.group);
		const uint8					sz		 = static_cast<uint8>(group.bindings.size());

		for (uint8 i = 0; i < sz; i++)
		{
			const group_binding&	 binding = group.bindings[i];
			const descriptor_handle& dh		 = _descriptors.get(binding.descriptor_index);
			const binding_type		 type	 = static_cast<binding_type>(binding.binding_type);

			if (type == binding_type::sampler || type == binding_type::pointer)
				cmd_list->SetGraphicsRootDescriptorTable(binding.root_param_index, {dh.gpu});
			else if (type == binding_type::ubo)
				cmd_list->SetGraphicsRootConstantBufferView(binding.root_param_index, dh.gpu);
			else if (type == binding_type::ssbo)
				cmd_list->SetGraphicsRootShaderResourceView(binding.root_param_index, dh.gpu);
			else if (type == binding_type::uav)
				cmd_list->SetGraphicsRootUnorderedAccessView(binding.root_param_index, dh.gpu);
			else if (type == binding_type::constant)
				cmd_list->SetGraphicsRoot32BitConstants(binding.root_param_index, binding.count, binding.constants, 0);
		}
	}

	void dx12_backend::cmd_dispatch(gfx_id cmd_id, const command_dispatch& cmd) const
	{
		const command_buffer&		buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();
		cmd_list->Dispatch(cmd.group_size_x, cmd.group_size_y, cmd.group_size_z);
	}

	void dx12_backend::cmd_barrier(gfx_id cmd_id, const command_barrier& cmd)
	{
		command_buffer&				buffer	 = _command_buffers.get(cmd_id);
		ID3D12GraphicsCommandList4* cmd_list = buffer.ptr.Get();

		_reuse_barriers.resize(0);

		for (uint16 i = 0; i < cmd.barrier_count; i++)
		{
			const barrier& barrier = cmd.barriers[i];

			ID3D12Resource* res = nullptr;

			if (barrier.flags.is_set(barrier_flags::baf_is_resource))
				res = _resources.get(barrier.resource).ptr->GetResource();
			else if (barrier.flags.is_set(barrier_flags::baf_is_swapchain))
			{
				swapchain& swp = _swapchains.get(barrier.resource);
				res			   = swp.textures[swp.image_index].Get();
			}
			else
			{
				texture& txt = _textures.get(barrier.resource);
				res			 = txt.ptr->GetResource();
			}

			_reuse_barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(res, get_resource_state(barrier.from_state), get_resource_state(barrier.to_state)));
		}

		if (!_reuse_barriers.empty())
			cmd_list->ResourceBarrier(static_cast<UINT>(_reuse_barriers.size()), _reuse_barriers.data());
	}

}