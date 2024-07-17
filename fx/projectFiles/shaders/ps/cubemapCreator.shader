#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>

//[
cbuffer params : register(b1)
{
    float p;
}
//]

struct PS_OUTPUT
{
    float4 c0 : SV_Target0;
    float4 c1 : SV_Target1;
    float4 c2 : SV_Target2;
    float4 c3 : SV_Target3;
    float4 c4 : SV_Target4;
    float4 c5 : SV_Target5;
    
};


float calcWalls(float2 uv)
{
    float walls = 0;
    float2 uv5 = float2(0, 0);
    for (int i = 0; i < 5; i++)
    {
  
        float2 xy = uv - .5 - .15 * float2(sin(i * 4. + time.x * .01), sin(i * 5. + time.x * .02));
        float2 uv4 = float2(atan2(xy.y, xy.x) * 1, 12 / (length(xy) + .1) + time.y * .1);
        walls += pow(saturate(1 - 2 * length(xy)), 62) * 120;
        uv5 += uv4;
        
    }
    walls += pow(saturate(1 - 1 * (abs(sin(uv5.x)) * abs(sin(uv5.y / 13)))), 5) * 2;
    walls = walls * saturate(1 - 2 * length(uv - .5));
    return walls;
}

PS_OUTPUT PS(VS_OUTPUT input) 
{

    float2 uv = input.uv;
    float4 color = 1;//    float4(uv, 0, 1);

    PS_OUTPUT cube;
    float a = PI*4;
    float walls = saturate(1 - 2 * length(uv - .5));
    //color.xyz = saturate(-sign(max(sin(uv.x * a), sin(uv.y * a))));
    float top = 0;
    for (int i = 0; i < 5;i++)
    {
        float2 xy = .25*float2(sin(i * PI / 5*2), cos(i * PI / 5*2))+.5;
        top += pow(saturate(1 - 2 * length(uv - xy)), 32) * 550;
    }
    cube.c2 = top * float4(3,3,4,1);
    
    float2 uv3 = float2(atan2(uv.y-.5, uv.x-.5) * PI*.06, length(uv - .5)-time.x*.01);
    
    float floor_ = 0.0;//    (max(sin(uv3.x * 112), sin(uv3.y * 112)) - .7) * saturate(1 - length(uv - .5) * 4);
    
    cube.c3 = float4(1, 1, 1, 1) * saturate(floor_);
    
    walls =calcWalls(uv);
    walls -= calcWalls(uv + float2(0.0035,.0035))*1.2;
    //walls += .2;
    //walls *= 1 - uv.y;
    
    //walls = max(walls, 0);
    
    //cube.c2 = walls * float4(1, 1, 1, 1)*2;
    //cube.c3 = walls * float4(.2, .6, 1, 1)/4;
    
    cube.c4 = walls*float4(1,1,1.3,1);
    cube.c5 = walls*float4(1,1,1.3,1);
    cube.c0 = walls*float4(1,1,1.3,1);
    cube.c1 = walls*float4(1,1,1.3,1);

    return cube;
}