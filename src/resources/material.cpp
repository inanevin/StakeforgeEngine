// Copyright (c) 2025 Inan Evin

#include "material.hpp"
#include "data/string.hpp"
#include "gfx/common/descriptions.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/util/gfx_util.hpp"
#include "io/assert.hpp"
#include "world/world_resources.hpp"
#include "resources/texture.hpp"
#include "math/vector2.hpp"
#include "data/static_vector.hpp"

#ifdef SFG_TOOLMODE
#include <fstream>
#include "vendor/nhlohmann/json.hpp"
#include "io/file_system.hpp"
using json = nlohmann::json;
#endif

namespace SFG
{

#ifdef SFG_TOOLMODE

	bool material::create_from_file(const char* file, world_resources& resources)
	{
		if (!file_system::exists(file))
		{
			SFG_ERR("File doesn't exists! {0}", file);
			return false;
		}

		std::ifstream f(file);
		json		  json_data = json::parse(f);

		reset_material_data();

		static_vector<string_id, 10> textures;

		_flags = 0;

		for (auto& [key, value] : json_data.items())
		{
			if (key.compare("shaders") == 0 && value.is_array())
			{
				const vector<string> shaders = value.get<vector<string>>();
				for (const string& sh : shaders)
					_all_shaders.push_back(resources.get_shader_handle_by_hash(TO_SID(sh)));
			}
			else if (key.compare("is_opaque") == 0)
			{
				_flags.set(material::flags::is_opaque);
			}
			else if (key.compare("is_forward") == 0)
			{
				_flags.set(material::flags::is_opaque);
			}
			else if (value.is_number_unsigned())
			{
				uint32 val = value.get<uint32>();
				_material_data << val;
			}
			else if (value.is_number_float())
			{
				float val = value.get<float>();
				_material_data << val;
			}
			else if (value.is_object() && value.contains("x") && value.contains("y") && value.contains("z") && value.contains("w") && value.size() == 4)
			{
				vector4 val = value.get<vector4>();
				_material_data << val;
			}
			else if (value.is_object() && value.contains("x") && value.contains("y") && value.contains("z") && value.size() == 3)
			{
				vector3 val = value.get<vector3>();
				_material_data << val;
			}
			else if (value.is_object() && value.contains("x") && value.contains("y") && value.size() == 2)
			{
				vector2 val = value.get<vector2>();
				_material_data << val;
			}
			else if (value.is_string())
			{
				if (value.contains("sampler"))
				{
					// sampler deconstruct?
				}
				else
				{
					const string	txt_path = value.get<string>();
					const string_id sid		 = TO_SID(txt_path);
					textures.push_back(sid);
				}
			}
			else
			{
				SFG_ASSERT(false);
			}
		}

		SFG_ASSERT(!_all_shaders.empty());
		_default_shader = resources.get_shader(_all_shaders[0]).get_hw();

		f.close();
		close_material_data(resources, textures.data(), static_cast<uint8>(textures.size()));

		return true;
	}

	void material::reset_material_data()
	{
		if (_material_data.get_size() != 0)
			_material_data.destroy();

		_material_data.create(16);
	}

	void material::set_material_data(size_t padding, uint8* data, size_t sz)
	{
		SFG_MEMCPY(_material_data.get_raw() + padding, data, sz);
	}

	void material::add_material_data(uint8* data, size_t sz)
	{
		_material_data.write_raw(data, sz);
	}

	void material::close_material_data(world_resources& resources, string_id* textures, uint8 texture_count)
	{
		gfx_backend* backend = gfx_backend::get();

		for (uint8 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			if (_buffers[i].is_alive())
			{
				_buffers[i].destroy();
				backend->destroy_bind_group(_bind_groups[i]);
			}

			_buffers[i].create_staging_hw(
				{
					.size		= static_cast<uint32>(_material_data.get_size()),
					.flags		= resource_flags::rf_constant_buffer | resource_flags::rf_cpu_visible,
					.debug_name = "material_data",
				},
				{
					.size		= static_cast<uint32>(_material_data.get_size()),
					.flags		= resource_flags::rf_constant_buffer | resource_flags::rf_gpu_only,
					.debug_name = "material_data",
				});

			_bind_groups[i] = backend->create_empty_bind_group();
			backend->bind_group_add_pointer(_bind_groups[i], rpi_table_material, 3, false);

			vector<bind_group_pointer> updates;
			updates.push_back({
				.resource	   = _buffers[i].get_hw_gpu(),
				.pointer_index = upi_material_ubo0,
				.type		   = binding_type::ubo,
			});
			for (uint8 k = 0; k < texture_count; k++)
			{
				updates.push_back({
					.resource	   = resources.get_texture_by_hash(textures[k]).get_hw(),
					.pointer_index = static_cast<uint8>(upi_material_texture0 + k),
					.type		   = binding_type::texture_binding,
				});
			}
			backend->bind_group_update_pointer(_bind_groups[i], 0, updates);
		}

		update_material_data();
	}

	gfx_id material::get_shader(world_resources& resources, uint8 flags_to_match) const
	{
		for (pool_handle<resource_id> handle : _all_shaders)
		{
			const shader& sh = resources.get_shader(handle);
			if (sh.get_flags().is_set(flags_to_match))
				return sh.get_hw();
		}
		SFG_ASSERT(false);
		return _default_shader;
	}

	void material::destroy()
	{
		gfx_backend* backend = gfx_backend::get();

		for (uint8 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			SFG_ASSERT(_buffers[i].is_alive());
			_buffers[i].destroy();
			backend->destroy_bind_group(_bind_groups[i]);
		}
	}

	void material::update_material_data()
	{
		for (uint8 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			SFG_ASSERT(_buffers[i].is_alive());
			_buffers[i].buffer_data(0, &_material_data, _material_data.get_size());
			_buffers[i].buffer_data(0, &_material_data, _material_data.get_size());
		}
	}

#endif
}