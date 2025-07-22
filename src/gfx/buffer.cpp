/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "buffer.hpp"
#include "io/assert.hpp"
#include "memory/memory.hpp"

namespace Game
{
	atomic<uint64> buffer::_s_used_cpu_visible_mem = 0;

	void buffer::create(uint32 hintFlags, uint32 size, const string& dbg_name, bool staging_only)
	{
		_size = size;

		//_buffer_flags.set(bf_staging_only, staging_only);
		//
		// if (!staging_only && LinaGX::GPUInfo.totalCPUVisibleGPUMemorySize - _s_used_cpu_visible_mem.load() > _size)
		//{
		//	_s_used_cpu_visible_mem.fetch_add(static_cast<uint64>(_size));
		//	_buffer_flags.set(bf_cpu_visible_gpu | bf_resides_in_gpu);
		//
		//	const string		 dbgName = dbg_name + " (CPUVisibleGPUMem)";
		//	LinaGX::ResourceDesc desc	 = {
		//		   .size		  = static_cast<uint64>(_size),
		//		   .typeHintFlags = hintFlags,
		//		   .heapType	  = LinaGX::ResourceHeap::CPUVisibleGPUMemory,
		//		   .debugName	  = dbgName.c_str(),
		//	   };
		//
		//	_gpu = LinaGX::instance()->CreateResource(desc);
		//	LinaGX::instance()->MapResource(_gpu, _mapped);
		//}
		// else
		//{
		//	const string dbg_name_stg = dbg_name + " (Staging)";
		//	const string dbg_name_gpu = dbg_name + " (GPU)";
		//
		//	LinaGX::ResourceDesc desc = {
		//		.size		   = static_cast<uint64>(_size),
		//		.typeHintFlags = hintFlags,
		//		.heapType	   = LinaGX::ResourceHeap::StagingHeap,
		//		.debugName	   = dbg_name_stg.c_str(),
		//	};
		//
		//	_staging = LinaGX::instance()->CreateResource(desc);
		//
		//	if (!_buffer_flags.is_set(bf_staging_only))
		//	{
		//		desc.heapType  = LinaGX::ResourceHeap::GPUOnly;
		//		desc.debugName = dbg_name_gpu.c_str();
		//		_gpu		   = LinaGX::instance()->CreateResource(desc);
		//		_buffer_flags.set(bf_resides_in_gpu);
		//	}
		//
		//	LinaGX::instance()->MapResource(_staging, _mapped);
		//}
	}

	void buffer::set_data(size_t padding, uint8* data, size_t size)
	{
		if (size == 0)
			return;

		GAME_ASSERT(padding + size <= _size, "Buffer overflow!");
		GAME_MEMCPY(_mapped + padding, data, size);
		_buffer_flags.set(bf_dirty);
	}

	void buffer::memset_mapped(int32 v)
	{
		GAME_MEMSET(_mapped, v, _size);
		_buffer_flags.set(bf_dirty);
	}

	// bool buffer::copy(LinaGX::CommandStream* stream)
	//{
	//	// no need either we already wrote to gpu mem or memory buffer.
	//	if (_buffer_flags.is_set(bf_cpu_visible_gpu) || _buffer_flags.is_set(bf_staging_only)) return false;
	//
	//	if (!_buffer_flags.is_set(bf_dirty)) return false;
	//
	//	LinaGX::CMDCopyResource* copy = stream->AddCommand<LinaGX::CMDCopyResource>();
	//	copy->destination			  = _gpu;
	//	copy->source				  = _staging;
	//	_buffer_flags.remove(bf_dirty);
	//
	//	return true;
	// }

	void buffer::destroy()
	{
		// if (_buffer_flags.is_set(bf_resides_in_gpu))
		// {
		// 	LinaGX::instance()->UnmapResource(_gpu);
		// 	LinaGX::instance()->DestroyResource(_gpu);
		// }
		//
		// if (!_buffer_flags.is_set(bf_cpu_visible_gpu))
		// {
		// 	LinaGX::instance()->UnmapResource(_staging);
		// 	LinaGX::instance()->DestroyResource(_staging);
		// }
	}

	// void buffer::bind_vertex(LinaGX::CommandStream* stream, uint32 vertexSize)
	//{
	//	LinaGX::CMDBindVertexBuffers* vtx = stream->AddCommand<LinaGX::CMDBindVertexBuffers>();
	//	vtx->offset						  = 0;
	//	vtx->slot						  = 0;
	//	vtx->vertexSize					  = vertexSize;
	//	vtx->resource					  = get_gpu();
	// }
	//
	// void buffer::bind_index(LinaGX::CommandStream* stream, LinaGX::IndexType indexType)
	//{
	//	LinaGX::CMDBindIndexBuffers* index = stream->AddCommand<LinaGX::CMDBindIndexBuffers>();
	//	index->indexType				   = indexType;
	//	index->offset					   = 0;
	//	index->resource					   = get_gpu();
	// }

} // namespace Lina
