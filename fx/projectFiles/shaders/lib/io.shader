struct VS_OUTPUT_POSONLY
{
    float4 pos : SV_POSITION;
};

struct VS_OUTPUT_POS_UV
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float2 sz : TEXCOORD1;
};


struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 vpos : POSITION0;
    float4 wpos : POSITION1;
    float4 vnorm : NORMAL1;
    float2 uv : TEXCOORD0;
    float4 id : TEXCOORD1;
    float4 color : TEXCOORD2;
    float4 sz1 : COLOR0;
};

struct VS_OUTPUT_PARTICLE
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
    float size : COLOR1;
};
