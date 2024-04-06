//automatically generated file: all used shaders as const char* strings

namespace shadersData {

const char* quad = "cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 C[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=C[V];f.pos=float4(b,0,1);f.uv=b/2.+.5;return f;}";

const char* quad2 = "cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 C[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=C[V];f.pos=float4(b/2-.4,0,1);f.uv=b/2.+.5;return f;}";

const char* quad3 = "cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 C[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=C[V];f.pos=float4(b/2,0,1);f.uv=b/2.+.5;return f;}";

const char* simple = "cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};float4 PS(VS_OUTPUT e):SV_Target{float s=3.141519,f=0;for(int t=1;t<3;t++){float2 x=2*(e.uv-.5)*s;x+=float2(sin(time.x*.13*sin(t*.4)),sin(time.x*.12*sin(t*.5)));f+=sin(atan2(x.x,x.y)*12-time.x*.3)*sin(1/length(x*2)+5)*saturate(1/pow(length(x),3))*2;}return float4(f,f,f,1);}";

const char* simple2 = "Texture2D tex1:register(t0);SamplerState sam1:register(s0);cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};float4 PS(VS_OUTPUT t):SV_Target{float4 f=float4(t.uv,sin(t.uv.x*12),1);float2 s=t.uv*5;s.y-=2;f=tex1.Sample(sam1,s);return f;}";

const char* simple3 = "Texture2D tex1:register(t0);SamplerState sam1:register(s0);cbuffer ConstantBuffer:register(b0){float4 time;};cbuffer ConstantBuffer:register(b3){float4 tone;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};float4 PS(VS_OUTPUT t):SV_Target{float4 s=float4(t.uv,sin(t.uv.x*12),1);float2 f=t.uv;f.y-=2;s=tex1.Sample(sam1,f);s.w=dot(s.xyz,1);s*=tone;return s;}";

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