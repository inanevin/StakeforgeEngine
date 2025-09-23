// Copyright (c) 2025 Inan Evin
#include "model.hpp"
#include "data/string.hpp"
#include "io/log.hpp"
#include "io/assert.hpp"
#include "memory/memory.hpp"
#include "data/vector_util.hpp"
#include "memory/chunk_allocator.hpp"

#ifdef SFG_TOOLMODE

#include "math/math.hpp"
#include "vendor/nhlohmann/json.hpp"
#include "world/world_resources.hpp"

#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "vendor/stb/stb_image.h"
#include "vendor/stb/stb_image_write.h"
#include "vendor/syoyo/tiny_gltf.h"

#endif

namespace SFG
{
#ifdef SFG_TOOLMODE
	namespace
	{
		matrix4x4 make_mat(const vector<double>& d)
		{
			matrix4x4 mat;
			for (size_t i = 0; i < 16; ++i)
				mat.m[i] = static_cast<float>(d[i]);
			return mat;
		}

		matrix4x3 make_mat43(const vector<double>& d)
		{
			matrix4x3 mat;
			for (size_t i = 0; i < 12; ++i)
				mat.m[i] = static_cast<float>(d[i]);
			return mat;
		}

		auto fill_prim = [](auto&						prim,
							const tinygltf::Model&		model,
							const tinygltf::Primitive&	tprim,
							const tinygltf::Accessor&	vertex_a,
							const tinygltf::BufferView& vertex_bv,
							const tinygltf::Buffer&		vertex_b,
							size_t						num_vertices,
							size_t						start_vertices,
							size_t						start_indices) {
			prim.material_index = static_cast<int16>(tprim.material);

			for (size_t j = 0; j < num_vertices; ++j)
			{
				const size_t stride					  = vertex_bv.byteStride == 0 ? sizeof(float) * 3 : vertex_bv.byteStride;
				const float* rawFloatData			  = reinterpret_cast<const float*>(vertex_b.data.data() + vertex_a.byteOffset + vertex_bv.byteOffset + j * stride);
				prim.vertices[start_vertices + j].pos = vector3(rawFloatData[0], rawFloatData[1], rawFloatData[2]);
			}

			if (tprim.indices != -1)
			{
				const tinygltf::Accessor&	index_a	 = model.accessors[tprim.indices];
				const tinygltf::BufferView& index_bv = model.bufferViews[index_a.bufferView];
				const tinygltf::Buffer&		index_b	 = model.buffers[index_bv.buffer];
				SFG_ASSERT(
					(index_a.type == TINYGLTF_TYPE_SCALAR && (index_a.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT || index_a.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE || index_a.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)),
					"Unsupported component type!");

				if (index_a.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
				{
					SFG_ASSERT(sizeof(primitive_index) >= sizeof(uint16));
				}

				if (index_a.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
				{
					SFG_ASSERT(sizeof(primitive_index) >= sizeof(uint32));
				}

				const size_t num_indices = index_a.count;
				prim.indices.resize(start_indices + num_indices);
				SFG_ASSERT(num_indices % 3 == 0);

				if (index_a.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
				{
					uint8* data = (uint8*)&index_b.data[index_a.byteOffset + index_bv.byteOffset];
					for (uint32 k = 0; k < num_indices; k++)
						prim.indices[start_indices + k] = static_cast<primitive_index>(data[k]);
				}
				else if (index_a.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
				{
					if (sizeof(primitive_index) == sizeof(uint16))
					{
						SFG_MEMCPY(prim.indices.data() + start_indices * sizeof(primitive_index), &index_b.data[index_a.byteOffset + index_bv.byteOffset], num_indices * sizeof(uint16));
					}
					else
					{
						// our indices are 32 bit, need to cast
						uint16* data = (uint16*)&index_b.data[index_a.byteOffset + index_bv.byteOffset];

						for (uint32 k = 0; k < num_indices; k++)
							prim.indices[start_indices + k] = static_cast<uint32>(data[k]);
					}
				}
				else
				{
					SFG_MEMCPY(prim.indices.data() + start_indices * sizeof(primitive_index), &index_b.data[index_a.byteOffset + index_bv.byteOffset], num_indices * sizeof(uint32));
				}
			}

			auto normals_attribute = tprim.attributes.find("NORMAL");
			if (normals_attribute != tprim.attributes.end())
			{
				const tinygltf::Accessor&	normals_a  = model.accessors[normals_attribute->second];
				const tinygltf::BufferView& normals_bv = model.bufferViews[normals_a.bufferView];
				const tinygltf::Buffer&		normals_b  = model.buffers[normals_bv.buffer];
				SFG_ASSERT((normals_a.type == TINYGLTF_TYPE_VEC3 && normals_a.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT), "Unsupported component type!");

				const size_t num_normals = normals_a.count;
				SFG_ASSERT(num_normals == num_vertices);

				for (size_t j = 0; j < num_normals; ++j)
				{
					const size_t stride						 = normals_bv.byteStride == 0 ? sizeof(float) * 3 : normals_bv.byteStride;
					const float* raw_data					 = reinterpret_cast<const float*>(normals_b.data.data() + normals_a.byteOffset + normals_bv.byteOffset + j * stride);
					prim.vertices[start_vertices + j].normal = vector3(raw_data[0], raw_data[1], raw_data[2]);
				}
			}
			else
			{
				SFG_ASSERT(false);
			}

			auto uv_attribute = tprim.attributes.find("TEXCOORD_0");
			if (uv_attribute != tprim.attributes.end())
			{
				const tinygltf::Accessor&	uv_a  = model.accessors[uv_attribute->second];
				const tinygltf::BufferView& uv_bv = model.bufferViews[uv_a.bufferView];
				const tinygltf::Buffer&		uv_b  = model.buffers[uv_bv.buffer];
				SFG_ASSERT((uv_a.type == TINYGLTF_TYPE_VEC2 && uv_a.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT), "Unsupported component type!");

				const size_t num_uv = uv_a.count;
				SFG_ASSERT(num_uv == num_vertices);

				for (size_t j = 0; j < num_uv; ++j)
				{
					const size_t stride					 = uv_bv.byteStride == 0 ? sizeof(float) * 2 : uv_bv.byteStride;
					const float* raw_data				 = reinterpret_cast<const float*>(uv_b.data.data() + uv_a.byteOffset + uv_bv.byteOffset + j * stride);
					prim.vertices[start_vertices + j].uv = vector2(raw_data[0], raw_data[1]);
				}
			}
			else
			{
				SFG_ASSERT(false);
			}

			auto tangents_attribute = tprim.attributes.find("TANGENT");
			if (tangents_attribute != tprim.attributes.end())
			{
				const tinygltf::Accessor&	tangents_a	= model.accessors[tangents_attribute->second];
				const tinygltf::BufferView& targents_bv = model.bufferViews[tangents_a.bufferView];
				const tinygltf::Buffer&		tangents_b	= model.buffers[targents_bv.buffer];
				SFG_ASSERT((tangents_a.type == TINYGLTF_TYPE_VEC4 && tangents_a.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT), "Unsupported component type!");

				const size_t num_tangents = tangents_a.count;
				SFG_ASSERT(num_tangents == num_vertices);

				for (size_t j = 0; j < num_tangents; ++j)
				{
					const size_t stride						  = targents_bv.byteStride == 0 ? sizeof(float) * 4 : targents_bv.byteStride;
					const float* raw_data					  = reinterpret_cast<const float*>(tangents_b.data.data() + tangents_a.byteOffset + targents_bv.byteOffset + j * stride);
					prim.vertices[start_vertices + j].tangent = vector4(raw_data[0], raw_data[1], raw_data[2], raw_data[3]);
				}
			}
			else
			{
				SFG_ASSERT(false);
			}

			/*
			auto colorsAttribute = tprim.attributes.find("COLOR_0");
			if (colorsAttribute != tprim.attributes.end())
			{
				const tinygltf::Accessor&	colorsAccessor	 = model.accessors[colorsAttribute->second];
				const tinygltf::BufferView& colorsBufferView = model.bufferViews[colorsAccessor.bufferView];
				const tinygltf::Buffer&		colorsBuffer	 = model.buffers[colorsBufferView.buffer];
				SFG_ASSERT((colorsAccessor.type == TINYGLTF_TYPE_VEC4 && colorsAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT), "Unsupported component type!");

				const size_t numColors = colorsAccessor.count;
				primitive->colors.resize(numColors);

				for (size_t j = 0; j < numColors; ++j)
				{
					const size_t stride		  = colorsBufferView.byteStride == 0 ? sizeof(float) * 4 : colorsBufferView.byteStride;
					const float* rawFloatData = reinterpret_cast<const float*>(colorsBuffer.data.data() + colorsAccessor.byteOffset + colorsBufferView.byteOffset + j * stride);
					LGXVector4&	 color		  = primitive->colors[j];
					color.x					  = rawFloatData[0];
					color.y					  = rawFloatData[1];
					color.z					  = rawFloatData[2];
					color.w					  = rawFloatData[3];
				}
			}


			*/
		};

	}
	bool model_loaded::create_from_file(const char* file, const char* relative_path)
	{
		tinygltf::Model	   model;
		tinygltf::TinyGLTF loader;
		loader.SetPreserveImageChannels(false);

		string err = "", warn = "";
		bool   ret = loader.LoadASCIIFromFile(&model, &err, &warn, file);

		if (!warn.empty())
		{
			SFG_WARN("{0}", warn);
		}

		if (!err.empty())
		{
			SFG_ERR("{0}", err);
			return false;
		}

		if (!ret)
		{
			SFG_ERR("Loading model failed! {0}", file);
			return false;
		}

		material_count = static_cast<uint8>(model.materials.size());

		const size_t all_meshes_sz = model.meshes.size();
		loaded_meshes.resize(all_meshes_sz);

		for (size_t i = 0; i < all_meshes_sz; i++)
		{
			const tinygltf::Mesh& tmesh = model.meshes[i];
			mesh_loaded&		  mesh	= loaded_meshes[i];
			mesh.name					= tmesh.name;
			const string	hash_path	= string(relative_path) + "/" + mesh.name;
			const string_id hash		= TO_SID(hash_path);
			mesh.sid					= hash;

			for (const tinygltf::Primitive& tprim : tmesh.primitives)
			{
				const tinygltf::Accessor&	vertex_accessor	   = model.accessors[tprim.attributes.find("POSITION")->second];
				const tinygltf::BufferView& vertex_buffer_view = model.bufferViews[vertex_accessor.bufferView];
				const tinygltf::Buffer&		vertex_buffer	   = model.buffers[vertex_buffer_view.buffer];

				SFG_ASSERT((vertex_accessor.type == TINYGLTF_TYPE_VEC3 && vertex_accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT), "Unsupported component type!");
				const size_t num_vertices = vertex_accessor.count;

				const vector3 min_position = {static_cast<float>(vertex_accessor.minValues[0]), static_cast<float>(vertex_accessor.minValues[1]), static_cast<float>(vertex_accessor.minValues[2])};
				const vector3 max_position = {static_cast<float>(vertex_accessor.maxValues[0]), static_cast<float>(vertex_accessor.maxValues[1]), static_cast<float>(vertex_accessor.maxValues[2])};

				total_aabb.bounds_min = vector3::min(total_aabb.bounds_min, min_position);
				total_aabb.bounds_max = vector3::max(total_aabb.bounds_max, max_position);

				auto joints0  = tprim.attributes.find("JOINTS_0");
				auto weights0 = tprim.attributes.find("WEIGHTS_0");

				// if skinned prim, fill joints & weights here & call generic fill_prim.
				if (joints0 != tprim.attributes.end() && weights0 != tprim.attributes.end())
				{
					auto	   it	 = vector_util::find_if(mesh.primitives_skinned, [&tprim](const primitive_skinned_loaded& p) -> bool { return p.material_index == tprim.material; });
					const bool found = it != mesh.primitives_skinned.end();
					if (!found)
						mesh.primitives_skinned.push_back({});

					primitive_skinned_loaded& prim		   = found ? *it : mesh.primitives_skinned.back();
					const size_t			  start_vertex = prim.vertices.size();
					const size_t			  start_index  = prim.indices.size();
					prim.vertices.resize(start_vertex + num_vertices);

					const tinygltf::Accessor&	joints_a   = model.accessors[joints0->second];
					const tinygltf::BufferView& joints_bv  = model.bufferViews[joints_a.bufferView];
					const tinygltf::Buffer&		joints_b   = model.buffers[joints_bv.buffer];
					const tinygltf::Accessor&	weights_a  = model.accessors[weights0->second];
					const tinygltf::BufferView& weights_bv = model.bufferViews[weights_a.bufferView];
					const tinygltf::Buffer&		weights_b  = model.buffers[weights_bv.buffer];

					SFG_ASSERT((joints_a.type == TINYGLTF_TYPE_VEC4 && (joints_a.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT || joints_a.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)), "Unsupported component type!");
					SFG_ASSERT((weights_a.type == TINYGLTF_TYPE_VEC4 && weights_a.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT), "Unsupported component type!");
					SFG_ASSERT(joints_a.count == weights_a.count);
					SFG_ASSERT(joints_a.count == num_vertices);

					const size_t num_joints = joints_a.count;

					for (size_t j = 0; j < num_joints; j++)
					{
						const uint32 vertex_index = start_vertex + j / 4;

						if (joints_a.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
						{
							const size_t  stride					 = joints_bv.byteStride == 0 ? sizeof(uint16) * 4 : joints_bv.byteStride;
							const uint16* rawData					 = reinterpret_cast<const uint16*>(joints_b.data.data() + joints_a.byteOffset + joints_bv.byteOffset + j * stride);
							prim.vertices[vertex_index].bone_indices = vector4i16(rawData[0], rawData[1], rawData[2], rawData[3]);
						}
						else
						{
							const size_t stride						 = joints_bv.byteStride == 0 ? sizeof(uint8) * 4 : joints_bv.byteStride;
							const uint8* rawData					 = reinterpret_cast<const uint8*>(joints_b.data.data() + joints_a.byteOffset + joints_bv.byteOffset + j * stride);
							prim.vertices[vertex_index].bone_indices = vector4i16(rawData[0], rawData[1], rawData[2], rawData[3]);
						}
					}

					const size_t num_weights = weights_a.count;
					for (size_t j = 0; j < num_weights; ++j)
					{
						const uint32 vertex_index				 = start_vertex + j / 4;
						const size_t stride						 = weights_bv.byteStride == 0 ? sizeof(float) * 4 : weights_bv.byteStride;
						const float* rawData					 = reinterpret_cast<const float*>(weights_b.data.data() + weights_a.byteOffset + weights_bv.byteOffset + j * stride);
						prim.vertices[vertex_index].bone_weights = vector4(rawData[0], rawData[1], rawData[2], rawData[3]);
					}

					fill_prim(prim, model, tprim, vertex_accessor, vertex_buffer_view, vertex_buffer, num_vertices, start_vertex, start_index);
					continue;
				}

				auto	   it	 = vector_util::find_if(mesh.primitives_static, [&tprim](const primitive_static_loaded& p) -> bool { return p.material_index == tprim.material; });
				const bool found = it != mesh.primitives_static.end();
				if (!found)
					mesh.primitives_static.push_back({});
				primitive_static_loaded& prim		  = found ? *it : mesh.primitives_static.back();
				const size_t			 start_vertex = prim.vertices.size();
				const size_t			 start_index  = prim.indices.size();
				prim.vertices.resize(start_vertex + num_vertices);
				fill_prim(prim, model, tprim, vertex_accessor, vertex_buffer_view, vertex_buffer, num_vertices, start_vertex, start_index);
			}

			const size_t all_nodes_sz = model.nodes.size();
			loaded_nodes.resize(all_nodes_sz);

			for (size_t i = 0; i < all_nodes_sz; i++)
			{
				const tinygltf::Node& tnode = model.nodes[i];
				model_node_loaded&	  node	= loaded_nodes[i];
				node.name					= tnode.name;
				node.mesh_index				= static_cast<int16>(tnode.mesh);

				if (tnode.matrix.empty())
				{
					const vector3 p	  = tnode.translation.empty() ? vector3::zero : vector3(tnode.translation[0], tnode.translation[1], tnode.translation[2]);
					const vector3 s	  = tnode.scale.empty() ? vector3::one : vector3(tnode.scale[0], tnode.scale[1], tnode.scale[2]);
					const quat	  r	  = tnode.rotation.empty() ? quat::identity : quat(tnode.rotation[0], tnode.rotation[1], tnode.rotation[2], tnode.rotation[3]);
					node.local_matrix = matrix4x3::transform(p, r, s);
				}
				else
				{
					node.local_matrix = make_mat43(tnode.matrix);
				}

				for (int child : tnode.children)
					loaded_nodes[child].parent_index = i;

				if (tnode.mesh != -1)
					loaded_meshes[tnode.mesh].node_index = static_cast<uint16>(i);
			}

			const size_t all_skins_sz = model.skins.size();

			for (size_t i = 0; i < all_skins_sz; i++)
			{
				const tinygltf::Skin& tskin = model.skins[i];

				const string	hash_path = string(relative_path) + "/" + tskin.name;
				const string_id hash	  = TO_SID(hash_path);
				loaded_skins.push_back({});
				skin_loaded& skin = loaded_skins.back();
				skin.sid		  = hash;

				const size_t joints_sz = tskin.joints.size();
				skin.name			   = tskin.name;

				skin.joints.resize(joints_sz);
				skin.root_joint = static_cast<int16>(tskin.skeleton);

				const tinygltf::Accessor&	inverse_bind_a	= model.accessors[tskin.inverseBindMatrices];
				const tinygltf::BufferView& inverse_bind_bv = model.bufferViews[inverse_bind_a.bufferView];
				const tinygltf::Buffer&		inverse_bind_b	= model.buffers[inverse_bind_bv.buffer];
				const size_t				matrix_count	= inverse_bind_a.count;
				SFG_ASSERT((inverse_bind_a.type == TINYGLTF_TYPE_MAT4 && inverse_bind_a.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT), "Unsupported component type!");
				SFG_ASSERT(matrix_count == joints_sz);

				for (size_t j = 0; j < joints_sz; j++)
				{
					const size_t stride		 = inverse_bind_bv.byteStride == 0 ? sizeof(float) * 16 : inverse_bind_bv.byteStride;
					const float* raw		 = reinterpret_cast<const float*>(inverse_bind_b.data.data() + inverse_bind_a.byteOffset + inverse_bind_bv.byteOffset + j * stride);
					const int32	 joint_index = tskin.joints[j];

					skin_joint& sj		= skin.joints[j];
					sj.model_node_index = static_cast<int16>(joint_index);

					for (size_t k = 0; k < 16; k++)
						sj.inverse_bind_matrix.m[k] = raw[k];
				}
			}

			const size_t all_anims_sz = model.animations.size();

			for (size_t i = 0; i < all_anims_sz; i++)
			{
				tinygltf::Animation& tanim	   = model.animations[i];
				const string		 hash_path = string(relative_path) + "/" + tanim.name;
				const string_id		 hash	   = TO_SID(hash_path);
				loaded_animations.push_back({});
				animation_loaded& anim = loaded_animations.back();
				anim.sid			   = hash;
				anim.name			   = tanim.name;

				const size_t channels_sz = tanim.channels.size();

				for (size_t j = 0; j < channels_sz; j++)
				{
					tinygltf::AnimationChannel& tchannel = tanim.channels[j];

					const tinygltf::AnimationSampler sampler   = tanim.samplers[tchannel.sampler];
					const tinygltf::Accessor&		 input_a   = model.accessors[sampler.input];
					const tinygltf::BufferView&		 input_bv  = model.bufferViews[input_a.bufferView];
					const tinygltf::Buffer&			 input_b   = model.buffers[input_bv.buffer];
					const tinygltf::Accessor&		 output_a  = model.accessors[sampler.output];
					const tinygltf::BufferView&		 output_bv = model.bufferViews[output_a.bufferView];
					const tinygltf::Buffer&			 output_b  = model.buffers[output_bv.buffer];

					SFG_ASSERT((input_a.type == TINYGLTF_TYPE_SCALAR && input_a.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT), "Unsupported component type!");
					SFG_ASSERT(output_a.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT, "Unsupported component type!");

					const size_t input_count  = input_a.count;
					const size_t output_count = output_a.count;

					const float*  raw_float_data = reinterpret_cast<const float*>(output_b.data.data() + output_a.byteOffset + output_bv.byteOffset);
					vector<float> keyframe_times = {};
					keyframe_times.resize(input_count);

					for (size_t k = 0; k < input_count; k++)
					{
						const size_t stride = input_bv.byteStride == 0 ? sizeof(float) : input_bv.byteStride;
						const float* raw	= reinterpret_cast<const float*>(input_b.data.data() + input_a.byteOffset + input_bv.byteOffset + j * stride);
						keyframe_times[k]	= raw[0];
						anim.duration		= math::max(anim.duration, raw[0]);
					}

					animation_interpolation interpolation = animation_interpolation::linear;
					if (sampler.interpolation.compare("LINEAR") == 0)
						interpolation = animation_interpolation::linear;
					else if (sampler.interpolation.compare("STEP") == 0)
						interpolation = animation_interpolation::step;
					else if (sampler.interpolation.compare("CUBICSPLINE") == 0)
						interpolation = animation_interpolation::cubic_spline;

					if (interpolation == animation_interpolation::cubic_spline)
					{
						SFG_ASSERT(output_count == input_count * 3, "Output count mismatch for CubicSpline!");
					}
					else
					{
						SFG_ASSERT(input_count == output_count, "Input & output counts do not match!");
					}

					const bool is_translation = tchannel.target_path.compare("translation");
					const bool is_scale		  = tchannel.target_path.compare("scale");

					if (is_translation || is_scale)
					{
						animation_channel_v3_loaded* channel = nullptr;

						if (is_translation)
						{
							anim.position_channels.push_back({});
							channel = &anim.position_channels.back();
						}
						else
						{
							anim.scale_channels.push_back({});
							channel = &anim.scale_channels.back();
						}

						channel->interpolation = interpolation;
						channel->node_index	   = static_cast<int16>(tchannel.target_node);

						if (interpolation == animation_interpolation::cubic_spline)
						{
							for (size_t k = 0; k < input_count; k++)
							{
								size_t base = k * 9;
								channel->keyframes_spline.push_back({});
								animation_keyframe_v3_spline& kf = channel->keyframes_spline.back();
								kf.in_tangent					 = vector3(raw_float_data[base], raw_float_data[base + 1], raw_float_data[base + 2]);
								kf.value						 = vector3(raw_float_data[base + 3], raw_float_data[base + 4], raw_float_data[base + 5]);
								kf.out_tangent					 = vector3(raw_float_data[base + 6], raw_float_data[base + 7], raw_float_data[base + 8]);
								kf.time							 = keyframe_times[k];
							}
						}
						else
						{
							for (size_t k = 0; k < input_count; k++)
							{
								size_t base = k * 3;
								channel->keyframes.push_back({});
								animation_keyframe_v3& kf = channel->keyframes.back();
								kf.value				  = vector3(raw_float_data[base], raw_float_data[base + 1], raw_float_data[base + 2]);
								kf.time					  = keyframe_times[k];
							}
						}
					}
					else if (tchannel.target_path.compare("rotation") == 0)
					{
						anim.rotation_channels.push_back({});
						animation_channel_q_loaded& channel = anim.rotation_channels.back();
						channel.interpolation				= interpolation;
						channel.node_index					= static_cast<int16>(tchannel.target_node);

						if (interpolation == animation_interpolation::cubic_spline)
						{
							for (size_t k = 0; k < input_count; k++)
							{
								size_t base = k * 12;
								channel.keyframes_spline.push_back({});
								animation_keyframe_q_spline kf = channel.keyframes_spline.back();

								kf.in_tangent  = quat(raw_float_data[base], raw_float_data[base + 1], raw_float_data[base + 2], raw_float_data[base + 3]);
								kf.value	   = quat(raw_float_data[base + 4], raw_float_data[base + 5], raw_float_data[base + 6], raw_float_data[base + 7]);
								kf.out_tangent = quat(raw_float_data[base + 8], raw_float_data[base + 9], raw_float_data[base + 10], raw_float_data[base + 11]);
								kf.time		   = keyframe_times[k];
							}
						}
						else
						{
							for (size_t k = 0; k < input_count; k++)
							{
								size_t base = k * 4;
								channel.keyframes.push_back({});
								animation_keyframe_q& kf = channel.keyframes.back();
								kf.value				 = quat(raw_float_data[base], raw_float_data[base + 1], raw_float_data[base + 2], raw_float_data[base + 3]);
								kf.time					 = keyframe_times[k];
							}
						}
					}
					else if (tchannel.target_path.compare("weights") == 0)
					{
					}
				}
			}
		}

		total_aabb.update_half_extents();
		SFG_INFO("Loaded model from file: {0}", file);
		return true;
	}

#endif

	model::~model()
	{
		SFG_ASSERT(!_flags.is_set(model::flags::hw_exists));
	}

	void model::create_from_loaded(const model_loaded& loaded, chunk_allocator32& alloc, world_resources& resources)
	{
		SFG_ASSERT(!_flags.is_set(model::flags::hw_exists));

		_total_aabb		= loaded.total_aabb;
		_material_count = loaded.material_count;

		const uint16 node_count	 = static_cast<uint16>(loaded.loaded_nodes.size());
		const uint16 mesh_count	 = static_cast<uint16>(loaded.loaded_meshes.size());
		const uint16 skins_count = static_cast<uint16>(loaded.loaded_skins.size());
		const uint16 anims_count = static_cast<uint16>(loaded.loaded_animations.size());

		if (node_count != 0)
		{
			_nodes				  = alloc.allocate<model_node>(node_count);
			model_node* ptr_nodes = alloc.get<model_node>(_nodes);

			for (uint16 i = 0; i < node_count; i++)
			{
				const model_node_loaded& loaded_node = loaded.loaded_nodes[i];
				model_node&				 node		 = ptr_nodes[i];
				node.create_from_loaded(loaded_node, alloc);
			}
		}

		if (mesh_count != 0)
		{
			_created_meshes				= alloc.allocate<resource_handle>(mesh_count);
			resource_handle* meshes_ptr = alloc.get<resource_handle>(_created_meshes);

			for (uint16 i = 0; i < mesh_count; i++)
			{
				const mesh_loaded&	  loaded_mesh = loaded.loaded_meshes[i];
				const resource_handle handle	  = resources.create_resource<mesh>(loaded_mesh.sid);
				meshes_ptr[i]					  = handle;

				mesh& m = resources.get_resource<mesh>(handle);
				m.create_from_loaded(loaded_mesh, alloc);
			}
		}

		if (skins_count != 0)
		{
			_created_skins			   = alloc.allocate<resource_handle>(skins_count);
			resource_handle* skins_ptr = alloc.get<resource_handle>(_created_skins);

			for (uint16 i = 0; i < skins_count; i++)
			{
				const skin_loaded&	  loaded_skin = loaded.loaded_skins[i];
				const resource_handle handle	  = resources.create_resource<skin>(loaded_skin.sid);
				skins_ptr[i]					  = handle;
				skin& created					  = resources.get_resource<skin>(handle);
				created.create_from_loaded(loaded_skin, alloc);
			}
		}

		if (anims_count != 0)
		{
			_created_anims			   = alloc.allocate<resource_handle>(anims_count);
			resource_handle* anims_ptr = alloc.get<resource_handle>(_created_anims);

			for (uint16 i = 0; i < anims_count; i++)
			{
				const animation_loaded& loaded_anim = loaded.loaded_animations[i];
				const resource_handle	handle		= resources.create_resource<animation>(loaded_anim.sid);
				anims_ptr[i]						= handle;
				animation& created					= resources.get_resource<animation>(handle);
				created.create_from_loaded(loaded_anim, alloc);
			}
		}

		_skins_count  = skins_count;
		_anims_count  = anims_count;
		_meshes_count = mesh_count;
		_nodes_count  = node_count;
		_flags.set(model::flags::pending_upload | model::flags::hw_exists);
	}

	void model::destroy(world_resources& resources, chunk_allocator32& alloc)
	{
		SFG_ASSERT(_flags.is_set(model::flags::hw_exists));

		if (_nodes.size != 0)
		{
			model_node* ptr_nodes = alloc.get<model_node>(_nodes);

			for (uint16 i = 0; i < _nodes_count; i++)
			{
				model_node& node = ptr_nodes[i];
				node.destroy(alloc);
			}

			alloc.free(_nodes);
		}

		if (_created_meshes.size != 0)
		{
			resource_handle* ptr = alloc.get<resource_handle>(_created_meshes);
			for (uint16 i = 0; i < _meshes_count; i++)
			{
				const resource_handle handle = ptr[i];
				mesh&				  m		 = resources.get_resource<mesh>(handle);
				m.destroy(alloc);
				resources.destroy_resource<mesh>(handle);
			}

			alloc.free(_created_meshes);
		}

		if (_created_skins.size != 0)
		{
			resource_handle* skins_ptr = alloc.get<resource_handle>(_created_skins);

			for (uint16 i = 0; i < _skins_count; i++)
			{
				const resource_handle handle = skins_ptr[i];
				skin&				  sk	 = resources.get_resource<skin>(handle);
				sk.destroy(alloc);
				resources.destroy_resource<skin>(handle);
			}

			alloc.free(_created_skins);
		}

		if (_created_anims.size != 0)
		{
			resource_handle* anims_ptr = alloc.get<resource_handle>(_created_anims);

			for (uint16 i = 0; i < _anims_count; i++)
			{
				const resource_handle handle = anims_ptr[i];
				animation&			  anim	 = resources.get_resource<animation>(handle);
				anim.destroy(alloc);
				resources.destroy_resource<animation>(handle);
			}

			alloc.free(_created_anims);
		}

		_created_meshes = {};
		_nodes			= {};
		_created_skins	= {};
		_created_anims	= {};
		_flags.remove(model::flags::hw_exists);
	}
}