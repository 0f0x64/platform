#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>
#include <../lib/utils.shader>

cbuffer params : register(b0)
{
    int gX;
    int gY;
    float4 pos1;
    float4 pos2;
}

float toRad(float a)
{
    return a*PI/180.;
}

float quantize2(float x, float q)
{
    return floor(x*q)/q;
}

pos_color CalcParticles(uint qid, uint iid, float4 grid)
{
    pos_color p;
    p.color = float4(1, 0, 0, 1);
    p.sz = 2;

    float t = grid.z;
    float side = grid.w * 2.0 - 1.0;

    float3 worldA = pos1.xyz;
    float3 worldB = pos2.xyz;
    float3 worldPos = lerp(worldA, worldB, t);
    float3 dirWorld = worldB - worldA;

    float3 dirView = mul(float4(dirWorld, 0.0), view[0]).xyz;

    float2 dirXY = dirView.xy;
    float2 perpXY = (dot(dirXY, dirXY) > 1e-8)
        ? normalize(float2(-dirXY.y, dirXY.x))
        : float2(1.0, 0.0);

    // Обратно в мир: только вращательная часть view, транспонированная
    float3x3 viewRotInv = transpose((float3x3)view[0]);
    float3 perpWorld = mul(float3(perpXY, 0.0), viewRotInv);

    float halfWidth = 0.05;
    float3 worldOffset = worldPos + perpWorld * halfWidth * side;

    p.pos = transform_unisize(worldOffset, float2(0.5, 0.5), 1);
    return p;
}

#include <../lib/particleVS_main2.shader>
