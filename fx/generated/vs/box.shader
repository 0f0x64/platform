#include<../../lib/constBuf.shader>
#include<../../lib/io.shader>
#include<../../lib/constants.shader>
#include<../../lib/utils.shader>
cbuffer params:register(b0)
{
float4 pos_size[256];
}
VS_OUTPUT_POS_UV VS(uint p:SV_VertexID,uint V:SV_InstanceID){VS_OUTPUT_POS_UV f;float2 e[6]={0,0,1,0,0,1,1,0,1,1,0,1},u=(e[p]-float2(0,1))*pos_size[V].zw*2.+pos_size[V].xy*float2(2,-2)-float2(1,-1);f.pos=float4(u,0,1);f.uv=e[p];f.uv.y=1-f.uv.y;f.sz=pos_size[V].zw;return f;}