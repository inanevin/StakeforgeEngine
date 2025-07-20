// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "gfx/common/resource_limits.hpp"
#include "data/vector.hpp"
#include "data/span.hpp"
#include "data/bitmask.hpp"
#include "io/assert.hpp"
#include "dx12_heap.hpp"
#include "gfx/gfx_util.hpp"
#include <functional>

namespace D3D12MA
{
	class Allocator;
	class Allocation;
} // namespace D3D12MA

namespace Game
{

	struct resource_desc;
	struct texture_desc;
	struct sampler_desc;
	struct swapchain_desc;
	struct swapchain_recreate_desc;
	struct shader_desc;
	struct bind_group_desc;
	struct pipeline_layout_desc;
	struct command_buffer_desc;
	struct queue_desc;
	struct bind_group_update_desc;
	struct bind_layout_desc;
	struct present_desc;

	struct command_bind_group;

	typedef std::function<void(ID3D12GraphicsCommandList4* cmd_list, uint8* data)> command_function;

	class dx12_backend
	{
	public:
		struct resource
		{
			D3D12MA::Allocation* ptr			  = nullptr;
			int16				 descriptor_index = -1;
			uint32				 size			  = 0;
		};

		struct texture
		{
			D3D12MA::Allocation* ptr = nullptr;
			resource_id			 srvs[6];
			resource_id			 dsvs[6];
			resource_id			 rtvs[6];
			uint8				 rtv_count = 0;
			uint8				 srv_count = 0;
			uint8				 dsv_count = 0;
			uint8				 format	   = 0;
		};

		struct sampler
		{
			resource_id descriptor_index = 0;
		};

		struct swapchain
		{
			Microsoft::WRL::ComPtr<IDXGISwapChain3> ptr = NULL;
			Microsoft::WRL::ComPtr<ID3D12Resource>	textures[gfx_util::BACK_BUFFER_COUNT];
			resource_id								rtv_indices[gfx_util::BACK_BUFFER_COUNT];
			uint8									format		= 0;
			uint8									image_index = 0;
			uint8									vsync		= 0;
		};

		struct semaphore
		{
			Microsoft::WRL::ComPtr<ID3D12Fence> ptr = nullptr;
		};

		struct shader
		{
			Microsoft::WRL::ComPtr<ID3D12PipelineState> ptr				   = nullptr;
			Microsoft::WRL::ComPtr<ID3D12RootSignature> root_signature	   = nullptr;
			uint8										indirect_signature = 0;
			uint8										topology		   = 0;
		};

		struct indirect_signature
		{
			Microsoft::WRL::ComPtr<ID3D12CommandSignature> signature = nullptr;
		};

		struct group_binding
		{
			resource_id descriptor_index = 0;
			uint32		root_param_index = 0;
			uint8		descriptor_type	 = 0;
		};

		struct bind_group
		{
			group_binding bindings[8];
			uint8		  bindings_count = 0;
		};

		struct command_buffer
		{
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> ptr;
			resource_id										   allocator   = 0;
			uint8											   is_transfer = 0;
		};

		struct command_allocator
		{
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> ptr;
		};

		struct queue
		{
			Microsoft::WRL::ComPtr<ID3D12CommandQueue> ptr;
		};

		struct bind_layout
		{
			Microsoft::WRL::ComPtr<ID3D12RootSignature> root_signature = nullptr;
		};

		template <typename T, int N> struct pool
		{
			T					resources[N];
			vector<resource_id> free_list = {};
			resource_id			head	  = 0;

			inline resource_id add()
			{
				if (!free_list.empty())
				{
					const resource_id id = free_list.back();
					free_list.pop_back();
					return id;
				}

				const resource_id id = head;
				head++;
				GAME_ASSERT(head < N);
				return id;
			}

			void remove(resource_id id)
			{
				free_list.push_back(id);
				resources[id] = T();
			}

			T& get(resource_id id)
			{
				GAME_ASSERT(id < N);
				return resources[id];
			}

			const T& get(resource_id id) const
			{
				GAME_ASSERT(id < N);
				return resources[id];
			}
		};

	public:
		void init();
		void uninit();
		void reset_command_buffer(resource_id cmd_buffer);
		void close_command_buffer(resource_id cmd_buffer);
		void submit_commands(resource_id queue, resource_id* commands, uint8 commands_count);
		void queue_wait(resource_id queue, resource_id* semaphores, uint8 semaphore_count, uint64* semaphore_values);
		void queue_signal(resource_id queue, resource_id* semaphores, uint8 semaphore_count, uint64* semaphore_values);
		void present(const present_desc& desc);

		bool compile_shader(uint8 stage, const string& source, span<uint8>& out_data, bool compile_root_sig, span<uint8>& out_signature_data);

		resource_id create_resource(const resource_desc& desc);
		resource_id create_texture(const texture_desc& desc);
		resource_id create_sampler(const sampler_desc& desc);
		resource_id create_swapchain(const swapchain_desc&);
		resource_id recreate_swapchain(const swapchain_recreate_desc& desc);
		resource_id create_semaphore();
		resource_id create_shader(const shader_desc& desc);
		resource_id create_bind_group(const bind_group_desc& desc);
		resource_id create_command_buffer(const command_buffer_desc& desc);
		resource_id create_command_allocator(uint8 ctype);
		resource_id create_queue(const queue_desc& desc);
		resource_id create_bind_layout(const bind_layout_desc& desc);

		void destroy_resource(resource_id id);
		void destroy_texture(resource_id id);
		void destroy_sampler(resource_id id);
		void destroy_swapchain(resource_id id);
		void destroy_semaphore(resource_id id);
		void destroy_shader(resource_id id);
		void destroy_bind_group(resource_id id);
		void destroy_command_buffer(resource_id id);
		void destroy_command_allocator(resource_id id);
		void destroy_queue(resource_id id);
		void destroy_bind_layout(resource_id id);

		void wait_semaphore(resource_id id, uint64 value);
		void map_resource(resource_id id, uint8* ptr);
		void unmap_resource(resource_id id);
		void update_bind_group(resource_id, const bind_group_update_desc& update);

		uint32 get_aligned_texture_size(uint32 width, uint32 height, uint32 bpp);
		void*  adjust_buffer_pitch(void* data, uint32 width, uint32 height, uint32 bpp);

		inline resource_id get_queue_gfx() const
		{
			return _queue_graphics;
		}

		inline resource_id get_queue_transfer() const
		{
			return _queue_transfer;
		}

		inline resource_id get_queue_compute() const
		{
			return _queue_compute;
		}

	private:
		void wait_for_fence(ID3D12Fence* fence, uint64 value);

		void cmd_begin_render_pass(resource_id cmd_list, const command_begin_render_pass& command);
		void cmd_begin_render_pass_depth(resource_id cmd_list, const command_begin_render_pass_depth& command);
		void cmd_begin_render_pass_swapchain(resource_id cmd_list, const command_begin_render_pass_swapchain& command);
		void cmd_begin_render_pass_swapchain_depth(resource_id cmd_list, const command_begin_render_pass_swapchain_depth& command);
		void cmd_end_render_pass(resource_id cmd_list, const command_end_render_pass& command);
		void cmd_set_scissors(resource_id cmd_list, const command_set_scissors& command);
		void cmd_set_viewport(resource_id cmd_list, const command_set_viewport& command);
		void cmd_bind_pipeline(resource_id cmd_list, const command_bind_pipeline& command);
		void cmd_bind_pipeline_compute(resource_id cmd_list, const command_bind_pipeline_compute& command);
		void cmd_draw_instanced(resource_id cmd_list, const command_draw_instanced& command);
		void cmd_draw_instanced(resource_id cmd_list, const command_draw_instanced& command);
		void cmd_draw_indexed_instanced(resource_id cmd_list, const command_draw_indexed_instanced& command);
		void cmd_draw_indexed_indirect(resource_id cmd_list, const command_draw_indexed_indirect& command);
		void cmd_draw_indirect(resource_id cmd_list, const command_draw_indirect& command);
		void cmd_bind_vertex_buffers(resource_id cmd_list, const command_bind_vertex_buffers& command);
		void cmd_bind_index_buffers(resource_id cmd_list, const command_bind_index_buffers& command);
		void cmd_copy_resource(resource_id cmd_list, const command_copy_resource& command);
		void cmd_copy_buffer_to_texture(resource_id cmd_list, const command_copy_buffer_to_texture& command);
		void cmd_copy_texture_to_buffer(resource_id cmd_list, const command_copy_texture_to_buffer& command);
		void cmd_copy_texture_to_texture(resource_id cmd_list, const command_copy_texture_to_texture& command);
		void cmd_bind_constants(resource_id cmd_list, const command_bind_constants& command);
		void cmd_bind_layout(resource_id cmd_list, const command_bind_layout& command);
		void cmd_bind_layout_compute(resource_id cmd_list, const command_bind_layout_compute& command);
		void cmd_bind_group(resource_id cmd_list, const command_bind_group& command);
		void cmd_dispatch(resource_id cmd_list, const command_dispatch& command);
		void cmd_barrier(resource_id cmd_list, const command_barrier& command);

	private:
		pool<resource, MAX_RESOURCES>					_resources;
		pool<texture, MAX_TEXTURES>						_textures;
		pool<sampler, MAX_SAMPLERS>						_samplers;
		pool<swapchain, MAX_SWAPCHAINS>					_swapchains;
		pool<semaphore, MAX_SEMAPHORES>					_semaphores;
		pool<shader, MAX_SHADERS>						_shaders;
		pool<bind_group, MAX_BIND_GROUPS>				_bind_groups;
		pool<command_buffer, MAX_COMMAND_BUFFERS>		_command_buffers;
		pool<command_allocator, MAX_COMMAND_BUFFERS>	_command_allocators;
		pool<queue, MAX_QUEUES>							_queues;
		pool<indirect_signature, 255>					_indirect_signatures;
		pool<descriptor_handle, MAX_DESCRIPTOR_HANDLES> _descriptors;
		pool<bind_layout, MAX_BIND_LAYOUTs>				_bind_layouts;

		dx12_heap _heap_rtv			= {};
		dx12_heap _heap_buffer		= {};
		dx12_heap _heap_texture		= {};
		dx12_heap _heap_dsv			= {};
		dx12_heap _heap_sampler		= {};
		dx12_heap _heap_gpu_buffer	= {};
		dx12_heap _heap_gpu_sampler = {};

		D3D12MA::Allocator*						   _allocator = nullptr;
		Microsoft::WRL::ComPtr<IDXGIAdapter1>	   _adapter	  = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Device>	   _device	  = nullptr;
		Microsoft::WRL::ComPtr<IDXGIFactory4>	   _factory	  = nullptr;
		static Microsoft::WRL::ComPtr<IDxcLibrary> s_idxcLib;

		vector<D3D12_CPU_DESCRIPTOR_HANDLE>			 _reuse_dest_descriptors_buffer	 = {};
		vector<D3D12_CPU_DESCRIPTOR_HANDLE>			 _reuse_dest_descriptors_sampler = {};
		vector<D3D12_CPU_DESCRIPTOR_HANDLE>			 _reuse_src_descriptors_buffer	 = {};
		vector<D3D12_CPU_DESCRIPTOR_HANDLE>			 _reuse_src_descriptors_sampler	 = {};
		vector<CD3DX12_RESOURCE_BARRIER>			 _reuse_barriers;
		vector<D3D12_RENDER_PASS_RENDER_TARGET_DESC> _reuse_color_attachments;
		vector<CD3DX12_ROOT_PARAMETER1>				 _reuse_root_params		 = {};
		vector<CD3DX12_DESCRIPTOR_RANGE1>			 _reuse_root_ranges		 = {};
		vector<D3D12_SUBRESOURCE_DATA>				 _reuse_subresource_data = {};
		vector<ID3D12CommandList*>					 _reuse_lists			 = {};
		vector<ID3D12Fence*>						 _reuse_fences			 = {};
		vector<uint64>								 _reuse_values			 = {};

		resource_id _queue_graphics = 0;
		resource_id _queue_transfer = 0;
		resource_id _queue_compute	= 0;
	};
}