// Copyright (c) 2025 Inan Evin

#include "material_pbr_default.hpp"
#include "data/string.hpp"
#include "gfx/common/descriptions.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/util/gfx_util.hpp"
#include "io/assert.hpp"
#include "world_resources.hpp"
#include "resources/texture.hpp"

#ifdef SFG_TOOLMODE
#include <fstream>
#include "vendor/nhlohmann/json.hpp"
#include "io/file_system.hpp"
using json = nlohmann::json;
#endif

namespace SFG
{

#ifdef SFG_TOOLMODE

	bool material_pbr_default::create_from_file(const char* file, world_resources& resources)
	{
		if (!file_system::exists(file))
		{
			SFG_ERR("File doesn't exists! {0}", file);
			return false;
		}

		std::ifstream f(file);
		json		  json_data		  = json::parse(f);
		_material_data.base_color	  = json_data.value<vector4>("base_color", vector4::one);
		const string_id albedo_hash	  = TO_SID(json_data.value<string>("albedo", ""));
		const string_id rma_hash	  = TO_SID(json_data.value<string>("rma", ""));
		const string_id normal_hash	  = TO_SID(json_data.value<string>("normal", ""));
		const string_id emissive_hash = TO_SID(json_data.value<string>("emissive", ""));
		f.close();

		const texture& albedo	= resources.get_texture_by_hash(albedo_hash);
		const texture& rma		= resources.get_texture_by_hash(rma_hash);
		const texture& normal	= resources.get_texture_by_hash(normal_hash);
		const texture& emissive = resources.get_texture_by_hash(emissive_hash);

		gfx_backend* backend = gfx_backend::get();

		for (uint8 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			SFG_ASSERT(!_buffers[i].is_alive());

			_buffers[i].create_staging_hw(
				{
					.size		= sizeof(material_data),
					.flags		= resource_flags::rf_constant_buffer | resource_flags::rf_cpu_visible,
					.debug_name = file,
				},
				{
					.size		= sizeof(material_data),
					.flags		= resource_flags::rf_constant_buffer | resource_flags::rf_gpu_only,
					.debug_name = file,
				});

			_bind_groups[i] = backend->create_empty_bind_group();
			backend->bind_group_add_pointer(_bind_groups[i], rpi_table_material, 3, false);
			backend->bind_group_update_pointer(_bind_groups[i],
											   0,
											   {
												   {.resource = _buffers[i].get_hw_gpu(), .pointer_index = upi_material_ubo0, .type = binding_type::ubo},
												   {.resource = albedo.get_hw(), .pointer_index = upi_material_texture0, .type = binding_type::texture_binding},
												   {.resource = rma.get_hw(), .pointer_index = upi_material_texture1, .type = binding_type::texture_binding},
												   {.resource = normal.get_hw(), .pointer_index = upi_material_texture2, .type = binding_type::texture_binding},
												   {.resource = emissive.get_hw(), .pointer_index = upi_material_texture3, .type = binding_type::texture_binding},
											   });
		}

		update_material_data();
		return true;
	}

	void material_pbr_default::destroy()
	{
		gfx_backend* backend = gfx_backend::get();

		for (uint8 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			SFG_ASSERT(_buffers[i].is_alive());
			_buffers[i].destroy();
			backend->destroy_bind_group(_bind_groups[i]);
		}
	}

	void material_pbr_default::update_material_data()
	{
		for (uint8 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			SFG_ASSERT(_buffers[i].is_alive());
			_buffers[i].buffer_data(0, &_material_data, sizeof(material_data));
		}
	}

#endif
}