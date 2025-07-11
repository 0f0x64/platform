#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/utils.shader>


TextureCube env : register(t0);
Texture2D normals : register(t1);
TextureCube albedo : register(t2);

SamplerState sam1 : register(s0);


cbuffer params : register(b0)
{
    float hilight;
};


float3 FresnelSchlick(float3 F0, float3 v, float3 n)
{
    float cosTheta = dot(-n, v);
    return saturate(F0 + (1.0 - F0) * pow(1.0 - saturate(cosTheta), 5.0));
}

float3 rotY(float3 pos, float a)
{
    float3x3 m =
    {
        cos(a), 0, sin(a),
        0, 1, 0,
        -sin(a), 0, cos(a)
    };
    pos = mul(pos, m);
    return pos;
}

#define PI 3.1415926535897932384626433832795

float4 PS(VS_OUTPUT input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    float2 uv = input.uv;
    float2 grd = (sin(256*(uv))-.5)*2;
    //return pow(saturate((max(grd.x,grd.y))),5);;
    float3 albedo = float3(1, 1, 1);
    albedo = sin(sin(uv.x*8*PI)*2+uv.y*92);
    albedo=lerp(albedo, sin(uv.y*222),smoothstep(0,1,1-uv.y));
    albedo=saturate(albedo)*float3(1,.5,.2);


    //return float4(albedo,1);
    float roughness = .85;
    float metalness = 0.;
    float3 F0 = .04;
    
    float3 nrml = normals.SampleLevel(sam1, input.uv,7).xyz;
    nrml += normals.SampleLevel(sam1, input.uv,1).xyz*.02;
    nrml=normalize(nrml);
    //nrml=rotY(nrml,time.x*.1);
    //nrml *= -(1 - isFrontFace * 2);
    float3 eye = input.vpos.xyz;
    eye = normalize(mul((view[0]), float4(eye, 1)));
    eye = normalize(eye);
    float3 ref = reflect(eye,nrml);
    float3 specR = env.SampleLevel(sam1, ref, roughness * 10);
    float3 diffR = env.SampleLevel(sam1, -nrml, 9.5)/3;
    
    float3 albedo_ = lerp(albedo, 0, metalness);
    F0 = lerp(F0, F0 * albedo, metalness);
    float3 F = FresnelSchlick(F0, eye, -nrml);
    F *= lerp(saturate(1 - roughness), 1, metalness);
    float3 specK = specR * F;
    float3 diffK = saturate(1.0 - F);
    diffK = albedo * diffK * diffR;

    float3 color = diffK + specK;
    float3 screenN = mul(nrml, view[0]).xyz;

   // color = 0; 
    
    //color *= saturate(1-2 * pow(dot(normalize(mul(eye, view[0])), normalize(mul(nrml, view[0]))), .5));// * saturate(sin(eye.x * 513 + eye.y * 521 + eye.z * 432 + time.x) * 4);
    //float3 nt = mul(input.vnorm.xyz, view[0]);
    float3 nt = mul(nrml, view[0]);
    float d = dot(normalize(input.vpos.xyz), normalize(nt));
    color += saturate(10 - 35 * d)* sin(d * 127 + time.x) * 4 * hilight;
    
    color.rgb = pow(ACESFilm(color.rgb),1/2.2);
    //return 1;
    return float4(color, 1);
}
