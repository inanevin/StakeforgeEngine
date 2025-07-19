// Copyright (c) 2025 Inan Evin

#include "dx12_backend.hpp"
#include "gfx/common/descriptions.hpp"
#include "gfx/backend/dx12/sdk/D3D12MemAlloc.h"
#include "gfx/commands.hpp"
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

namespace Game
{

	Microsoft::WRL::ComPtr<IDxcLibrary> dx12_backend::s_idxcLib;

#define DX12_THROW(exception, ...)                                                                                                                                                                                                                                 \
	GAME_ERR(__VA_ARGS__);                                                                                                                                                                                                                                         \
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
			case topology::triangle_list_adjacency:
			case topology::triangle_strip_adjacency:
				return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			default:
				return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			}
		}

	}

	void dx12_backend::init()
	{
		_queue_graphics = create_queue({
			.type		= command_type::graphics,
			.debug_name = {"GfxQueue"},
		});
		_queue_transfer = create_queue({
			.type		= command_type::graphics,
			.debug_name = {"TransferQueue"},
		});
		_queue_compute	= create_queue({
			 .type		 = command_type::graphics,
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
	}

	void dx12_backend::uninit()
	{
		destroy_queue(_queue_graphics);
		destroy_queue(_queue_transfer);
		destroy_queue(_queue_compute);

		_heap_buffer.uninit();
		_heap_texture.uninit();
		_heap_dsv.uninit();
		_heap_rtv.uninit();
		_heap_sampler.uninit();
		_heap_gpu_buffer.uninit();
		_heap_gpu_sampler.uninit();
	}

	resource_id dx12_backend::create_resource(const resource_desc& desc)
	{
		const resource_id id  = _resources.add();
		resource&		  res = _resources.get(id);

		const uint32 aligned_size = ALIGN_SIZE_POW(desc.size, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
		const uint32 final_size	  = desc.flags.is_set(resource_flags::rf_constant_buffer) ? aligned_size : desc.size;

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

		throw_if_failed(_dx12Allocator->CreateResource(&allocation_desc, &resource_desc, state, NULL, &res.ptr, IID_NULL, NULL));

		if (desc.flags.is_set(resource_flags::rf_constant_buffer))
		{
			const D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {
				.BufferLocation = res.ptr->GetResource()->GetGPUVirtualAddress(),
				.SizeInBytes	= static_cast<UINT>(final_size),
			};
			res.descriptor = _heap_buffer.get_heap_handle_block(1);
			_device->CreateConstantBufferView(&desc, {res.descriptor.cpu});
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
			res.descriptor = _heap_buffer.get_heap_handle_block(1);
			_device->CreateShaderResourceView(res.ptr->GetResource(), &desc, {res.descriptor.cpu});
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
			res.descriptor = _heap_buffer.get_heap_handle_block(1);
			_device->CreateUnorderedAccessView(res.ptr->GetResource(), NULL, &desc, {res.descriptor.cpu});
		}

		NAME_DX12_OBJECT_CSTR(res.ptr, desc.debug_name);

		return id;
	}

	void dx12_backend::map_resource(resource_id id, uint8* ptr)
	{
		resource&	  res = _resources.get(id);
		CD3DX12_RANGE range(0, 0);
		throw_if_failed(res.ptr->GetResource()->Map(0, &range, reinterpret_cast<void**>(&ptr)));
	}

	void dx12_backend::unmap_resource(resource_id id)
	{
		resource&	  res = _resources.get(id);
		CD3DX12_RANGE range(0, 0);
		res.ptr->GetResource()->Unmap(0, &range);
	}

	void dx12_backend::destroy_resource(resource_id id)
	{
		unmap_resource(id);
		resource& res = _resources.get(id);
		if (res.descriptor.count != 0)
			_heap_buffer.remove_handle(res.descriptor);
		res.ptr->Release();
		_resources.remove(id);
	}

	resource_id dx12_backend::create_texture(const texture_desc& desc)
	{
		const resource_id id  = _textures.add();
		texture&		  txt = _textures.get(id);

		const DXGI_FORMAT color_format = get_format(desc.texture_format);
		const DXGI_FORMAT depth_format = get_format(desc.depth_stencil_format);

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

		if (desc.flags.is_set(texture_flags::tf_depth_texture) || desc.flags.is_set(texture_flags::tf_stencil_texture))
			resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		if (desc.flags.is_set(texture_flags::tf_color_attachment))
			resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		if (desc.samples == 1 && !desc.flags.is_set(texture_flags::tf_sampled) && !desc.flags.is_set(texture_flags::tf_sampled_outside_fragment))
			resource_desc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

		const D3D12MA::ALLOCATION_DESC allocation_desc = {
			.HeapType = D3D12_HEAP_TYPE_DEFAULT,
		};

		const D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

		const D3D12_RESOURCE_ALLOCATION_INFO& allocation_info = _device->GetResourceAllocationInfo(0, 1, &resource_desc);

		throw_if_failed(_dx12Allocator->CreateResource(&allocation_desc, &resource_desc, state, NULL, &txt.ptr, IID_NULL, NULL));
		NAME_DX12_OBJECT_CSTR(txt.ptr, desc.debug_name);

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
				GAME_ASSERT(false, "Can't create a depth Texture 3D!");
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

			if (desc.flags.is_set(texture_flags::tf_sampled || desc.flags.is_set(texture_flags::tf_sampled_outside_fragment)))
			{
				txt.srvs.push_back(_heap_texture.get_heap_handle_block(1));
				const descriptor_handle& srv = txt.srvs.back();
				create_srv(srv_format, view.is_cubemap, base_level, remaining_level, base_mip, remaining_mip, srv);
			}
			if (desc.flags.is_set(texture_flags::tf_depth_texture || desc.flags.is_set(texture_flags::tf_stencil_texture)))
			{
				txt.dsvs.push_back(_heap_dsv.get_heap_handle_block(1));
				const descriptor_handle& dsv = txt.dsvs.back();
				create_dsv(depth_format, base_level, remaining_level, base_mip, remaining_mip, dsv);
			}

			if (desc.flags.is_set(texture_flags::tf_color_attachment))
			{
				txt.rtvs.push_back(_heap_rtv.get_heap_handle_block(1));
				const descriptor_handle& rtv = txt.rtvs.back();
				create_rtv(color_format, base_level, remaining_level, base_mip, remaining_mip, rtv);
			}
		}

		return id;
	}

	void dx12_backend::destroy_texture(resource_id id)
	{
		texture& txt = _textures.get(id);

		for (const descriptor_handle& d : txt.rtvs)
			_heap_rtv.remove_handle(d);

		for (const descriptor_handle& d : txt.dsvs)
			_heap_dsv.remove_handle(d);

		for (const descriptor_handle& d : txt.srvs)
			_heap_texture.remove_handle(d);

		txt.ptr->Release();
		txt.ptr = NULL;
		_textures.remove(id);
	}

	resource_id dx12_backend::create_sampler(const sampler_desc& desc)
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

		const resource_id id  = _samplers.add();
		sampler&		  smp = _samplers.get(id);
		smp.handle			  = _heap_sampler.get_heap_handle_block(1);
		_device->CreateSampler(&samplerDesc, {smp.handle.cpu});
		return id;
	}

	void dx12_backend::destroy_sampler(resource_id id)
	{
		sampler& smp = _samplers.get(id);
		_heap_sampler.remove_handle(smp.handle);
		_samplers.remove(id);
	}

	resource_id dx12_backend::create_swapchain(const swapchain_desc& desc)
	{
		const resource_id id  = _swapchains.add();
		swapchain&		  swp = _swapchains.get(id);

		DXGI_FORMAT swap_format = DXGI_FORMAT_B8G8R8A8_UNORM;
		if (desc.format == format::r16g16b16a16_sfloat)
			swap_format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		else if (desc.format == format::r8g8b8a8_unorm)
			swap_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		else if (desc.format == format::r8g8b8a8_srgb)
			swap_format = DXGI_FORMAT_R8G8B8A8_UNORM;

		const DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {
			.Width	= static_cast<UINT>(desc.size.x),
			.Height = static_cast<UINT>(desc.size.y),
			.Format = swap_format,
			.SampleDesc =
				{
					.Count = 1,
				},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = gfx_util::BACK_BUFFER_COUNT,
			.SwapEffect	 = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.Flags		 = desc.flags.is_set(swapchain_flags::sf_allow_tearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : (UINT)0,
		};

		ComPtr<IDXGISwapChain1> swapchain;

		throw_if_failed(_factory->CreateSwapChainForHwnd(_queues.get(_queue_graphics).ptr.Get(), (HWND)desc.window, &swapchain_desc, nullptr, nullptr, &swapchain));
		throw_if_failed(swapchain.As(&swp.ptr));

		{
			for (uint32 i = 0; i < gfx_util::BACK_BUFFER_COUNT; i++)
			{
				throw_if_failed(swp.ptr->GetBuffer(i, IID_PPV_ARGS(&swp.textures[i])));
				swp.rtvs[i] = _heap_rtv.get_heap_handle_block(1);

				const D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {
					.Format		   = swap_format,
					.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
				};

				_device->CreateRenderTargetView(swp.textures[i].Get(), &rtv_desc, {swp.rtvs[i].cpu});
				NAME_DX12_OBJECT_CSTR(swp.textures[i], "Swapchain RTV");
			}
		}

		return id;
	}

	resource_id dx12_backend::recreate_swapchain(const swapchain_recreate_desc& desc)
	{
		swapchain& swp = _swapchains.get(desc.swapchain);

		DXGI_FORMAT swap_format = DXGI_FORMAT_B8G8R8A8_UNORM;
		if (desc.format == format::r16g16b16a16_sfloat)
			swap_format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		else if (desc.format == format::r8g8b8a8_unorm)
			swap_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		else if (desc.format == format::r8g8b8a8_srgb)
			swap_format = DXGI_FORMAT_R8G8B8A8_UNORM;

		DXGI_SWAP_CHAIN_DESC swp_desc = {};
		swp.ptr->GetDesc(&swp_desc);

		for (uint32 i = 0; i < gfx_util::BACK_BUFFER_COUNT; i++)
		{
			swp.textures[i].Reset();

			throw_if_failed(swp.ptr->GetBuffer(i, IID_PPV_ARGS(&swp.textures[i])));

			const D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {
				.Format		   = swap_format,
				.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
			};
			_device->CreateRenderTargetView(swp.textures[i].Get(), &rtv_desc, {swp.rtvs[i].cpu});
			NAME_DX12_OBJECT_CSTR(swp.textures[i], "Swapchain RTV");
		}

		return resource_id();
	}

	void dx12_backend::destroy_swapchain(resource_id id)
	{
		swapchain& swp = _swapchains.get(id);

		for (uint32 i = 0; i < gfx_util::BACK_BUFFER_COUNT; i++)
		{
			_heap_rtv.remove_handle(swp.rtvs[i]);
			swp.textures[i].Reset();
		}

		swp.ptr.Reset();
		_swapchains.remove(id);
	}

	resource_id dx12_backend::create_semaphore()
	{
		const resource_id id  = _semaphores.add();
		semaphore&		  sem = _semaphores.get(id);
		throw_if_failed(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&sem.ptr)));
		return id;
	}

	void dx12_backend::wait_semaphore(resource_id id, uint64 value)
	{
		semaphore& sem = _semaphores.get(id);
		wait_for_fence(sem.ptr.Get(), value);
	}

	void dx12_backend::destroy_semaphore(resource_id id)
	{
		semaphore& sem = _semaphores.get(id);
		sem.ptr.Reset();
		_semaphores.remove(id);
	}

	bool dx12_backend::compile_shader(shader_stage stage, const string& source, span<uint8>& out_data, bool compile_root_sig, span<uint8>& out_signature_data)
	{
		Microsoft::WRL::ComPtr<IDxcCompiler3> idxc_compiler;
		throw_if_failed(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&idxc_compiler)));

		ComPtr<IDxcUtils> utils;
		throw_if_failed(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf())));

		UINT32					 code_page = CP_UTF8;
		ComPtr<IDxcBlobEncoding> source_blob;
		const char*				 shader_source = source.c_str();
		s_idxcLib->CreateBlobWithEncodingFromPinned((const BYTE*)shader_source, static_cast<UINT>(source.size()), code_page, &source_blob);

		const wchar_t* target_profile = L"vs_6_0";
		if (stage == shader_stage::fragment)
			target_profile = L"ps_6_0";
		else if (stage == shader_stage::compute)
			target_profile = L"cs_6_0";

		DxcBuffer source_buffer;
		source_buffer.Ptr	   = source_blob->GetBufferPointer();
		source_buffer.Size	   = source_blob->GetBufferSize();
		source_buffer.Encoding = 0;

		vector<LPCWSTR> arguments = {L"-T", target_profile, DXC_ARG_WARNINGS_ARE_ERRORS, L"-HV 2021"};

#ifdef GAME_DEBUG
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
			GAME_ERR("DX12 -> %s", (char*)errors->GetStringPointer());
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
					GAME_ERR("DX12 -> Shader compilation failed: %s", (const char*)errorsBlob->GetBufferPointer());
					return false;
				}
			}
		}

		ComPtr<IDxcBlob> code;
		result->GetResult(&code);

		if (compile_root_sig)
		{
			ComPtr<IDxcBlob> root_sig_blob;
			result->GetOutput(DXC_OUT_ROOT_SIGNATURE, IID_PPV_ARGS(&root_sig_blob), nullptr);

			if (root_sig_blob && root_sig_blob->GetBufferSize() > 0)
			{
				out_signature_data.size = root_sig_blob->GetBufferSize();
				out_signature_data.data = new uint8[out_signature_data.size];
				GAME_MEMCPY(out_signature_data.data, root_sig_blob->GetBufferPointer(), out_signature_data.size);
			}
		}

		if (code.Get() != NULL)
		{
			const SIZE_T sz = code->GetBufferSize();
			out_data.size	= code->GetBufferSize();
			out_data.data	= new uint8[sz];
			GAME_MEMCPY(out_data.data, code->GetBufferPointer(), out_data.size);
		}
		else
		{
			GAME_ERR("DX12 -> Failed compiling IDXC blob!");
			return false;
		}

		return true;
	}

	resource_id dx12_backend::create_shader(const shader_desc& desc)
	{
		const resource_id id = _shaders.add();
		shader&			  sh = _shaders.get(id);

		throw_if_failed(_device->CreateRootSignature(0, desc.signature_data.data, desc.signature_data.size, IID_PPV_ARGS(&sh.root_signature)));

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
			const vertex_input& inp	  = desc.inputs[i];
			const uint32		index = i;
			input_layout.push_back({
				.SemanticName		  = inp.name.c_str(),
				.SemanticIndex		  = index,
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
				GAME_ASSERT(false);
			}
		}

		throw_if_failed(_device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&sh.ptr)));
		NAME_DX12_OBJECT_CSTR(sh.ptr, desc.debug_name);

		return id;
	}

	void dx12_backend::destroy_shader(resource_id id)
	{
		shader& sh = _shaders.get(id);
		sh.root_signature.Reset();
		sh.ptr.Reset();
		_shaders.remove(id);
	}

	resource_id dx12_backend::create_bind_group(const bind_group_desc& desc)
	{
		const resource_id id	= _bind_groups.add();
		bind_group&		  group = _bind_groups.get(id);

		uint32 i = 0;
		for (const binding& binding : desc.bindings)
		{
			group.bindings[i] = {
				.handle			  = binding.flags.is_set(binding_flags::bf_sampler) ? _heap_gpu_sampler.get_heap_handle_block(binding.count)
																					: (!binding.flags.is_set(binding_flags::bf_constant) ? _heap_gpu_buffer.get_heap_handle_block(binding.count) : descriptor_handle(binding.constant_value, binding.constant_offset)),
				.root_param_index = binding.root_index,
				.flags			  = binding.flags,
			};
			i++;
		}

		group.bindings_count = i;
		return id;
	}

	void dx12_backend::update_bind_group(resource_id id, const bind_group_update_desc& update)
	{
		const bind_group& group = _bind_groups.get(id);

		_reuse_dest_descriptors_buffer.resize(0);
		_reuse_dest_descriptors_sampler.resize(0);
		_reuse_src_descriptors_buffer.resize(0);
		_reuse_src_descriptors_sampler.resize(0);

		for (const binding_update& up : update.updates)
		{
			const group_binding& binding = group.bindings[up.binding_index];

			if (binding.flags.is_set(binding_flags::bf_constant))
				continue;

			for (size_t i = 0; i < up.resources.size(); i++)
			{
				const resource_id	  res_id = up.resources[i];
				const bitmask<uint8>& flags	 = up.resource_flags[i];

				if (flags.is_set(binding_flags::bf_ubo) || flags.is_set(binding_flags::bf_ssbo) || flags.is_set(binding_flags::bf_uav))
				{
					resource& res = _resources.get(res_id);
					_reuse_src_descriptors_buffer.push_back({res.descriptor.cpu});
					_reuse_dest_descriptors_buffer.push_back({binding.handle.cpu + i * _heap_gpu_buffer.get_descriptor_size()});
				}
				else if (flags.is_set(binding_flags::bf_texture))
				{
					texture& txt = _textures.get(res_id);
					_reuse_src_descriptors_buffer.push_back({txt.srvs[i].cpu});
					_reuse_dest_descriptors_buffer.push_back({binding.handle.cpu + i * _heap_gpu_buffer.get_descriptor_size()});
				}
				else if (flags.is_set(binding_flags::bf_sampler))
				{
					sampler& smp = _samplers.get(res_id);
					_reuse_src_descriptors_sampler.push_back({smp.handle.cpu});
					_reuse_dest_descriptors_sampler.push_back({binding.handle.cpu + i * _heap_gpu_sampler.get_descriptor_size()});
				}
			}
		}

		const uint32 descriptor_count_buffer  = static_cast<uint32>(_reuse_dest_descriptors_buffer.size());
		const uint32 descriptor_count_sampler = static_cast<uint32>(_reuse_dest_descriptors_sampler.size());

		if (descriptor_count_buffer != 0)
			_device->CopyDescriptors(descriptor_count_buffer, _reuse_dest_descriptors_buffer.data(), NULL, descriptor_count_buffer, _reuse_src_descriptors_buffer.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		if (descriptor_count_sampler != 0)
			_device->CopyDescriptors(descriptor_count_sampler, _reuse_dest_descriptors_sampler.data(), NULL, descriptor_count_sampler, _reuse_src_descriptors_sampler.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	}

	void dx12_backend::destroy_bind_group(resource_id id)
	{
		bind_group& group = _bind_groups.get(id);

		for (const group_binding& binding : group.bindings)
		{
			if (binding.flags.is_set(binding_flags::bf_sampler))
				_heap_gpu_sampler.remove_handle(binding.handle);
			else if (!binding.flags.is_set(binding_flags::bf_constant))
				_heap_gpu_buffer.remove_handle(binding.handle);
		}

		_bind_groups.remove(id);
	}

	resource_id dx12_backend::create_command_buffer(const command_buffer_desc& desc)
	{
		const resource_id  id	 = _command_buffers.add();
		command_allocator& alloc = _command_allocators.get(id);
		command_buffer&	   cmd	 = _command_buffers.get(id);
		throw_if_failed(_device->CreateCommandList(0, get_command_type(desc.type), alloc.ptr.Get(), nullptr, IID_PPV_ARGS(cmd.ptr.GetAddressOf())));
		NAME_DX12_OBJECT_CSTR(cmd.ptr, desc.debug_name);
		return id;
	}

	void dx12_backend::destroy_command_buffer(resource_id id)
	{
		command_buffer& cmd = _command_buffers.get(id);
		cmd.ptr.Reset();
		_command_buffers.remove(id);
	}

	resource_id dx12_backend::create_command_allocator(command_type type)
	{
		const resource_id  id	 = _command_allocators.add();
		command_allocator& alloc = _command_allocators.get(id);
		throw_if_failed(_device->CreateCommandAllocator(get_command_type(type), IID_PPV_ARGS(alloc.ptr.GetAddressOf())));
		return id;
	}

	void dx12_backend::destroy_command_allocator(resource_id id)
	{
		command_allocator& alloc = _command_allocators.get(id);
		alloc.ptr.Reset();
		_command_allocators.remove(id);
	}

	resource_id dx12_backend::create_queue(const queue_desc& desc)
	{
		const resource_id id = _queues.add();
		queue&			  q	 = _queues.get(id);

		const D3D12_COMMAND_QUEUE_DESC queue_desc = {
			.Type  = get_command_type(desc.type),
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
		};
		throw_if_failed(_device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&q.ptr)));
		return id;
	}

	void dx12_backend::destroy_queue(resource_id id)
	{
		queue& q = _queues.get(id);
		q.ptr.Reset();
		_queues.remove(id);
	}

	void dx12_backend::wait_for_fence(ID3D12Fence* fence, uint64 value)
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
				DX12_THROW(e, "Error while waiting for fence! {0}", e.what());
			}
		}
	}

	void dx12_backend::cmd_bind_group(ID3D12GraphicsCommandList4* cmd_list, command_bind_group& cmd)
	{
		bind_group& group = _bind_groups.get(cmd.group);

		for (uint8 i = 0; i < group.bindings_count; i++)
		{
			group_binding& binding = group.bindings[i];
			if (binding.flags.is_set(binding_flags::bf_table))
				cmd_list->SetGraphicsRootDescriptorTable(binding.root_param_index, {binding.handle.gpu});
			else if (binding.flags.is_set(binding_flags::bf_ubo))
				cmd_list->SetGraphicsRootConstantBufferView(binding.root_param_index, binding.handle.gpu);
			else if (binding.flags.is_set(binding_flags::bf_ssbo) || binding.flags.is_set(binding_flags::bf_texture))
				cmd_list->SetGraphicsRootShaderResourceView(binding.root_param_index, binding.handle.gpu);
			else if (binding.flags.is_set(binding_flags::bf_uav))
				cmd_list->SetGraphicsRootUnorderedAccessView(binding.root_param_index, binding.handle.gpu);
			else if (binding.flags.is_set(binding_flags::bf_constant))
				cmd_list->SetGraphicsRoot32BitConstant(binding.root_param_index, static_cast<uint32>(binding.handle.cpu), static_cast<uint32>(binding.handle.gpu));
		}
	}
}