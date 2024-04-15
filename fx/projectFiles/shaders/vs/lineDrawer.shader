#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>

//[
cbuffer params : register(b0)
{
    float x1,y1,x2,y2;
}

cbuffer geo : register(b6)
{
    float4 position[256];
}

//]


VS_OUTPUT_POSONLY VS(uint vID : SV_VertexID)
{
    VS_OUTPUT_POSONLY output = (VS_OUTPUT_POSONLY) 0;
    output.pos = position[vID];

    return output;
}
