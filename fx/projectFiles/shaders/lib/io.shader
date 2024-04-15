struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 vpos : POSITION0;
    float4 wpos : POSITION1;
    float2 uv : TEXCOORD0;
};

struct VS_OUTPUT_POSONLY
{
    float4 pos : SV_POSITION;
};