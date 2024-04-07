cbuffer c1:register(b1)
{
    float4 time;
};

cbuffer c2 : register(b2)
{
    float4x4 camera[2][3];
};

cbuffer c3: register(b3)
{
    float4 tone;
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
    float4 p2 = float4(p/4, 0, 1);
    
    p2.xyz = lerp(p2.xyz, p2.xzy, tone.x);
        
    p2 = mul(p2, camera[0][1]);
    p2 = mul(p2, camera[0][2]);  
    
    output.pos = p2;
    
    output.uv = p / 2. + .5;
    return output;
}
