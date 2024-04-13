#include <../lib/constBuf.shader>
#include <../lib/io.shader>

TextureCube env : register(t0);
Texture2D normals : register(t1);
TextureCube albedo : register(t2);

SamplerState sam1 : register(s0);

float4 PS(VS_OUTPUT input) : SV_Target
{
    float3 nrml = normals.SampleLevel(sam1, input.uv,1).xyz;
    //nrml = -normalize(input.vpos.xyz);
    //nrml = mul(float4(nrml,1), view[0]);
    //nrml = normalize(nrml);
    float3 eye = (input.vpos.xyz);
    eye = mul((view[0]), float4(eye, 1));
    eye = normalize(eye);
    float3 ref = reflect(eye,nrml);

    //ref = -nrml.xyz;
    //ref = normalize(input.wpos);
    //ref.y*= - 1;
    //ref.x *= -1;
    //ref.z *= -1;
    

    float4 color = env.SampleLevel(sam1, ref, 0);
    //color = float4(nrml.xyz, 1);
    return color;
}