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
  
        float2 xy = uv - .5 - .5 * float2(sin(i * 4. + time.x * .01), .5*sin(i * 5. + time.x * .012));
        float2 uv4 = float2(atan2(xy.y, xy.x) * 2, 2 / (length(xy) - .1) + time.y * .1);
        //walls += pow(saturate(1 - 2 * length(xy)), 2) * 12;
        uv5 += uv4*2;
        
    }
  
//  walls = pow((1 - 2 * (abs(sin(uv5.x)) * abs(sin(uv5.y / 13)))), 9) * 2;
    walls = pow((1 - 1 *(abs(sin(uv5.x)) * abs(sin(uv5.y / 13)))), 9) * 2;

    return walls;
}

float3 getTpos(float2 a, float R,float p,float q)
{    
        return float3((R + cos(a.x * q)) * cos(a.x * p), 
                      (R + cos(a.x * q)) * sin(a.x * p),
                      sin(a.x * q));
}

float3 torusKnot(float2 a)
{
    float R = 1.5;
    float r = .24;

    float p=5;
    float q=7;

    float3 pos = getTpos(a,R,p,q);
    float3 T = pos - getTpos(a+float2(.01,0),R,p,q);
    float3 N = pos+T;
                    
    float3 B = cross(T,N);
    N = cross(B,T);
    
    B= normalize(B);
    N= normalize(N);

    pos += r * (cos(a.y) * N + sin(a.y) * B);

    return pos;
}

float3 sphere(float2 uv)
{

    float2 a = uv * PI * 2;
    a.x *= -1;
    float3 pos = float3(sin(a.x), sin(a.y / 2), cos(a.x));
    pos.xz *= cos(a.y / 2);

    pos *= .35;
    float wc = pow(calcWalls(sin((uv * PI) * float2(4, 3))), .01);
    pos *= wc * .6 + 2;
    uv = abs(uv - .5);
    pos /= 1 + .4 * ((sin(uv.x * PI * 2*2) * cos(uv.y * PI * 4+time.y*.01))) ;
    pos *= 1.8;
    pos.y*=.5;
    pos.y-=.1*sin(length(pos.xz)*8+time.x*.1);
   // pos=clamp(pos,-1,1);
  
    pos.xyz=torusKnot(a);
    float3 pos2 = torusKnot(a+float2(.01,0));
    float3 pos3 = torusKnot(a+float2(.0,01));
    float3 t =pos2-pos.xyz;
    float3 b =pos3-pos.xyz;
    float3 n = cross(t,b);
    float a1= sin(a.x*4096/4)+sin(a.y*256/4+a.x*256);
    a1=saturate(a1);
    float3 ofs= n*(pow(a1,11)*5)*5.5*(sin(a.x*22+a.y*12)*.25+1+sin(time.x*sin(a.x+a.y)*.1)*.2);
    //ofs*=.42;
    //pos+=ofs*2;
    //pos.y-=pow(length(ofs.xyz),2);
    //pos = rotY(pos, time.x * 0.1);


    return pos;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.uv - .5;
    float3 pos = sphere(uv)*2;
    
    return float4(pos, 1.);

}
