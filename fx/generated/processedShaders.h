//automatically generated file: all used shaders as const char* strings

namespace shadersData {

const char* quad = "cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 C[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=C[V];f.pos=float4(b,0,1);f.uv=b/2.+.5;return f;}";

const char* quad2 = "cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 C[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=C[V];f.pos=float4(b/2-.4,0,1);f.uv=b/2.+.5;return f;}";

const char* quad3 = "cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 C[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=C[V];f.pos=float4(b/2,0,1);f.uv=b/2.+.5;return f;}";

const char* simple = "cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};float4 PS(VS_OUTPUT e):SV_Target{float s=3.141519,x=0;for(int f=1;f<3;f++){float2 t=2*(e.uv-.5)*s;t+=float2(sin(time.x*.13*sin(f*.4)),sin(time.x*.12*sin(f*.5)));x+=sin(atan2(t.x,t.y)*12-time.x*.3)*sin(1/length(t*2)+5)*saturate(1/pow(length(t),3))*2;}return float4(x,x,saturate(x*2)+.4,1);}";

const char* simple2 = "Texture2D tex1:register(t0);SamplerState sam1:register(s0);cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};float4 PS(VS_OUTPUT t):SV_Target{float4 f=float4(t.uv,sin(t.uv.x*12),1);float2 s=t.uv*5;s.y-=2;f=tex1.Sample(sam1,s);return f;}";

const char* simple3 = "Texture2D tex1:register(t0);SamplerState sam1:register(s0);cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};float4 PS(VS_OUTPUT t):SV_Target{float4 s=float4(t.uv,sin(t.uv.x*12),1);float2 f=t.uv;f.y-=2;s=tex1.Sample(sam1,f)/2;s.w=dot(s.xyz,1);return s;}";

const char* simple4 = "Texture2D tex1:register(t0);SamplerState sam1:register(s0);cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};float4 PS(VS_OUTPUT t):SV_Target{float4 f=float4(t.uv,sin(t.uv.x*12),1);float2 s=t.uv;s.y-=2;f=tex1.Sample(sam1,s);return f;}";


void CompileAll ()
{
dx::Shaders::Compiler::Vertex (0, quad);
dx::Shaders::Compiler::Vertex (1, quad2);
dx::Shaders::Compiler::Vertex (2, quad3);
dx::Shaders::Compiler::Pixel (0, simple);
dx::Shaders::Compiler::Pixel (1, simple2);
dx::Shaders::Compiler::Pixel (2, simple3);
dx::Shaders::Compiler::Pixel (3, simple4);
};


};