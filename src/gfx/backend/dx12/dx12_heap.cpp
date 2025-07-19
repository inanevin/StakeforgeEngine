// Copyright (c) 2025 Inan Evin

#pragma once

#include "dx12_heap.hpp"
#include "io/log.hpp"
#include "io/assert.hpp"

namespace Game
{

	void dx12_heap::init(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heap_type, uint32 num_descriptors, uint32 descriptor_size, bool shader_access)
	{
		_type			 = heap_type;
		_max_descriptors = num_descriptors;
		_shader_access	 = shader_access;
		_available_blocks.reserve(num_descriptors / 2);
		_descriptor_size = descriptor_size;

		try
		{
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
			heapDesc.NumDescriptors = _max_descriptors;
			heapDesc.Type			= _type;
			heapDesc.Flags			= _shader_access ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			heapDesc.NodeMask		= 0;
			throw_if_failed(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_heap)));
		}
		catch (HrException e)
		{
			GAME_ERR("Exception when creating a descriptor heap! {0}", e.what());
		}

		_heap->SetName(L"Descriptor Heap");
		_cpu_start = _heap->GetCPUDescriptorHandleForHeapStart();
		if (_shader_access)
			_gpu_start = _heap->GetGPUDescriptorHandleForHeapStart();
	}

	void dx12_heap::uninit()
	{
		_heap->Release();
		_heap = NULL;
	}

	void dx12_heap::reset()
	{
		_current_index = 0;
	}

	void dx12_heap::reset(uint32 newStart)
	{
		_current_index = newStart;
	}

	descriptor_handle dx12_heap::get_heap_handle_block(uint32 count)
	{
		for (auto it = _available_blocks.begin(); it != _available_blocks.end(); ++it)
		{
			auto& block = *it;

			if (block.count >= count)
			{
				const descriptor_handle handle = {
					.cpu   = _cpu_start.ptr + block.start * _descriptor_size,
					.gpu   = _gpu_start.ptr + block.start * _descriptor_size,
					.index = block.start,
					.count = count,
				};

				block.start += count;
				block.count -= count;

				if (block.count == 0)
					_available_blocks.erase(it);

				return handle;
			}
		}

		uint32 new_id	 = 0;
		uint32 block_end = _current_index + count;

		if (block_end < _max_descriptors)
		{
			new_id		   = _current_index;
			_current_index = block_end;
		}
		else
		{
			GAME_ASSERT(false);
			GAME_ERR("DX12Backend -> Ran out of descriptor heap handles, need to increase heap size.");
		}

		return {
			.cpu   = _cpu_start.ptr + new_id * _descriptor_size,
			.gpu   = _gpu_start.ptr + new_id * _descriptor_size,
			.index = block_end - count,
			.count = count,
		};
	}

	descriptor_handle dx12_heap::get_offsetted_handle(uint32 count)
	{
		return {
			.cpu = get_cpu_start().ptr + count * get_descriptor_size(),
			.gpu = get_gpu_start().ptr + count * get_descriptor_size(),
		};
	}

	void dx12_heap::remove_handle(const descriptor_handle& handle)
	{
		const auto start = handle.index;
		block	   b	 = {handle.index, handle.count};
		_available_blocks.push_back(b);
	}
} // namespace LinaGX
