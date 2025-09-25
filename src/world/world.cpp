// Copyright (c) 2025 Inan Evin

#include "world.hpp"
#include "io/log.hpp"
#include "io/file_system.hpp"
#include "project/engine_data.hpp"
#include "app/debug_console.hpp"

#ifdef SFG_TOOLMODE
#include <fstream>
#include <vendor/nhlohmann/json.hpp>
using json = nlohmann::json;
#endif

/* DEBUG */
#include <algorithm>
#include <execution>
#include "gfx/renderer.hpp"
#include "resources/texture.hpp"
#include "resources/texture_raw.hpp"
#include "resources/shader.hpp"
#include "resources/material.hpp"
#include "resources/model.hpp"
#include "resources/skin.hpp"
#include "resources/mesh.hpp"
#include "resources/animation.hpp"
#include "platform/time.hpp"
#include "gfx/world/world_renderer.hpp"
#include "resources/model_raw.hpp"
#include "resources/model_node.hpp"
#include "resources/primitive.hpp"
#include "world/traits/trait_mesh_renderer.hpp"

namespace SFG
{

	world::world()
		: _entity_manager(*this){

		  };

	world::~world()
	{
	}

	void world::init()
	{
		SFG_PROG("initializing world.");
		_flags.set(world_flags_is_init);
		debug_console::get()->register_console_function<int>("world_set_play", [this](int b) { _flags.set(world_flags_is_playing, b != 0); });
		_resources.init(this);
		_entity_manager.init();
	}

	static vector<const char*> debug_textures = {
		"assets/boombox/boombox_base.stkfrg",
		"assets/boombox/boombox_emissive.stkfrg",
		"assets/boombox/boombox_normal.stkfrg",
		"assets/boombox/boombox_orm.stkfrg",
		"assets/cesium_man/cesium_man_base.stkfrg",
	};

	static vector<const char*> debug_mats = {
		"assets/boombox/boombox_material.stkfrg",
		//"assets/cesium_man/cesium_man_material.stkfrg",
	};

	static vector<const char*> debug_shaders = {
		"assets/shaders/object/default_lit.stkfrg",
		"assets/shaders/object/default_lit_skinned.stkfrg",
	};

	static vector<const char*> debug_models = {
		"assets/boombox/boombox.gltf",
		"assets/cesium_man/CesiumMan.gltf",
	};

	static vector<resource_handle> loaded_debug_textures	  = {};
	static vector<resource_handle> loaded_debug_mats		  = {};
	static vector<resource_handle> loaded_debug_shaders		  = {};
	static vector<model_raw>	   loaded_debug_models_loaded = {};
	static vector<resource_handle> loaded_debug_models		  = {};

	void world::load_debug()
	{
		/*
		entity_handle parent = _entity_manager.create_entity();
		entity_handle child	 = _entity_manager.create_entity();
		_entity_manager.add_child(parent, child);

		auto report = [&]() {
			SFG_INFO("*************** Report ***************");

			const vector3 parent_pos   = _entity_manager.get_entity_position(parent);
			const vector3 parent_rot   = quat::to_euler(_entity_manager.get_entity_rotation(parent));
			const vector3 parent_scale = _entity_manager.get_entity_scale(parent);

			const vector3 parent_pos_abs   = _entity_manager.get_entity_position_abs(parent);
			const vector3 parent_rot_abs   = quat::to_euler(_entity_manager.get_entity_rotation_abs(parent));
			const vector3 parent_scale_abs = _entity_manager.get_entity_scale_abs(parent);
			SFG_INFO("parent local -> position: x:{0} y:{1} z:{2}, rotation: x:{3} y:{4} z:{5}, scale: x:{6} y:{7} z:{8}", parent_pos.x, parent_pos.y, parent_pos.z, parent_rot.x, parent_rot.y, parent_rot.z, parent_scale.x, parent_scale.y, parent_scale.z);
			SFG_INFO("parent abs   -> position: x:{0} y:{1} z:{2}, rotation: x:{3} y:{4} z:{5}, scale: x:{6} y:{7} z:{8}",
					 parent_pos_abs.x,
					 parent_pos_abs.y,
					 parent_pos_abs.z,
					 parent_rot_abs.x,
					 parent_rot_abs.y,
					 parent_rot_abs.z,
					 parent_scale_abs.x,
					 parent_scale_abs.y,
					 parent_scale_abs.z);

			const vector3 child_pos	  = _entity_manager.get_entity_position(child);
			const vector3 child_rot	  = quat::to_euler(_entity_manager.get_entity_rotation(child));
			const vector3 child_scale = _entity_manager.get_entity_scale(child);

			const vector3 child_pos_abs	  = _entity_manager.get_entity_position_abs(child);
			const vector3 child_rot_abs	  = quat::to_euler(_entity_manager.get_entity_rotation_abs(child));
			const vector3 child_scale_abs = _entity_manager.get_entity_scale_abs(child);
			SFG_INFO("               ");
			SFG_INFO("child local -> position: x:{0} y:{1} z:{2}, rotation: x:{3} y:{4} z:{5}, scale: x:{6} y:{7} z:{8}", child_pos.x, child_pos.y, child_pos.z, child_rot.x, child_rot.y, child_rot.z, child_scale.x, child_scale.y, child_scale.z);
			SFG_INFO("child abs   -> position: x:{0} y:{1} z:{2}, rotation: x:{3} y:{4} z:{5}, scale: x:{6} y:{7} z:{8}",
					 child_pos_abs.x,
					 child_pos_abs.y,
					 child_pos_abs.z,
					 child_rot_abs.x,
					 child_rot_abs.y,
					 child_rot_abs.z,
					 child_scale_abs.x,
					 child_scale_abs.y,
					 child_scale_abs.z);
		};

		report();

		_entity_manager.set_entity_position(parent, vector3(2, 5, 2));
		_entity_manager.set_entity_position_abs(child, vector3(0, 0, 0));

		_entity_manager.set_entity_rotation(parent, quat::from_euler(15, 30, 150));
		_entity_manager.set_entity_rotation_abs(child, quat::from_euler(0, 0, 100));

		_entity_manager.set_entity_scale(parent, vector3(5, 10, 2));
		_entity_manager.set_entity_scale(child, vector3(1, 1, 1));
		report();
		return;
		*/

		vector<std::function<void()>> tasks;

		const int64 mr_begin = time::get_cpu_microseconds();

		/* Debug
		for (const char* t : debug_textures)
		{
			const resource_handle handle = _resources.create_resource<texture>(TO_SIDC(t));
			loaded_debug_textures.push_back(handle);
			const string abs = engine_data::get().get_working_dir() + t;
			tasks.push_back([handle, abs, this]() {
				texture_raw raw;
				texture& txt = _resources.get_resource<texture>(handle);
				raw.create_from_file(abs.c_str());
				raw.populate(txt);

			});
		}

		for (const char* t : debug_shaders)
		{
			const resource_handle handle = _resources.create_resource<shader>(TO_SIDC(t));
			loaded_debug_shaders.push_back(handle);
			const string abs = engine_data::get().get_working_dir() + t;
			tasks.push_back([handle, abs, this]() { _resources.get_resource<shader>(handle).create_from_file_vertex_pixel(abs.c_str(), false, renderer::get_bind_layout_global()); });
		}

		std::for_each(std::execution::par, tasks.begin(), tasks.end(), [](std::function<void()>& task) { task(); });

		tasks.clear();

		for (const char* t : debug_mats)
		{
			const resource_handle handle = _resources.create_resource<material>(TO_SIDC(t));
			loaded_debug_mats.push_back(handle);
			const string abs = engine_data::get().get_working_dir() + t;
			tasks.push_back([handle, abs, this]() { _resources.get_resource<material>(handle).create_from_file(abs.c_str(), _resources); });
		}

		loaded_debug_models_loaded.resize(debug_models.size());
		int i = 0;
		for (const char* t : debug_models)
		{
			const resource_handle handle = _resources.create_resource<model>(TO_SIDC(t));
			loaded_debug_models.push_back(handle);

			const string relative = t;
			const string abs	  = engine_data::get().get_working_dir() + relative;
			const int	 index	  = i;
			tasks.push_back([handle, abs, relative, this, index]() { loaded_debug_models_loaded[index].create_from_file(abs.c_str(), relative.c_str()); });
			i++;
		}

		std::for_each(std::execution::par, tasks.begin(), tasks.end(), [](std::function<void()>& task) { task(); });

		i = 0;
		for (resource_handle handle : loaded_debug_models)
		{
			model& mdl = _resources.get_resource<model>(handle);
			loaded_debug_models_loaded[i].cook(mdl, _resources.get_aux(), _resources);
			i++;
		}

		for (resource_handle h : loaded_debug_textures)
		{
			_world_renderer->get_resource_uploads().add_pending_texture(&_resources.get_resource<texture>(h));
		}

		for (resource_handle h : loaded_debug_models)
		{
			model&				 m				= _resources.get_resource<model>(h);
			const chunk_handle32 created_meshes = m.get_created_meshes();
			const uint16		 meshes_count	= m.get_mesh_count();

			chunk_allocator32& aux	   = _resources.get_aux();
			resource_handle*   handles = meshes_count == 0 ? nullptr : aux.get<resource_handle>(created_meshes);

			for (uint16 i = 0; i < meshes_count; i++)
			{
				resource_handle& handle = handles[i];
				mesh&			 m		= _resources.get_resource<mesh>(handle);
				_world_renderer->get_resource_uploads().add_pending_mesh(&m);
			}
		}

		for (resource_handle h : loaded_debug_mats)
		{
			_world_renderer->get_resource_uploads().add_pending_material(&_resources.get_resource<material>(h));
		}

		const int64 mr_diff = time::get_cpu_microseconds() - mr_begin;
		SFG_INFO("Resources took: {0} ms", mr_diff / 1000);

		if (!loaded_debug_models.empty())
			add_model_to_world(loaded_debug_models[0], &loaded_debug_mats[0], 1);
			*/
	}

	void world::uninit()
	{
		for (resource_handle handle : loaded_debug_textures)
		{
			_resources.get_resource<texture>(handle).destroy();
			_resources.destroy_resource<texture>(handle);
		}

		for (resource_handle handle : loaded_debug_mats)
		{
			_resources.get_resource<material>(handle).destroy();
			_resources.destroy_resource<material>(handle);
		}

		for (resource_handle handle : loaded_debug_shaders)
		{
			_resources.get_resource<shader>(handle).destroy();
			_resources.destroy_resource<shader>(handle);
		}

		for (resource_handle handle : loaded_debug_models)
		{
			_resources.get_resource<model>(handle).destroy(_resources, _resources.get_aux());

			_resources.destroy_resource<model>(handle);
		}

		_entity_manager.uninit();
		_resources.uninit();
		_txt_allocator.reset();

		SFG_PROG("uninitializing world.");
		_flags.remove(world_flags_is_init);
		debug_console::get()->unregister_console_function("world_set_play");
	}

	void world::tick(uint8 data_index, const vector2ui16& res, float dt)
	{
	}

	void world::pre_render(uint8 data_index, const vector2ui16& res)
	{
	}

	entity_handle world::add_model_to_world(resource_handle handle, resource_handle* materials, uint32 material_size)
	{
		entity_handle	   root = _entity_manager.create_entity("root");
		model&			   mdl	= _resources.get_resource<model>(handle);
		chunk_allocator32& aux	= _resources.get_aux();

		const chunk_handle32 meshes		  = mdl.get_created_meshes();
		const chunk_handle32 nodes		  = mdl.get_nodes();
		const uint16		 meshes_count = mdl.get_mesh_count();
		const uint16		 nodes_count  = mdl.get_node_count();

		if (nodes_count == 0 || meshes_count == 0)
			return {};

		model_node*		 ptr_nodes		   = aux.get<model_node>(nodes);
		resource_handle* ptr_meshes_handle = aux.get<resource_handle>(meshes);

		// create nodes.
		static_vector<entity_handle, 128> created_node_entities;
		static_vector<entity_handle, 128> root_entities;
		for (uint16 i = 0; i < nodes_count; i++)
		{
			model_node&			node = ptr_nodes[i];
			const char*			name = reinterpret_cast<const char*>(aux.get(node.get_name().head));
			const entity_handle e	 = _entity_manager.create_entity(name);
			created_node_entities.push_back(e);
			if (node.get_parent_index() == -1)
				root_entities.push_back(e);
		}

		// assign parent child.
		for (uint16 i = 0; i < nodes_count; i++)
		{
			model_node& node = ptr_nodes[i];
			if (node.get_parent_index() != -1)
				_entity_manager.add_child(created_node_entities[node.get_parent_index()], created_node_entities[i]);
		}

		// assign transform hierarchy
		for (uint16 i = 0; i < nodes_count; i++)
		{
			model_node& node	  = ptr_nodes[i];
			vector3		out_pos	  = vector3::zero;
			quat		out_rot	  = quat::identity;
			vector3		out_scale = vector3::zero;
			node.get_local_matrix().decompose(out_pos, out_rot, out_scale);
			_entity_manager.set_entity_position(created_node_entities[i], out_pos);
			_entity_manager.set_entity_rotation(created_node_entities[i], out_rot);
			_entity_manager.set_entity_scale(created_node_entities[i], out_scale);
		}

		auto report_entity = [this](entity_handle e) {
			const entity_meta& meta	 = _entity_manager.get_entity_meta(e);
			const vector3&	   pos	 = _entity_manager.get_entity_position(e);
			const quat&		   rot	 = _entity_manager.get_entity_rotation(e);
			const vector3&	   scale = _entity_manager.get_entity_scale(e);
			SFG_INFO("entity: {0}", meta.name);
			SFG_INFO("position: {0} {1} {2}", pos.x, pos.y, pos.z);
			SFG_INFO("rotation: {0} {1} {2} {3}", rot.x, rot.y, rot.z, rot.w);
			SFG_INFO("scale: {0} {1} {2}", scale.x, scale.y, scale.z);
		};

		for (entity_handle e : root_entities)
		{
			SFG_INFO("reporting root...");
			report_entity(e);
			SFG_INFO("reporting children...");
			_entity_manager.visit_children(e, [&](entity_handle c) { report_entity(c); });
		}

		for (uint16 i = 0; i < meshes_count; i++)
		{
			resource_handle handle = ptr_meshes_handle[i];
			mesh&			m	   = _resources.get_resource<mesh>(handle);

			const uint16 mat_count = m.get_material_count();
			SFG_ASSERT(mat_count != 0);
			uint16* material_indices = aux.get<uint16>(m.get_material_indices());

			trait_handle		 trait		 = _entity_manager.add_trait<trait_mesh_renderer>(created_node_entities[m.get_node_index()]);
			trait_mesh_renderer& t			 = _entity_manager.get_trait<trait_mesh_renderer>(trait);
			t.material_count				 = mat_count;
			t.mesh							 = handle;
			t.materials						 = aux.allocate<resource_handle>(mat_count);
			resource_handle* trait_materials = aux.get<resource_handle>(t.materials);

			for (uint16 j = 0; j < mat_count; j++)
			{
				uint16 index = material_indices[j];
				SFG_ASSERT(index < material_size);
				trait_materials[j] = materials[index];
			}
		}

		return root;
	}

#ifdef SFG_TOOLMODE

	void world::save(const char* path)
	{
		json j;
		j["dummy"] = 2;

		std::ofstream file(path);

		if (file.is_open())
		{
			file << j.dump(4);
			file.close();
		}

		SFG_PROG("saved world: {0}", path);
	}

	void world::load(const char* path)
	{
		if (_flags.is_set(world_flags_is_init))
			uninit();

		if (strlen(path) == 0)
		{
			init();
			return;
		}

		if (!file_system::exists(path))
		{
			SFG_ERR("can't load world as path don't exist! {0}", path);
			return;
		}

		std::ifstream f(path);
		json		  data	= json::parse(f);
		int			  dummy = data["dummy"];
		SFG_PROG("loaded world {0}", path);

		init();
	}
#endif

	bool world::on_window_event(const window_event& ev)
	{
		if (!_flags.is_set(world_flags_is_init))
			return false;
		return false;
	}
}