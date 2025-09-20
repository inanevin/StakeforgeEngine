// Copyright (c) 2025 Inan Evin

#pragma once

#include "math/vector4.hpp"
#include "data/string_id.hpp"
#include "data/static_vector.hpp"
#include "data/bitmask.hpp"
#include "data/ostream.hpp"
#include "gfx/buffer.hpp"
#include "memory/pool_handle.hpp"

#ifdef SFG_TOOLMODE
#include "vendor/nhlohmann/json.hpp"
#endif

namespace SFG
{
	class world_resources;

#define MAX_MATERIAL_SHADER_VARIANTS 8

	struct parameter_entry
	{
		std::string name;

#ifdef SFG_TOOLMODE
		nlohmann::json value;
#endif
	};

	struct material_meta
	{
		uint8					is_forward = 0;
		uint8					is_opaque  = 0;
		vector<string>			shaders;
		vector<string>			textures;
		vector<parameter_entry> parameters;
	};

#ifdef SFG_TOOLMODE

	void to_json(nlohmann::json& j, const parameter_entry& s);
	void from_json(const nlohmann::json& j, parameter_entry& s);

	void to_json(nlohmann::json& j, const material_meta& s);
	void from_json(const nlohmann::json& j, material_meta& s);

#endif

	class material
	{
	private:
	public:
		enum flags
		{
			is_opaque  = 1 << 0,
			is_forward = 1 << 1,
		};

		~material()
		{
			if (_material_data.get_size() != 0)
				_material_data.destroy();
		}

#ifdef SFG_TOOLMODE
		bool create_from_file(const char* file, world_resources& resources);
#endif

		void   destroy();
		void   update_material_data();
		void   reset_material_data();
		void   set_material_data(size_t padding, uint8* data, size_t sz);
		void   add_material_data(uint8* data, size_t sz);
		void   close_material_data(world_resources& resources, string_id* textures, uint8 texture_count);
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

	private:
		ostream															 _material_data				= {};
		buffer															 _buffers[FRAMES_IN_FLIGHT] = {};
		static_vector<pool_handle<gfx_id>, MAX_MATERIAL_SHADER_VARIANTS> _all_shaders;
		gfx_id															 _default_shader				= 0;
		gfx_id															 _bind_groups[FRAMES_IN_FLIGHT] = {};
		bitmask<uint8>													 _flags							= 0;
	};

}
