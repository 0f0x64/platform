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
    output.pos = float4(p, 0, 1);\
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
    output.pos = float4(p , 0, 1);\
    output.pos.xy /= 2;\
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
    float pi = 3.141519;\
    \
    float c = 0;\
    for (int i = 1; i < 5;i++)\
    {\
        float2 uv = 2 * ((input.uv) - .5)*pi;\
        uv += float2(sin(time.x * .13 * sin(i*.4)), sin(time.x * .12 * sin(i*.5)));\
        c += sin((atan2(uv.x, uv.y) * 12 - time.x * .3)) * (sin(1/length(uv * 2) + 5)) * saturate(1/pow(length(uv),3))*2;\
    }\
    \
\
    return float4(c, c, saturate(c * 2) + .4, 1)+.3;\
\
}\
\
";

const char* simple2 = "\
Texture2D tex1 : register(t0);\
SamplerState sam1: register(s0);\
\
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
     \
    color = tex1.Sample(sam1, input.uv);\
    \
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