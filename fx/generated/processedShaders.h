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
    output.pos = float4(p/2, 0, 1);\
    output.uv = p / 2. + .5;\
    return output;\
}\
";

const char* quad2 = "\
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
    output.pos = float4(p / 5, 0, 1);\
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
    float4 color = float4(sin(input.uv*118), 1, 1);\
    return color;\
}\
\
";

const char* simple2 = "\
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
    float4 color = float4(input.uv, sin(input.uv.x * 12), 1);\
    return color;\
}\
\
";


void CompileAll ()
{
dx::Shaders::Compiler::Vertex (0, quad);
dx::Shaders::Compiler::Vertex (1, quad2);
dx::Shaders::Compiler::Pixel (0, simple);
dx::Shaders::Compiler::Pixel (1, simple2);
};


};