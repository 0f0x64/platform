//automatically generated file: all used shaders as const char* strings

namespace shadersData {

const char* quad = "\
cbuffer ConstantBuffer : register(b0)\
{\
    float4 time;\
};\
\
\
struct VS_OUTPUT\
{\
    float4 pos : SV_POSITION;\
    float2 uv : TEXCOORD0;\
};\
\
\
VS_OUTPUT VS(uint vID : SV_VertexID)\
{\
    VS_OUTPUT output = (VS_OUTPUT) 0;\
    float2 quad[6] = { -1, -1, 1, -1, -1, 1, 1, -1, 1, 1, -1, 1 };\
    float2 p = quad[vID];\
    output.pos = float4(p/1, 0, 1);\
    output.uv = p / 2. + .5;\
    return output;\
}\
";

const char* simple = "\
cbuffer ConstantBuffer : register(b0)\
{\
    float4 time;\
};\
\
struct VS_OUTPUT\
{\
    float4 pos : SV_POSITION;\
    float2 uv : TEXCOORD0;\
};\
\
float4 PS(VS_OUTPUT input) : SV_Target\
{\
    float4 color = float4(sin(input.uv*118), 0, 1);\
    return color;\
}\
\
";


void CompileAll ()
{
dx::CompileVertexShader (0, quad);
dx::CompilePixelShader (0, simple);
};


};