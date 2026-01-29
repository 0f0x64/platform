#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>
#include <../lib/utils.shader>

cbuffer params : register(b0)
{
    float4x4 model;
    int gX;
    int gY;
    int mode;
    int skipper;
    float4 base_color;
}

float toRad(float a)
{
    return a*PI/180.;
}

float smin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return lerp(b, a, h) - k * h * (1.0 - h);
}

float3 hash33(float3 p)
{
    return float3(hash(p.x),hash(p.y),hash(p.z))-.5;
}

float3 hash31(float p) {
   float3 p3 = frac(p * float3(.1031, .1030, .0973));
   p3 += dot(p3, p3.yzx + 33.33);
   return frac((p3.xxy + p3.yzz) * p3.zyx); 
}

pos_color CalcParticles(uint qid,uint iid,float4 grid)
{

float4 _ColorHot = float4(1.0, 0.9, 0.7,1);
float4 _ColorCold = float4( 0.1, 0.0, 0.4,1);

float form=smoothstep(0,1,smoothstep(0,1,.5+.5*sin(time.x/32)));

float3 pos;
    pos_color p1;

    uint inStars = 9623;
    float t = time.x * 1.;
 
    // if (iid%inStars==0)
         {
             t=0;
         }
    float4 a = float4(1.4 + sin(t*0.1)*0.1, -2.3, 2.4, -2.1 + cos(t*0.15)*0.1);
    float4 b = float4(0.8, -1.2 + sin(t*0.2)*0.05, 1.3, -1.5);

    float3 p = hash31((float)iid * 0.123) -.5;
    p=normalize(p)/1.5;
    float3 p_prev = p;
    
    for (int i = 0; i < 18; i++) {
        p_prev = p;
        float3 next = noise3(p*8+22+a/b+(time.x/32)*0);
        
        p -=next/(i+1)*(1-form);
        //p=rot3(p,p*3*(1-form));
        p=lerp(p,rot3(p,p*3),(1-form));
        p+=(rot3(p-p/2,p*3)+p/2)/11*form;
        p += next/(i+1)*form;


    }
    
    p+=noise3(p*11+(time.x/32))/14;

    float delta = length(p - p_prev)*1;
    float heat = exp(-delta * 10); 

    p*=12;
    //p=rotZ(p,toRad(90)*(1-form))*4;
    //p=rotZ(p,toRad(90)*(form));
    p=p.zxy*lerp(14,6,form);

    float3 jitter = (hash31((float)iid) - 0.5) * 0.02;
    p += jitter/(pow(length(p),2)+.1)*172;

    float3 nebulaColor = lerp(_ColorCold.rgb, _ColorHot.rgb, pow(heat, 3.0));
    p1.color = float4(nebulaColor, 1);

    pos=p;
    

     qid *= skipper;
     t=time.x*.004;

     if (mode==1||iid%inStars==0)
     {
        t=0;
     }

    if (mode==0)    
    {
         if (iid%inStars==0)
         {
              p1.pos = transform(pos,grid.zw,75.5);
              p1.sz=1.2;
              p1.color*=2;
              p1.color*=lerp(0.03,1,form);
         }
         else{
            p1.pos = transform_unisize(pos,grid.zw,2.);
            p1.sz=1.2;
         }

    }
    else
    {
        p1.pos = transform(pos,grid.zw,121.2);
        p1.color*=.01;
        p1.sz=1.2;
        p1.color*=lerp(0.03,1,form);

    }

    //density compensation
    //p1.color/=min(pow(p1.pos.w,1.1)*.21+.5,11);
    return p1;
}

#include <../lib/particleVS_main2.shader>
