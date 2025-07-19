// Copyright (c) 2025 Inan Evin
#include "mesh_manager.hpp"
#include "common/linagx_incl.hpp"
#include "gfx/common/gfx_common.hpp"

namespace Game
{
	void mesh_manager::init()
	{
		_buffer_vtx.create(LinaGX::ResourceTypeHint::TH_VertexBuffer, sizeof(vertex) * 1000, "Big Vertex Buffer");
		_buffer_idx.create(LinaGX::ResourceTypeHint::TH_VertexBuffer, sizeof(index) * 3000, "Big Vertex Buffer");
	}

	void mesh_manager::uninit()
	{
		_buffer_vtx.destroy();
		_buffer_idx.destroy();
	}
}