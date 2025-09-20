#include "../../../engine/shaders/layout_defines.hlsl"

//------------------------------------------------------------------------------
// In & Outs
//------------------------------------------------------------------------------

struct vs_input
{
	float3 pos : POSITION;
	float3 normal : NORMAL0;
	float4 tangent : TANGENT0;
	float2 uv : TEXCOORD0;

#ifdef USE_SKINNING
    float4 bone_weights : BLENDWEIGHT0;
    uint4  bone_indices : BLENDINDICES0;
#endif
};

struct vs_output
{
	float4 pos : SV_POSITION;
	float3 world_pos : TEXCOORD0;
	float3 world_norm : TEXCOORD1;
	float3 world_tan : TEXCOORD2;
	float3 world_bit : TEXCOORD3;
	float2 uv : TEXCOORD4;
};

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------

cbuffer constant_data : constants_cbv
{
	uint entity_index;
	uint pad0;
	uint pad1;
	uint pad2;
};

struct gpu_entity
{
	float3x4 model;
	float3x4 normal_matrix;
};

StructuredBuffer<gpu_entity> entity_buffer : render_pass_ssbo0;

#ifdef USE_SKINNING
struct gpu_bone
{
    float3x4 bone;
};
StructuredBuffer<gpu_bone> bone_buffer : render_pass_ssbo1;
#endif

cbuffer render_pass : render_pass_ubo0
{
	float4x4 view;
	float4x4 proj;
	float4x4 view_proj;
};

//------------------------------------------------------------------------------
// Material
//------------------------------------------------------------------------------

cbuffer material_data : material_ubo0
{
	float4 base_color_factor;
	float3 emissive_factor;
	float metallic_factor;
	float roughness_factor;
	float normal_scale;
	float occlusion_strength;
	float2 pad;
};

Texture2D tex_albedo : material_texture0;
Texture2D tex_normal : material_texture1;
Texture2D tex_orm : material_texture2;
Texture2D tex_emissive : material_texture3;

SamplerState sampler_default : static_sampler_anisotropic;

//------------------------------------------------------------------------------
// Vertex Shader
//------------------------------------------------------------------------------

vs_output vs_main(vs_input IN)
{
	vs_output OUT;
	gpu_entity entity = entity_buffer[entity_index];

	float4 obj_pos;
	float3 obj_norm;
	float3 obj_tan;

#ifdef USE_SKINNING
    // skinning in object space
    float4 skinned_pos    = float4(0, 0, 0, 0);
    float3 skinned_normal = 0;
    float3 skinned_tan    = 0;

    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        uint bone_index   = IN.bone_indices[i];
        float weight      = IN.bone_weights[i];
        float3x4 bone_mat = bone_buffer[bone_index].bone;

        skinned_pos    += mul(float4(IN.pos, 1.0f), bone_mat) * weight;
        skinned_normal += mul(IN.normal, (float3x3)bone_mat) * weight;
        skinned_tan    += mul(IN.tangent.xyz, (float3x3)bone_mat) * weight;
    }

    obj_pos  = skinned_pos;
    obj_norm = skinned_normal;
    obj_tan  = skinned_tan;
#else
    // rigid mesh, object-space values directly
	obj_pos = float4(IN.pos, 1.0f);
	obj_norm = IN.normal;
	obj_tan = IN.tangent.xyz;
#endif

    // world-space transforms
	float3 world_pos = mul(obj_pos, entity.model);
	float3 world_norm = normalize(mul(obj_norm, (float3x3) entity.normal_matrix));
	float3 world_tan = normalize(mul(obj_tan, (float3x3) entity.model));
	float3 world_bit = normalize(cross(world_norm, world_tan) * IN.tangent.w);

	OUT.pos = mul(view_proj, float4(world_pos, 1.0f));
	OUT.world_pos = world_pos;
	OUT.world_norm = world_norm;
	OUT.world_tan = world_tan;
	OUT.world_bit = world_bit;
	OUT.uv = IN.uv;

	return OUT;
}

//------------------------------------------------------------------------------
// Pixel Shader (G-Buffer)
//------------------------------------------------------------------------------

struct ps_output
{
	float4 rt0 : SV_Target0; // Albedo
	float4 rt1 : SV_Target1; // World Normal
	float4 rt2 : SV_Target2; // orm
	float4 rt3 : SV_Target3; // Emissive
};

ps_output ps_main(vs_output IN)
{
	ps_output OUT;

    // --- Base color ---
	float4 albedo_tex = tex_albedo.Sample(sampler_default, IN.uv);
	float4 albedo = albedo_tex * base_color_factor;

    // --- Normal map ---
	float3 tangent_normal = tex_normal.Sample(sampler_default, IN.uv).xyz * 2.0 - 1.0;
	tangent_normal.xy *= normal_scale;
	tangent_normal = normalize(tangent_normal);

	float3x3 TBN = float3x3(IN.world_tan, IN.world_bit, IN.world_norm);
	float3 world_normal = normalize(mul(TBN, tangent_normal));

    // --- orm ---
	float3 orm_tex = tex_orm.Sample(sampler_default, IN.uv).rgb;
	float ao = saturate(orm_tex.r * occlusion_strength);
	float roughness = saturate(orm_tex.g * roughness_factor);
	float metallic = saturate(orm_tex.b * metallic_factor);

    // --- Emissive ---
	float3 emissive_tex = tex_emissive.Sample(sampler_default, IN.uv).rgb;
	float3 emissive = emissive_tex * emissive_factor;

    // --- G-buffer outputs ---
	OUT.rt0 = albedo;
	OUT.rt1 = float4(world_normal * 0.5 + 0.5, 1.0);
	OUT.rt2 = float4(roughness, metallic, ao, 1.0);
	OUT.rt3 = float4(emissive, 1.0);

	return OUT;
}
