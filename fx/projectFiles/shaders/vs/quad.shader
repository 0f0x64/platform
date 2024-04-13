#include <../lib/constBuf.shader>
#include <../lib/io.shader>

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    float2 quad[6] = { -1, -1, 1, -1, -1, 1, 1, -1, 1, 1, -1, 1 };
    float2 p = quad[vID];
    output.pos = float4(p, 0, 1);
    output.uv = float2(1,-1)* p / 2. + .5;
    return output;
}
