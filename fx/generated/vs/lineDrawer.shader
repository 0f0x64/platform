#include<../../lib/constBuf.shader>
#include<../../lib/io.shader>
#include<../../lib/constants.shader>
cbuffer params:register(b0)
{
float4 position[4000];
}
VS_OUTPUT_POS_UV VS(uint p:SV_VertexID){VS_OUTPUT_POS_UV s=(VS_OUTPUT_POS_UV)0;s.pos.xy=position[p].xy*float2(2,-2)+float2(-1,1);s.pos.z=0;s.pos.w=1;s.uv=0.;s.uv=1;return s;}