#include<../lib/constBuf.shader>
#include<../lib/io.shader>
cbuffer params:register(b1)
{
float p;
}
struct PS_OUTPUT{float4 face0:SV_Target0;float4 face1:SV_Target1;float4 face2:SV_Target2;float4 face3:SV_Target3;float4 face4:SV_Target4;float4 face5:SV_Target5;};PS_OUTPUT PS(VS_OUTPUT f):SV_Target{float2 P=f.uv;float4 p=float4(P,0,1);PS_OUTPUT o;o.face0=p;o.face1=p;o.face2=p;o.face3=p;o.face4=p;o.face5=p;return o;}