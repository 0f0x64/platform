#include<../lib/constBuf.shader>
#include<../lib/io.shader>
cbuffer params:register(b0)
{
float tone;
}
VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 m[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=m[V];float4 p=float4(b/4,0,1);p.xyz=lerp(p.xyz,p.xzy,tone);p=mul(p,view[0]);p=mul(p,proj[0]);f.pos=p;f.uv=b/2.+.5;return f;}