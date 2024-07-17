#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>

//[
cbuffer params : register(b1)
{
    float sx,sy,sz;
};
//]

float3 rotX(float3 pos,float a)
{
    float3x3 m =
    {
        1, 0,       0,
        0, cos(a), -sin(a),
        0, sin(a), cos(a)
    };
    pos = mul(pos, m);
    return pos;
}

float3 rotY(float3 pos, float a)
{
    float3x3 m =
    {
        cos(a), 0, sin(a),
        0, 1, 0,
        -sin(a), 0, cos(a)
    };
    pos = mul(pos, m);
    return pos;
}

float3 rotZ(float3 pos, float a)
{
    float3x3 m =
    {
        cos(a), -sin(a),0,
        sin(a), cos(a), 0,
        0, 0, 1
    };
    pos = mul(pos, m);
    return pos;
}

float calcWalls(float2 uv)
{
    float walls = 0;
    float2 uv5 = float2(0, 0);
    for (int i = 0; i < 5; i++)
    {
  
        float2 xy = uv - .5 - .25 * float2(sin(i * 4. + time.x * .01), sin(i * 5. + time.x * .02));
        float2 uv4 = float2(atan2(xy.y, xy.x) * 1, 12 / (length(xy) + .1) + time.y * .1);
        walls += pow(saturate(1 - 2 * length(xy)), 62) * 120;
        uv5 += uv4;
        
    }
    //walls += pow(saturate(1 - 2 * (abs(sin(uv5.x)) * abs(sin(uv5.y / 13)))), 5) * 2;
    walls = pow((1 - 2 * (abs(sin(uv5.x)) * abs(sin(uv5.y / 13)))), 9) * 2;
    //walls = walls /  length(uv - .5);
    return walls;
}

float3 sphere(float2 uv)
{

    float2 a = uv * PI * 2;
    a.x *= -1;
    float3 pos = float3(sin(a.x), sin(a.y / 2), cos(a.x));
    pos.xz *= cos(a.y / 2);

    pos *= .35;
    float wc = pow(calcWalls(sin((uv * PI) * float2(4, 3))), .01);
    pos *= wc * .6 + 1;
    uv = abs(uv - .5);
    pos /= 1 + .24 * ((sin(uv.x * PI * 2) * cos(uv.y * PI * 4+time.y*.01))) ;
    pos *= 1.5;
  
    
    return pos;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.uv - .5;
    float3 pos = sphere(uv)*2;
    
    return float4(pos, 1.);

}
