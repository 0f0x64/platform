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

pos_color CalcParticles(uint qid,uint iid,float4 grid)
{
    pos_color p;
    p.color = float4(1, 0, 0, 1); // статичный красный
    p.sz = 2;

    // grid.z — торец (0 у pos1, 1 у pos2), grid.w — сторона (0/1)
    float t = grid.z;
    float side = grid.w * 2.0 - 1.0;

    // Концы отрезка в мире
    float3 worldA = pos1.xyz;
    float3 worldB = pos2.xyz;

    // Центр текущего торца
    float3 worldPos = lerp(worldA, worldB, t);

    // Направление отрезка в мире
    float3 dirWorld = worldB - worldA;

    // Переводим в view-space
    float3 posView = mul(float4(worldPos, 1.0), view[0]).xyz;
    float3 dirView = mul(float4(dirWorld, 0.0), view[0]).xyz;

    // Перпендикуляр к направлению в плоскости XY view-space
    float2 dirXY = dirView.xy;
    float2 perpXY = (dot(dirXY, dirXY) > 1e-8)
        ? normalize(float2(-dirXY.y, dirXY.x))
        : float2(1.0, 0.0);

    // Полуширина линии в view-space (мировые единицы)
    float halfWidth = 0.05;

    posView.xy += perpXY * halfWidth * side;

    // Проекция
    p.pos = mul(float4(posView, 1.0), proj[0]);

    return p;
}

#include <../lib/particleVS_main2.shader>
