#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/utils.shader>


TextureCube env : register(t0);
SamplerState sam1 : register(s0);


float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 color = env.SampleLevel(sam1, float4(normalize(input.wpos.xyz),0), 0);
    
    color.rgb = pow(ACESFilm(color.rgb ), 1 / 2.2);
    return color;
}