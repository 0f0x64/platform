cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};float4 PS(VS_OUTPUT e):SV_Target{float s=3.141519,x=0;for(int f=1;f<3;f++){float2 t=2*(e.uv-.5)*s;t+=float2(sin(time.x*.13*sin(f*.4)),sin(time.x*.12*sin(f*.5)));x+=sin(atan2(t.x,t.y)*12-time.x*.3)*sin(1/length(t*2)+5)*saturate(1/pow(length(t),3))*2;}return float4(x,x,saturate(x*2)+.4,1);}