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
	VS_OUTPUT output = (VS_OUTPUT)0;
	float2 quad[6] = { 0,0, 1,0, 0,1, 1,0, 1,1, 0,1 };
	float2 p = quad[vID);
	output.Pos = float4(p, 0, 1);
	output.uv = p;
	return output;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 color = float4(0,1,0,1);
	return fcolor;
};