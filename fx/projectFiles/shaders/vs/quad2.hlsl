cbuffer ConstantBuffer : register(b0)
{
    float4 time;
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
    output.pos = float4(p/2 , 0, 1);
    output.uv = p / 2. + .5;
    return output;
}
