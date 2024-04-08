cbuffer c1 : register(b1)
{
    float4 time;
};

cbuffer c2 : register(b2)
{
    float4x4 camera[2][3];
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};


VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    float2 quad[6] = { -1, -1, 1, -1, -1, 1, 1, -1, 1, 1, -1, 1 };
    float2 p = quad[vID];
    output.pos = float4(p, 0, 1);
    output.uv = p / 3. + .5;
    return output;
}
