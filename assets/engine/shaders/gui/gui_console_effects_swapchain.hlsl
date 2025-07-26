//------------------------------------------------------------------------------
// In & Outs
//------------------------------------------------------------------------------

struct VSOutput
{
	float4 pos : SV_POSITION; // transformed position
	float2 uv : TEXCOORD0; // pass-through UV
};

//------------------------------------------------------------------------------
// Vertex Shader
//------------------------------------------------------------------------------

static const float2 NDC_POS[6] =
{
	float2(-1.0f, 1.0f), 
    float2(1.0f, 1.0f), 
	 float2(-1.0f, 0.0f),
	 float2(1.0f, 1.0f),
    float2(1.0f, 0.0f),
    float2(-1.0f, 0.0f) 
};

static const float2 NDC_UV[6] =
{
	float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
	 float2(0.0f, 1.0f),
	 float2(1.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(0.0f, 1.0f)
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
	VSOutput OUT;
	OUT.pos = float4(NDC_POS[vertexID], 0.0f, 1.0f);
	OUT.uv = NDC_UV[vertexID];
	return OUT;
}

cbuffer EngineCBV : register(b0, space0)
{
	float _time;
	float _pad;
}

Texture2D _txt_source: register(t4, space1);
SamplerState _smp_base : register(s6);

//------------------------------------------------------------------------------
// Pixel Shader: just output the interpolated vertex color
//------------------------------------------------------------------------------
float4 PSMain(VSOutput IN) : SV_TARGET
{
	float4 color = _txt_source.SampleLevel(_smp_base, IN.uv, 0);
	return color;

}