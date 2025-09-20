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
#include "io/file_system.hpp"
using json = nlohmann::json;
#endif

namespace SFG
{

#ifdef SFG_TOOLMODE

	void to_json(nlohmann::json& j, const parameter_entry& p)
	{
		j = {{"name", p.name}, {"value", p.value}};
	}

	void from_json(const nlohmann::json& j, parameter_entry& p)
	{
		p.name	= j.at("name").get<std::string>();
		p.value = j.at("value");
	}

	void to_json(nlohmann::json& j, const material_meta& s)
	{
		j["is_forward"] = s.is_forward;
		j["is_opaque"]	= s.is_opaque;
		j["textures"]	= s.textures;
		j["shaders"]	= s.shaders;
		j["parameters"] = s.parameters;
	}
	void from_json(const nlohmann::json& j, material_meta& s)
	{
		s.is_forward = j.value<uint8>("is_forward", 0);
		s.is_opaque	 = j.value<uint8>("is_opaque", 0);
		s.shaders	 = j.value<vector<string>>("shaders", {});
		s.textures	 = j.value<vector<string>>("textures", {});
		if (j.contains("parameters"))
			s.parameters = j.at("parameters").get<std::vector<parameter_entry>>();
	}

	bool material::create_from_file(const char* file, world_resources& resources)
	{
		if (!file_system::exists(file))
		{
			SFG_ERR("File doesn't exists! {0}", file);
			return false;
		}

		try
		{
			std::ifstream f(file);
			json		  json_data = json::parse(f);
			material_meta meta		= json_data;
			f.close();

			SFG_ASSERT(!meta.shaders.empty());
			_flags.set(material::flags::is_opaque, meta.is_opaque);
			_flags.set(material::flags::is_forward, meta.is_forward);

			reset_material_data();

			static_vector<string_id, 10> textures;
			SFG_ASSERT(meta.textures.size() < 10);
			for (const string& txt : meta.textures)
				textures.push_back(TO_SID(txt));

			_flags = 0;

			for (const parameter_entry& p : meta.parameters)
			{
				const auto& param = p.value;

				if (param.is_number_unsigned())
				{
					uint32 val = param.get<uint32>();
					_material_data << val;
				}
				else if (param.is_number_float())
				{
					float val = param.get<float>();
					_material_data << val;
				}
				else if (param.is_object() && param.contains("x") && param.contains("y") && param.contains("z") && param.contains("w") && param.size() == 4)
				{
					vector4 val = param.get<vector4>();
					_material_data << val;
				}
				else if (param.is_object() && param.contains("x") && param.contains("y") && param.contains("z") && param.size() == 3)
				{
					vector3 val = param.get<vector3>();
					_material_data << val;
				}
				else if (param.is_object() && param.contains("x") && param.contains("y") && param.size() == 2)
				{
					vector2 val = param.get<vector2>();
					_material_data << val;
				}
				else
				{
					SFG_ASSERT(false);
				}
			}

			for (const string& sh : meta.shaders)
				_all_shaders.push_back(resources.get_shader_handle_by_hash(TO_SID(sh)));
			SFG_ASSERT(!_all_shaders.empty());
			_default_shader = resources.get_shader(_all_shaders[0]).get_hw();

			close_material_data(resources, textures.data(), static_cast<uint8>(textures.size()));
		}
		catch (std::exception e)
		{
			SFG_ERR("Failed loading material: {0}", e.what());
			return false;
		}

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

		if (_material_data.get_size() != 0)
			_material_data.destroy();
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