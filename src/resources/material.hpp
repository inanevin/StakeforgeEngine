// Copyright (c) 2025 Inan Evin

#pragma once
#include "data/static_vector.hpp"
#include "data/bitmask.hpp"
#include "data/ostream.hpp"
#include "gfx/buffer.hpp"
#include "gfx/common/gfx_constants.hpp"
#include "resources/common_resources.hpp"

namespace SFG
{
	class world_resources;
	struct material_raw;

	class material
	{
	public:
		static constexpr uint32 TYPE_INDEX = resource_types::resource_type_material;

		enum flags
		{
			is_opaque  = 1 << 0,
			is_forward = 1 << 1,
		};

		void   create_from_raw(const material_raw& raw, world_resources& resources);
		void   destroy();
		gfx_id get_shader(world_resources& resources, uint8 flags_to_match) const;

		inline bool is_dirty(uint8 frame_index) const
		{
			return _buffers[frame_index].is_dirty();
		}

		inline gfx_id get_gpu(uint8 frame_index) const
		{
			return _bind_groups[frame_index];
		}

		inline gfx_id get_shader() const
		{
			return _default_shader;
		}

		inline const bitmask<uint8>& get_flags() const
		{
			return _flags;
		}

		inline buffer& get_buffer(uint8 frame_index)
		{
			return _buffers[frame_index];
		}

		inline ostream& get_data()
		{
			return _material_data;
		}

	private:
		ostream														 _material_data				= {};
		buffer														 _buffers[FRAMES_IN_FLIGHT] = {};
		static_vector<resource_handle, MAX_MATERIAL_SHADER_VARIANTS> _all_shaders;
		gfx_id														 _default_shader				= 0;
		gfx_id														 _bind_groups[FRAMES_IN_FLIGHT] = {};
		bitmask<uint8>												 _flags							= 0;
	};

}
