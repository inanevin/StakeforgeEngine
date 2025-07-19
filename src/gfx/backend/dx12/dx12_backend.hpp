// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "gfx/common/resource_limits.hpp"
#include "gfx/common/gfx_common.hpp"
#include "data/vector.hpp"
#include "data/span.hpp"
#include "data/bitmask.hpp"
#include "io/assert.hpp"
#include "dx12_heap.hpp"
#include "gfx/gfx_util.hpp"

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

	struct command_bind_group;

	enum class command_type;

	class dx12_backend
	{
	public:
		struct resource
		{
			D3D12MA::Allocation* ptr		= nullptr;
			descriptor_handle	 descriptor = {};
		};

		struct texture
		{
			D3D12MA::Allocation*	  ptr = nullptr;
			vector<descriptor_handle> srvs;
			vector<descriptor_handle> dsvs;
			vector<descriptor_handle> rtvs;
		};

		struct sampler
		{
			descriptor_handle handle = {};
		};

		struct swapchain
		{
			Microsoft::WRL::ComPtr<IDXGISwapChain3> ptr = NULL;
			Microsoft::WRL::ComPtr<ID3D12Resource>	textures[gfx_util::BACK_BUFFER_COUNT];
			descriptor_handle						rtvs[gfx_util::BACK_BUFFER_COUNT];
		};

		struct semaphore
		{
			Microsoft::WRL::ComPtr<ID3D12Fence> ptr = nullptr;
		};

		struct shader
		{
			Microsoft::WRL::ComPtr<ID3D12PipelineState> ptr			   = nullptr;
			Microsoft::WRL::ComPtr<ID3D12RootSignature> root_signature = nullptr;
		};

		struct group_binding
		{
			descriptor_handle handle		   = {};
			uint32			  root_param_index = 0;
			bitmask<uint8>	  flags			   = 0;
		};

		struct bind_group
		{
			group_binding bindings[8];
			uint8		  bindings_count = 0;
		};

		struct command_buffer
		{
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> ptr;
		};

		struct command_allocator
		{
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> ptr;
		};

		struct queue
		{
			Microsoft::WRL::ComPtr<ID3D12CommandQueue> ptr;
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
		};

	public:
		void init();
		void uninit();

		bool compile_shader(shader_stage stage, const string& source, span<uint8>& out_data, bool compile_root_sig, span<uint8>& out_signature_data);

		resource_id create_resource(const resource_desc& desc);
		resource_id create_texture(const texture_desc& desc);
		resource_id create_sampler(const sampler_desc& desc);
		resource_id create_swapchain(const swapchain_desc&);
		resource_id recreate_swapchain(const swapchain_recreate_desc& desc);
		resource_id create_semaphore();
		resource_id create_shader(const shader_desc& desc);
		resource_id create_bind_group(const bind_group_desc& desc);
		resource_id create_command_buffer(const command_buffer_desc& desc);
		resource_id create_command_allocator(command_type type);
		resource_id create_queue(const queue_desc& desc);

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

		void wait_semaphore(resource_id id, uint64 value);
		void map_resource(resource_id id, uint8* ptr);
		void unmap_resource(resource_id id);
		void update_bind_group(resource_id, const bind_group_update_desc& update);

	private:
		void wait_for_fence(ID3D12Fence* fence, uint64 value);

		void cmd_bind_group(ID3D12GraphicsCommandList4* cmd_list, command_bind_group& cmd);

	private:
		pool<resource, MAX_RESOURCES>				 _resources;
		pool<texture, MAX_TEXTURES>					 _textures;
		pool<sampler, MAX_SAMPLERS>					 _samplers;
		pool<swapchain, MAX_SWAPCHAINS>				 _swapchains;
		pool<semaphore, MAX_SEMAPHORES>				 _semaphores;
		pool<shader, MAX_SHADERS>					 _shaders;
		pool<bind_group, MAX_DESCRIPTOR_SETS>		 _bind_groups;
		pool<command_buffer, MAX_COMMAND_BUFFERS>	 _command_buffers;
		pool<command_allocator, MAX_COMMAND_BUFFERS> _command_allocators;
		pool<queue, MAX_QUEUES>						 _queues;

		dx12_heap _heap_rtv			= {};
		dx12_heap _heap_buffer		= {};
		dx12_heap _heap_texture		= {};
		dx12_heap _heap_dsv			= {};
		dx12_heap _heap_sampler		= {};
		dx12_heap _heap_gpu_buffer	= {};
		dx12_heap _heap_gpu_sampler = {};

		D3D12MA::Allocator*						   _dx12Allocator = nullptr;
		Microsoft::WRL::ComPtr<IDXGIAdapter1>	   _adapter		  = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Device>	   _device		  = nullptr;
		Microsoft::WRL::ComPtr<IDXGIFactory4>	   _factory		  = nullptr;
		static Microsoft::WRL::ComPtr<IDxcLibrary> s_idxcLib;

		vector<D3D12_CPU_DESCRIPTOR_HANDLE> _reuse_dest_descriptors_buffer	= {};
		vector<D3D12_CPU_DESCRIPTOR_HANDLE> _reuse_dest_descriptors_sampler = {};
		vector<D3D12_CPU_DESCRIPTOR_HANDLE> _reuse_src_descriptors_buffer	= {};
		vector<D3D12_CPU_DESCRIPTOR_HANDLE> _reuse_src_descriptors_sampler	= {};

		resource_id _queue_graphics = 0;
		resource_id _queue_transfer = 0;
		resource_id _queue_compute	= 0;
	};
}