#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
cbuffer params:register(b1)
{
float p;
}
struct PS_OUTPUT{float4 c0:SV_Target0;float4 c1:SV_Target1;float4 c2:SV_Target2;float4 c3:SV_Target3;float4 c4:SV_Target4;float4 c5:SV_Target5;};PS_OUTPUT PS(VS_OUTPUT c){float2 P=c.uv;float4 f=float4(P,0,1);PS_OUTPUT p;f.xyz=saturate(sin(P.x*32)*sin(P.y*32)*100).xxx;p.c0=f*float4(0,0,1,1);p.c1=f*float4(0,1,0,1);p.c2=f*float4(0,1,1,1);p.c3=f*float4(1,0,0,1);p.c4=f*float4(1,0,1,1);p.c5=f*float4(1,1,0,1);return p;}