// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "data/vector.hpp"
#include "data/string.hpp"
#include <map>

namespace Game
{
	namespace gltf
	{
		struct primitive
		{
			std::map<string, int32_t> attributes;
			int32_t					  indices  = -1;
			int32_t					  material = -1;
		};

		struct mesh
		{
			string			  name = "";
			vector<primitive> primitives;
		};

		struct accessor
		{
			int32		  bufferView	= -1;
			int32		  componentType = -1;
			int32		  count			= -1;
			string		  type			= "";
			vector<float> max;
			vector<float> min;
		};

		struct buffer
		{
			string uri		  = "";
			uint32 byteLength = 0;
		};

		struct buffer_view
		{
			int32  buffer	  = -1;
			uint32 byteLength = 0;
			uint32 byteOffset = 0;
		};

		struct texture_ref
		{
			int32 index = -1;
		};

		struct pbr_metallic_roughness
		{
			texture_ref baseColorTexture = {};
			texture_ref metallicRoughnessTexture{};
		};

		struct material
		{
			string				   name					= "";
			pbr_metallic_roughness pbrMetallicRoughness = {};
			vector<float>		   emissiveFactor		= {0.0f, 0.0f, 0.0f};
			texture_ref			   normalTexture		= {};
			texture_ref			   occlusionTexture		= {};
			texture_ref			   emissiveTexture		= {};
		};

		struct image
		{
			string uri = "";
		};

		struct texture
		{
			int32 source = -1;
		};

		struct sampler
		{
		};

		struct node
		{
			int32		  mesh	   = -1;
			vector<float> position = {1.0f, 1.0f, 1.0f};
			vector<float> rotation = {0.0f, 0.0f, 0.0f, 1.0f};
			vector<float> scale	   = {1.0f, 1.0f, 1.0f};
			string		  name	   = "";
		};

		struct scene
		{
			vector<int32_t> nodes;
			string			name = "";
		};

		struct asset_data
		{
			string generator = "";
			string version	 = "";
		};

		struct gltf_json
		{
			vector<mesh>		meshes;
			vector<accessor>	accessors;
			vector<buffer>		buffers;
			vector<buffer_view> bufferViews;
			vector<material>	materials;
			vector<image>		images;
			vector<texture>		textures;
			vector<sampler>		samplers;
			vector<node>		nodes;
			vector<scene>		scenes;
			vector<string>		extensionsUsed;
			vector<string>		extensionsRequired;
			asset_data			asset = {};
			int32_t				scene = -1;
		};
	}

}
