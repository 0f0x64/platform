cbuffer ConstantBuffer:register(b1)
{
float4 time;
};
struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};float4 PS(VS_OUTPUT e):SV_Target{float s=3.141519,f=0;for(int t=1;t<3;t++){float2 x=2*(e.uv-.5)*s;x+=float2(sin(time.x*.13*sin(t*.4)),sin(time.x*.12*sin(t*.5)));f+=sin(atan2(x.x,x.y)*12-time.x*.3)*sin(1/length(x*2)+5)*saturate(1/pow(length(x),3))*2;}return float4(f,f,f,1.);}