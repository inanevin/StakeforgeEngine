// Copyright (c) 2025 Inan Evin

#pragma once

#include "data/vector.hpp"
#include "gfx/common/gfx_common.hpp"
#include "sdk/d3d12.h"

namespace SFG
{
	class dx12_heap
	{
	private:
		struct block
		{
			uint32 start = 0;
			uint32 count = 0;
		};

	public:
		dx12_heap()	 = default;
		~dx12_heap() = default;

		void			  init(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heap_type, uint32 num_descriptors, uint32 descriptor_size, bool shader_access);
		void			  uninit();
		void			  reset();
		void			  reset(uint32 newStart);
		void			  remove_handle(const descriptor_handle& handle);
		descriptor_handle get_heap_handle_block(uint32 count);
		descriptor_handle get_offsetted_handle(uint32 count);

		inline ID3D12DescriptorHeap* get_heap()
		{
			return _heap;
		}

		inline D3D12_DESCRIPTOR_HEAP_TYPE get_type() const
		{
			return _type;
		}

		inline D3D12_CPU_DESCRIPTOR_HANDLE get_cpu_start() const
		{
			return _cpu_start;
		}

		inline D3D12_GPU_DESCRIPTOR_HANDLE get_gpu_start() const
		{
			return _gpu_start;
		}

		inline uint32 get_max_descriptors() const
		{
			return _max_descriptors;
		}

		inline uint32 get_descriptor_size() const
		{
			return _descriptor_size;
		}

		inline uint32 get_current_index()
		{
			return _current_index;
		};

	private:
		ID3D12DescriptorHeap*		_heap	   = nullptr;
		D3D12_DESCRIPTOR_HEAP_TYPE	_type	   = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		D3D12_CPU_DESCRIPTOR_HANDLE _cpu_start = {};
		D3D12_GPU_DESCRIPTOR_HANDLE _gpu_start = {};
		vector<block>				_available_blocks;
		uint32						_max_descriptors = 0;
		uint32						_descriptor_size = 0;
		uint32						_current_index	 = 0;
		bool						_shader_access	 = false;
	};
} // namespace LinaGX
