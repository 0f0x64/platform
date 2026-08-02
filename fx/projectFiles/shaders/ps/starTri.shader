#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/utils.shader>

cbuffer params : register(b0)
{
    float hilight;
};

#define PI 3.1415926535897932384626433832795

float3 GetWorldSpaceCameraPos(float4x4 viewMatrix)
{
    float3x3 rot = float3x3(
        viewMatrix._11, viewMatrix._21, viewMatrix._31,
        viewMatrix._12, viewMatrix._22, viewMatrix._32,
        viewMatrix._13, viewMatrix._23, viewMatrix._33
    );
    
    float3 trans = float3(viewMatrix._41, viewMatrix._42, viewMatrix._43);
    return -mul(trans, rot);
}

float3 GetWorldViewDir(float3 worldPos, float3 cameraPos)
{
    return normalize(worldPos - cameraPos);
}

float CalculateFresnelFade(float3 localPos, float3 worldViewDir, float power)
{
    float3 normal = normalize(localPos);
    float fresnel = saturate(abs(dot(normal, worldViewDir)));
    return pow(fresnel, power);
}

float4 PS(VS_OUTPUT_PARTICLE2 input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    float3 c = (noise(input.wpos*2)+1.);
    
    c*=float3(11,2,.5)*1;

float3 cameraPos   = GetWorldSpaceCameraPos(view[0]);
float3 worldViewDir = GetWorldViewDir(input.wpos, cameraPos);
float edgeFade  = CalculateFresnelFade(input.wpos, worldViewDir, 3.);

 c*=edgeFade;

    //if (c.r*edgeFade<.05) discard;
    return float4(c,saturate(edgeFade) );

}
