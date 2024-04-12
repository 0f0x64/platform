#include<../lib/constBuf.shader>
#include<../lib/io.shader>
cbuffer params:register(b0)
{
float gX,gY;
}
VS_OUTPUT VS(uint g:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 m[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=m[g];float V=g/3%uint(gX),v=floor(g/3/uint(gX));V/=gX;v/=gY;b.x=V-.5;b.y=v-.5;float4 X=float4(b/4,0,1);X=mul(X,view[0]);X=mul(X,proj[0]);f.pos=X;f.uv=b/2.+.5;return f;}