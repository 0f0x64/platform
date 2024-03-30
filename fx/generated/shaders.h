//automatically generated file: all used shaders as const char* strings

namespace shadersData {

const char* quad = "cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 C[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=C[V];f.pos=float4(b,0,1);f.uv=b/2.+.5;return f;}";

const char* simple = "cbuffer ConstantBuffer:register(b0){float4 time;};struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};float4 PS(VS_OUTPUT V):SV_Target{return float4(V.uv,1,1);}";


void CompileAll ()
{
dx::CompileShader (0, quad);
dx::CompileShader (1, simple);
};


};