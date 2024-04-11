#include <../lib/constBuf.shader>
#include <../lib/io.shader>

//[
cbuffer params : register(b0)
{
    float  tone,    x1,x2;
}
//]

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    float2 quad[6] = { -1, -1, 1, -1, -1, 1, 1, -1, 1, 1, -1, 1 };
    float2 p = quad[vID];
    float4 p2 = float4(p/4, 0, 1);
    
    p2.xyz = lerp(p2.xyz, p2.xzy, tone);
        
    p2 = mul(p2, view[0]);
    p2 = mul(p2, proj[0]);  
    
    output.pos = p2;
    
    output.uv = p / 2. + .5;
    return output;
}
