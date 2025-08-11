// Copyright (c) 2025 Inan Evin
#pragma once

#include "gfx/buffer.hpp"
#include "data/vector.hpp"
#include "gfx/common/gfx_common.hpp"

namespace SFG
{
	class mesh_manager
	{
	public:
		void init();
		void uninit();

	private:
		buffer				_buffer_vtx = {};
		buffer				_buffer_idx = {};
		vector<mesh_static> _meshes_static;
	};
}