#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
cbuffer params:register(b1)
{
float p;
}
struct PS_OUTPUT{float4 c0:SV_Target0;float4 c1:SV_Target1;float4 c2:SV_Target2;float4 c3:SV_Target3;float4 c4:SV_Target4;float4 c5:SV_Target5;};PS_OUTPUT PS(VS_OUTPUT c){float2 s=c.uv;float4 f=1;PS_OUTPUT P;float x=PI*4,y=saturate(1-2*length(s-.5));f.xyz=saturate(-sign(max(sin(s.x*x),sin(s.y*x))));P.c0=y*float4(0,0,1,1);P.c1=y*float4(0,1,0,1);float p=saturate(-sign(max(sin(s.x*x),sin(s.y*x))))*12;P.c2=p;float l=saturate(-sign(sin(s.x*x*4)*sin(s.y*x*4)))*(1-length(s-.5)*2);P.c3=float4(.5,.5,.35,1)*l;P.c4=y*float4(1,0,0,1);P.c5=y*float4(1,0,1,1);return P;}