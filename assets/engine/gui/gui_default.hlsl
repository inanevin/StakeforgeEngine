//------------------------------------------------------------------------------
// Constant buffer (b0) with a single 4x4 projection matrix
//------------------------------------------------------------------------------
cbuffer ProjectionCB : register(b0)
{
    float4x4 uProjection; // projection matrix
}

//------------------------------------------------------------------------------
// Static sampler (s0)
//------------------------------------------------------------------------------
SamplerState g_Sampler : register(s0); 

//------------------------------------------------------------------------------
// Vertex input structure
//------------------------------------------------------------------------------
struct VSInput
{
    float2 pos   : POSITION;   // XY position
    float2 uv    : TEXCOORD0;  // UV coords (unused in PS here, but available)
    float4 color : COLOR0;     // RGBA vertex color
};

//------------------------------------------------------------------------------
// Vertex-to-pixel payload
//------------------------------------------------------------------------------
struct VSOutput
{
    float4 pos   : SV_POSITION; // transformed position
    float2 uv    : TEXCOORD0;   // pass-through UV
    float4 color : COLOR0;      // pass-through color
};

//------------------------------------------------------------------------------
// Vertex Shader
//------------------------------------------------------------------------------
VSOutput VSMain(VSInput IN)
{
    VSOutput OUT;
    
    // Transform 2D pos into clip space.
    float4 worldPos = float4(IN.pos, 0.0f, 1.0f);
    OUT.pos = mul(uProjection, worldPos);

    OUT.uv    = IN.uv;
    OUT.color = IN.color;
    return OUT;
}

//------------------------------------------------------------------------------
// Pixel Shader: just output the interpolated vertex color
//------------------------------------------------------------------------------
float4 PSMain(VSOutput IN) : SV_TARGET
{
    return IN.color;
}