// Copyright (c) 2025 Inan Evin
#pragma once
#include "gui/vekt.hpp"

namespace vekt
{
	class builder;
}

namespace Game
{
	class vector2ui;
	class render_pass;

	class gui_renderer
	{
	public:
		void init();
		void uninit();
		void render(const vector2ui& size);

	private:
		void on_draw(const vekt::draw_buffer& buffer);

	private:
		vekt::builder _vekt_builder = {};
	};
}