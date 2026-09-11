#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/utils.shader>

float4 PS(VS_OUTPUT_PARTICLE input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    return input.color;
}
