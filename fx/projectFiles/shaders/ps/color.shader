#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/utils.shader>

cbuffer params : register(b0)
{
    float4 color;
};

float4 PS(VS_OUTPUT_PARTICLE input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    return color;
}
