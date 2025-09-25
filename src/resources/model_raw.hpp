// Copyright (c) 2025 Inan Evin
#pragma once

#include "math/aabb.hpp"
#include "data/vector.hpp"
#include "animation_raw.hpp"
#include "skin_raw.hpp"
#include "mesh_raw.hpp"
#include "model_node_raw.hpp"

namespace SFG
{
	class ostream;
	class istream;

	struct model_raw
	{
		vector<model_node_raw> loaded_nodes;
		vector<mesh_raw>	   loaded_meshes;
		vector<skin_raw>	   loaded_skins;
		vector<animation_raw>  loaded_animations;
		aabb				   total_aabb;
		uint8				   material_count = 0;

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);

#ifdef SFG_TOOLMODE
		bool cook_from_file(const char* file, const char* relative_path);
#endif
	};
}
