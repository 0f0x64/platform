#include<../lib/constBuf.shader>
#include<../lib/io.shader>
cbuffer params:register(b1)
{
float p1;float p2;
};
float4 PS(VS_OUTPUT e):SV_Target{float s=3.141519,f=0;for(int p=1;p<3;p++){float2 x=2*(e.uv-.5)*s;x+=float2(sin(time.x*.13*sin(p*.4)),sin(time.x*.12*sin(p*.5)));f+=sin(atan2(x.x,x.y)*12-time.x*.3)*sin(1/length(x*2)+5)*saturate(1/pow(length(x),3))*2;}return float4(f,f,f,1.);}