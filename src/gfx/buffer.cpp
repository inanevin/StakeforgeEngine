// Copyright (c) 2025 Inan Evin

#include "buffer.hpp"
#include "io/assert.hpp"
#include "io/log.hpp"
#include "memory/memory.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/common/descriptions.hpp"
#include "gfx/common/commands.hpp"

namespace SFG
{

	void buffer::create_staging_hw(const resource_desc& staging, const resource_desc& hw)
	{
		gfx_backend* backend = gfx_backend::get();
		_total_size			 = staging.size;
		_hw_staging			 = backend->create_resource(staging);
		_hw_gpu				 = backend->create_resource(hw);
		backend->map_resource(_hw_staging, _mapped);
		_flags.set(buf_has_staging | buf_alive);
	}

	void buffer::create_hw(const resource_desc& desc)
	{
		gfx_backend* backend = gfx_backend::get();
		_total_size			 = desc.size;
		_hw_gpu				 = backend->create_resource(desc);
		backend->map_resource(_hw_gpu, _mapped);
	}

	void buffer::destroy()
	{
		gfx_backend* backend = gfx_backend::get();
		if (_flags.is_set(buffer_flags::buf_has_staging))
			backend->destroy_resource(_hw_staging);

		backend->destroy_resource(_hw_gpu);
		_mapped = nullptr;
		_flags.remove(buffer_flags::buf_alive);
		_flags.remove(buffer_flags::buf_has_staging);
	}

	void buffer::buffer_data(size_t padding, const void* data, size_t size)
	{
		SFG_ASSERT(padding + size <= _total_size);
		SFG_MEMCPY(_mapped + padding, data, size);
		_flags.set(buf_dirty);
	}

	bool buffer::copy(gfx_id cmd_buffer)
	{
		if (!_flags.is_set(buf_dirty) || !_flags.is_set(buffer_flags::buf_has_staging))
			return false;

		gfx_backend* backend = gfx_backend::get();
		backend->cmd_copy_resource(cmd_buffer,
								   {
									   .source		= _hw_staging,
									   .destination = _hw_gpu,
								   });
		_flags.remove(buf_dirty);
		return true;
	}

} // namespace Lina
