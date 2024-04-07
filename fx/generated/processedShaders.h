//automatically generated file: all used shaders as const char* strings

namespace shadersData {

const char* quad = "\
cbuffer ConstantBuffer : register(b1)\
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
    output.uv = p / 3. + .5;\
    return output;\
}\
";

const char* quad2 = "\
cbuffer c1:register(b1)\
{\
    float4 time;\
};\
\
cbuffer c2 : register(b2)\
{\
    float4x4 camera[2][3];\
};\
\
cbuffer c3: register(b3)\
{\
    float4 tone;\
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
    float4 p2 = float4(p/4, 0, 1);\
    \
    p2.xyz = lerp(p2.xyz, p2.xzy, tone.x);\
        \
    p2 = mul(p2, camera[0][1]);\
    p2 = mul(p2, camera[0][2]);  \
    \
    output.pos = p2;\
    \
    output.uv = p / 2. + .5;\
    return output;\
}\
";

const char* quad3 = "\
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
    output.pos = float4(p / 2, 0, 1);\
    output.uv = p / 2. + .5;\
    return output;\
}\
";

const char* simple = "\
cbuffer ConstantBuffer:register(b1)\
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
    for (int i = 1; i < 3;i++)\
    {\
        float2 uv = 2 * ((input.uv) - .5)*pi;\
        uv += float2(sin(time.x * .13 * sin(i*.4)), sin(time.x * .12 * sin(i*.5)));\
        c += sin((atan2(uv.x, uv.y) * 12 - time.x * .3)) * (sin(1/length(uv * 2) + 5)) * saturate(1/pow(length(uv),3))*2;\
    }\
    return 1;\
    return float4(c, c, c , 1);\
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
    float2 uv = input.uv*5;\
    uv.y -= 2;\
    color = tex1.Sample(sam1, uv);\
\
    return color;\
}\
\
";

const char* simple3 = "\
Texture2D tex1 : register(t0);\
SamplerState sam1 : register(s0);\
\
cbuffer c1 : register(b0)\
{\
    float4 time;\
};\
\
cbuffer c2 : register(b3)\
{\
    float4 tone;\
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
float4 PS(VS_OUTPUT input) : SV_Target\
{\
    float4 color = float4(input.uv, sin(input.uv.x * 12), 1);\
     \
    float2 uv = input.uv ;\
    uv.y -= 2;\
    color = tex1.Sample(sam1, uv);\
    color.a = dot(color.rgb, 1);\
    color *= tone;\
    \
    return color;\
}\
\
";

const char* simple4 = "\
Texture2D tex1 : register(t0);\
SamplerState sam1 : register(s0);\
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
    float2 uv = input.uv;\
    uv.y -= 2;\
    color = tex1.Sample(sam1, uv);\
    \
    return color;\
}\
\
";


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