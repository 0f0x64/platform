#include<../lib/constBuf.shader>
#include<../lib/io.shader>
cbuffer params:register(b4)
{
float4 p1;
};
float4 PS(VS_OUTPUT e):SV_Target{float s=3.141519,t=0;for(int x=1;x<3;x++){float2 f=2*(e.uv-.5)*s;f+=float2(sin(time.x*.13*sin(x*.4)),sin(time.x*.12*sin(x*.5)));t+=sin(atan2(f.x,f.y)*12-time.x*.3)*sin(1/length(f*2)+5)*saturate(1/pow(length(f),3))*2;}return float4(t,t,t,1.);}