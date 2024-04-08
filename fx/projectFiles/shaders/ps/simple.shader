cbuffer ConstantBuffer:register(b1)
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
    float pi = 3.141519;
    
    float c = 0;
    for (int i = 1; i < 3;i++)
    {
        float2 uv = 2 * ((input.uv) - .5)*pi;
        uv += float2(sin(time.x * .13 * sin(i*.4)), sin(time.x * .12 * sin(i*.5)));
        c += sin((atan2(uv.x, uv.y) * 12 - time.x * .3)) * (sin(1/length(uv * 2) + 5)) * saturate(1/pow(length(uv),3))*2;
    }
    //return 1;
    return float4(c, c, c , 1);

}

