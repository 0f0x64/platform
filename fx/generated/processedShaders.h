//automatically generated file: all used shaders as const char* strings

namespace shadersData {

const char* quad = ";

const char* quad2 = ";

const char* quad3 = cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 C[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=C[V];f.pos=float4(b/2,0,1);f.uv=b/2.+.5;return f;}";

const char* simple = cbuffer ConstantBuffer:register(b1){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};float4 PS(VS_OUTPUT e):SV_Target{float s=3.141519,f=0;for(int t=1;t<3;t++){float2 x=2*(e.uv-.5)*s;x+=float2(sin(time.x*.13*sin(t*.4)),sin(time.x*.12*sin(t*.5)));f+=sin(atan2(x.x,x.y)*12-time.x*.3)*sin(1/length(x*2)+5)*saturate(1/pow(length(x),3))*2;}return float4(f,f,f,1.);}";

const char* simple2 = Texture2D tex1:register(t0);SamplerState sam1:register(s0);cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};float4 PS(VS_OUTPUT t):SV_Target{float4 f=float4(t.uv,sin(t.uv.x*12),1);float2 s=t.uv*5;s.y-=2;f=tex1.Sample(sam1,s);return f;}";

const char* simple3 = #include<../lib/noise.shader>
#include<../lib/sinwave.shader>
Texture2D tex1:register(t0);SamplerState sam1:register(s0);cbuffer ConstantBuffer:register(b0){float4 time;};cbuffer params:register(b3){float4 tone;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};float4 PS(VS_OUTPUT t):SV_Target{float4 f=float4(t.uv,sin(t.uv.x*12),1);float2 s=t.uv*5;s.y-=2;f=tex1.Sample(sam1,s);if(tone.x==1)f=noise(float3(s*24,1));if(tone.y==1)f=wave(float3(s*24,1));return f;}";

const char* simple4 = Texture2D tex1:register(t0);SamplerState sam1:register(s0);cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};float4 PS(VS_OUTPUT t):SV_Target{float4 f=float4(t.uv,sin(t.uv.x*12),1);float2 s=t.uv;s.y-=2;f=tex1.Sample(sam1,s);return f;}";

const char* noise = 
"float hash(float n)\n"
"{\n"
"    return frac(sin(n) * 43758.5453);\n"
"}\n"
"     \n"
"float noise(float3 x)\n"
"{\n"
"    float3 p = floor(x);\n"
"    float3 f = frac(x);\n"
"     \n"
"    f = f * f * (3.0 - 2.0 * f);\n"
"    float n = p.x + p.y * 57.0 + 113.0 * p.z;\n"
"     \n"
"    return lerp(lerp(lerp(hash(n + 0.0), hash(n + 1.0), f.x),\n"
"               lerp(hash(n + 57.0), hash(n + 58.0), f.x), f.y),\n"
"               lerp(lerp(hash(n + 113.0), hash(n + 114.0), f.x),\n"
"               lerp(hash(n + 170.0), hash(n + 171.0), f.x), f.y), f.z);\n"
"}\n"
" \n"
;

const char* sinwave = 
"\n"
"float wave(float3 x)\n"
"{\n"
"    return sin(x.x) * cos(x.y);\n"
"}\n"
" \n"
;


void CompileAll ()
{
Vertex (0, quad);
Vertex (1, quad2);
Vertex (2, quad3);
Pixel (0, simple);
Pixel (1, simple2);
Pixel (2, simple3);
Pixel (3, simple4);
};


};