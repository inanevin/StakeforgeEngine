// Copyright (c) 2025 Inan Evin
#pragma once

#include "data/vector.hpp"
#include "data/bitmask.hpp"
#include "animation.hpp"
#include "skin.hpp"
#include "mesh.hpp"
#include "model_node.hpp"
#include "math/aabb.hpp"

namespace SFG
{
	class world_resources;

	class model
	{
	public:
		enum flags
		{
			pending_upload = 1 << 0,
		};
#ifdef SFG_TOOLMODE
		bool create_from_file(const char* file, const char* relative_path, world_resources& r);
#endif

		inline bitmask<uint8>& get_flags()
		{
			return _flags;
		}

		inline vector<mesh>& get_meshes()
		{
			return _all_meshes;
		}

		inline const aabb& get_total_aabb() const
		{
			return _total_aabb;
		}

		inline const vector<model_node>& get_nodes() const
		{
			return _all_nodes;
		}

		inline uint8 get_material_count() const
		{
			return _material_count;
		}

	private:
		vector<model_node> _all_nodes;
		vector<mesh>	   _all_meshes;
		aabb			   _total_aabb;
		bitmask<uint8>	   _flags;
		uint8			   _material_count = 0;
	};
}