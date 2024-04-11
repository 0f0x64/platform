#include<../lib/constBuf.shader>
#include<../lib/io.shader>
cbuffer params:register(b1)
{
float r,g,b;
};
float4 PS(VS_OUTPUT e):SV_Target{float s=3.141519,x=0;for(int f=1;f<3;f++){float2 b=2*(e.uv-.5)*s;b+=float2(sin(time.x*.13*sin(f*.4)),sin(time.x*.12*sin(f*.5)));x+=sin(atan2(b.x,b.y)*12-time.x*.3)*sin(1/length(b*2)+5)*saturate(1/pow(length(b),3))*2;}return float4(x*r,x*g,x*b,1.);}