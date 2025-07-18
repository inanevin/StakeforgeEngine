#vertex

#version 460

layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec4 in_col;

layout (location = 0) out vec4 out_col;
layout (location = 1) out vec2 out_uv;

#include "assets/shaders/global_data.glsl"
#include "assets/shaders/renderpasses/renderpass_debug_gui.glsl"

void main()
{
   out_col = in_col;
   out_uv = in_uv;
   gl_Position = render_pass.proj * vec4(in_pos.x, in_pos.y, 0.0, 1.0);
}

#end

#frag

layout (location = 0) in vec4 in_col;
layout (location = 1) in vec2 in_uv;

layout (location = 0) out vec4 frag_col;

void main()
{
   frag_col = in_col;
}

#end
