#include<../lib/constBuf.shader>
#include<../lib/io.shader>
cbuffer params:register(b3)
{
float4 tone;
}
VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 m[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=m[V];float4 e=float4(b/4,0,1);e.xyz=lerp(e.xyz,e.xzy,tone.x);e=mul(e,view[0]);e=mul(e,proj[0]);f.pos=e;f.uv=b/2.+.5;return f;}