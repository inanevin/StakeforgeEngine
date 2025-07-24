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

cbuffer ProjectionCB : register(b0)
{
	float4x4 uProjection; // projection matrix
}

VSOutput VSMain(VSInput IN)
{
	VSOutput OUT;
    
    // Transform 2D pos into clip space.
	float4 worldPos = float4(IN.pos, 0.0f, 1.0f);
	OUT.pos = mul(uProjection, worldPos);
	OUT.uv = IN.uv;
	OUT.color = IN.color;
	return OUT;
}

float2 SDF_Constants : register(b0);
Texture2D g_atlas : register(t0);
SamplerState g_Sampler : register(s0);

//------------------------------------------------------------------------------
// Pixel Shader: just output the interpolated vertex color
//------------------------------------------------------------------------------
float4 PSMain(VSOutput IN) : SV_TARGET
{
	float4 tex_color = g_atlas.SampleLevel(g_Sampler, IN.uv, 0);
	return float4(IN.color.xyz, tex_color.r * IN.color.w);
}