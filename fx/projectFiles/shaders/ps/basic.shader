#include <../lib/constBuf.shader>
#include <../lib/io.shader>

TextureCube env : register(t0);
Texture2D normals : register(t1);
TextureCube albedo : register(t2);

SamplerState sam1 : register(s0);

//[
cbuffer params : register(b1)
{
    float r,g,b,a;
};
//]

float3 FresnelSchlick(float3 F0, float3 v, float3 n)
{
    float cosTheta = dot(-n, v);
    return saturate(F0 + (1.0 - F0) * pow(1.0 - saturate(cosTheta), 5.0));
}


float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.uv;
    float3 albedo = float3(1, .5, .3);
    float roughness = 0;
    float metalness = 1;
    float3 F0 = .04;
    
    float3 nrml = normals.SampleLevel(sam1, input.uv,1).xyz;
    float3 eye = input.vpos.xyz;
    eye = normalize(mul((view[0]), float4(eye, 1)));
    eye = normalize(eye);
    float3 ref = reflect(eye,nrml);
    float3 specR = env.SampleLevel(sam1, ref, roughness * 10)*2;
    float3 diffR = env.SampleLevel(sam1, -nrml, 8.5)*2;
    
    float3 albedo_ = lerp(albedo, 0, metalness);
    F0 = lerp(F0, F0 * albedo, metalness);
    float3 F = FresnelSchlick(F0, eye, -nrml);
    F *= lerp(saturate(1 - roughness), 1, metalness);
    float3 specK = specR * F;
    float3 diffK = saturate(1.0 - F);
    diffK = albedo * diffK * diffR;

    float3 color = diffK + specK;
    float3 screenN = mul(nrml, view[0]).xyz;
    

    //color *= saturate(1-2 * pow(dot(normalize(mul(eye, view[0])), normalize(mul(nrml, view[0]))), .5));// * saturate(sin(eye.x * 513 + eye.y * 521 + eye.z * 432 + time.x) * 4);
    //float3 nt = mul(input.vnorm.xyz, view[0]);
    float3 nt = mul(nrml, view[0]);
    float d = dot(normalize(input.vpos.xyz), normalize(nt));
    color += saturate(10 - 35 * d)* sin(d * 127 + time.x) * 4 * hilight;
    
    return float4(color, 1);
}