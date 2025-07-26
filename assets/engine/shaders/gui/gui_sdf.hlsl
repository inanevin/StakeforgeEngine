#include "../layout_defines.hlsl"

//------------------------------------------------------------------------------
// In & Outs
//------------------------------------------------------------------------------

struct VSInput
{
	float2 pos : POSITION; // XY position
	float2 uv : TEXCOORD0; // UV coords (unused in PS here, but available)
	float4 color : COLOR0; // RGBA vertex color
};


struct VSOutput
{
	float4 pos : SV_POSITION; // transformed position
	float2 uv : TEXCOORD0; // pass-through UV
	float4 color : COLOR0; // pass-through color
};

//------------------------------------------------------------------------------
// Vertex Shader
//------------------------------------------------------------------------------

cbuffer RenderPassCBV : render_pass_ubo0
{
	float4x4 _rp_projection;
	float _sdf_thickness;
	float _sdf_softness;
}

VSOutput VSMain(VSInput IN)
{
	VSOutput OUT;
    
	float4 worldPos = float4(IN.pos, 0.0f, 1.0f);
	OUT.pos = mul(_rp_projection, worldPos);
	OUT.uv = IN.uv;
	OUT.color = IN.color;
	return OUT;
}

//------------------------------------------------------------------------------
// Pixel Shader
//------------------------------------------------------------------------------

Texture2D _txt_atlas : material_texture0;
SamplerState _smp_base : static_sampler_gui_text;

float4 PSMain(VSOutput IN) : SV_TARGET
{
	float distance = _txt_atlas.SampleLevel(_smp_base, IN.uv, 0).x;
	float alpha = smoothstep(_sdf_thickness - _sdf_softness, _sdf_thickness + _sdf_softness, distance);
	return float4(IN.color.xyz, alpha);
}