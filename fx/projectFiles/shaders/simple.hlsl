cbuffer ConstantBuffer : register(b0)
{
	float4 time;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 color = float4(input.uv,1 ,1);
	return color;
}