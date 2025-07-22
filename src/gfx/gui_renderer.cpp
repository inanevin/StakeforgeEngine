// Copyright (c) 2025 Inan Evin

#include "gui_renderer.hpp"
#include "math/vector2ui.hpp"

#define VEKT_IMPL
#include "gui/vekt.hpp"

namespace Game
{
	void gui_renderer::init()
	{
		_vekt_builder.init({});
		_vekt_builder.set_on_draw([this](const vekt::draw_buffer& buffer) { on_draw(buffer); });
	}

	void gui_renderer::render(const vector2ui& size)
	{
		_vekt_builder.build(vekt::vec2(size.x, size.y));
		_vekt_builder.flush();
	}

	void gui_renderer::on_draw(const vekt::draw_buffer& buffer)
	{
		// add draw commands to render_pass
		// bind vertex and index buffers.
		// transfer those buffers.
	}

}