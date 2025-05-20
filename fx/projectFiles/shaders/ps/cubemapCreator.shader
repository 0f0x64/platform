#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>

struct PS_OUTPUT
{
    float4 c0 : SV_Target0;
    float4 c1 : SV_Target1;
    float4 c2 : SV_Target2;
    float4 c3 : SV_Target3;
    float4 c4 : SV_Target4;
    float4 c5 : SV_Target5;
    
};

static const float3 offsetV[6] = { float3(1, 1, 1), float3(-1, 1, -1), float3(-1, 1, -1), float3(-1, -1, 1), float3(-1, 1, 1), float3(1, 1, -1) };
static const float3 offsetX[6] = { float3(0, 0, -2), float3(0, 0, 2), float3(2, 0, 0), float3(2, 0, 0), float3(2, 0, 0), float3(-2, 0, 0) };
static const float3 offsetY[6] = { float3(0, -2, 0), float3(0, -2, 0), float3(0, 0, 2), float3(0, 0, -2), float3(0, -2, 0), float3(0, -2, 0) };

float3 ConvertUV(float2 UV, int FaceIndex)
{
    float3 outV = offsetV[FaceIndex] + offsetX[FaceIndex] * UV.x + offsetY[FaceIndex] * UV.y;
    return normalize(outV);
}

float calcWalls(float2 uv)
{
    float walls = 0;
    float2 uv5 = float2(0, 0);
    for (int i = 0; i < 5; i++)
    {
  
        float2 xy = uv - .5 - .15 * float2(sin(i * 4. + time.x * .01), sin(i * 5. + time.x * .02));
        float2 uv4 = float2(atan2(xy.y, xy.x) * 1, 12 / (length(xy) + .1) + time.y * .1);
        walls += pow(saturate(1 - 2 * length(xy)), 62) * 120;
        uv5 += uv4;
        
    }
    walls += pow(saturate(1 - 1 * (abs(sin(uv5.x)) * abs(sin(uv5.y / 13)))), 5) * 2;
    walls = walls * saturate(1 - 2 * length(uv - .5));
    return walls;
}

float4 env(float3 v)
{
    float a = .9*saturate(1244*sin((v.z / v.y) * 6) * sin((v.x / v.y) * 6)) ;
    float blend = saturate(8 - pow(length(v.xz / v.y), .7));
    
    float va = atan2(v.z, v.x);
    float x = frac(va / PI / 2 * 64);
    float y = frac((v.y) *10.+.5);
    
    float b = saturate(1 - 2 * length(float2(x, y) - .5));
    
    a = lerp(a, b, 1-blend);
    a = pow(a, 24)*10;
    a *= saturate(-v.y);
    a += saturate(1 - 2 * length(v.xz)) * saturate(v.y)*44;
    return float4(a, a, a, 1);
}

PS_OUTPUT PS(VS_OUTPUT input) 
{

    float2 uv = input.uv;
    float4 color = 1;//    float4(uv, 0, 1);

    PS_OUTPUT cube;
    
    cube.c0 = env(ConvertUV(uv, 0));
    cube.c1 = env(ConvertUV(uv, 1));
    cube.c2 = env(ConvertUV(uv, 2));
    cube.c3 = env(ConvertUV(uv, 3));
    cube.c4 = env(ConvertUV(uv, 4));
    cube.c5 = env(ConvertUV(uv, 5));

    return cube;
}