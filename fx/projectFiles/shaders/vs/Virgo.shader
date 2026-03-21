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

float quantize2(float x, float q)
{
    return floor(x*q)/q;
}

float3 torus(float u, float v, float R, float r) {
    float TWO_PI = 6.283185307;
    float theta = u * TWO_PI/3; // Angle around the major ring
    float phi = v * TWO_PI;   // Angle around the minor tube

    float x = (R + r * cos(phi)) * cos(theta);
    float y = (R + r * cos(phi)) * sin(theta);
    float z = r * sin(phi);

    return float3(x, y, z);

}

float3 getTpos(float2 a, float R,float p,float q)
{    
        return float3((R + cos(a.x * q)) * cos(a.x * p), 
                      (R + cos(a.x * q)) * sin(a.x * p),
                      sin(a.x * q));
}

float3 torusKnot(float2 a)
{
    
    float R = 3.5;
    float r = 3.;

    float p=1;
    float q=3.;

    float3 pos = getTpos(a,R,p,q);
    float3 T = pos - getTpos(a+float2(.01,0),R,p,q);
    float3 N = pos+T;
                    
    float3 B = cross(T,N);
    N = cross(B,T);
    
    B= normalize(B);
    N= normalize(N);
    float t=time.x/270;
    pos += r * (cos(a.y) * N + sin(a.y) * B);
    //pos=rot3(pos,t);
    return pos;
}

float3 SphericalToCartesian(float3 p)
{
    float radius=p.x;
    float theta=p.y;
    float phi=p.z;
    float3 cartesian;
    // В HLSL обычно Y — это верх (Up), поэтому:
    // x = r * sin(phi) * cos(theta)
    // y = r * cos(phi)
    // z = r * sin(phi) * sin(theta)
    
    float sinPhi = sin(phi);
    
    cartesian.x = radius * sinPhi * cos(theta);
    cartesian.y = radius * cos(phi);
    cartesian.z = radius * sinPhi * sin(theta);
    
    return cartesian;
}

float3 CartesianToSpherical(float3 p)
{
    float radius = length(p);
    
    // Если точка в центре координат, углы не определены
    if (radius <= 0.0) return float3(0, 0, 0);

    // theta = угол в плоскости XZ (азимут)
    // Используем atan2(z, x) для полного диапазона [-PI, PI]
    float theta = atan2(p.z, p.x);
    
    // phi = угол от оси Y (зенит) [0, PI]
    // acos возвращает значения от 0 до PI
    float phi = acos(p.y / radius);
    
    return float3(radius, theta, phi);
}

float3 pillar(uint qid,uint iid,float2 grid,float a, float t, float h)
{
    float3 pos = float3(hash(iid/200.),hash(iid/140.),hash(iid/120.))-.5;
    uint inStars = 1232*2;

    float3 pos3=pos;
    float l=hash(iid%15)*20+20;
    pos=torusKnot(grid*PI*2);
    pos=rot3(pos,pos/12);
    pos=rot3(pos,(iid%15)/3*float3(4,5,6));
    
    pos*=5.6;

    pos+=rot3(pos,noise3(pos/10+1122));
    pos=rot3(pos,noise3(pos/5+1122)/5);
    pos+=noise3(pos/4+112)*6;

    float dst=3;
    for (int i=0;i<32;i++)
    {
        float4 j=(i+1)*float4(5,7,8,14)+time.x*.01;
        float3 hole=float3(sin(j.x),cos(j.y),sin(j.z)*cos(j.w));
        hole=normalize(hole)*62;
        dst=165/(distance(pos,hole));
        pos-=normalize(hole-pos)*pow(dst,1);
    }

    pos=CartesianToSpherical(pos);
    pos.x+=noise3(pos/15)*22;
    pos.x+=noise3(pos/15)*22;
    pos=SphericalToCartesian(pos); 
    
    for (int i=0;i<32;i++)
    {
        float4 j=(i+1)*float4(5,7,8,14)+time.x*.01;
        float3 hole=float3(sin(j.x),cos(j.y),sin(j.z)*cos(j.w));
        hole=normalize(hole)*62;
        dst=165/(distance(pos,hole));
        pos-=normalize(hole-pos)*pow(dst,1);
    }
    pos*=1-noise3(pos/5)*pow(length(pos)/422,3);
    //pos*=1+1/noise3(pos/3)*pow(length(pos)/422,5);
    
    return pos/6;
}



pos_color CalcParticles(uint qid,uint iid,float4 grid)
{
     qid *= skipper;
     float t=time.x*.004;
     uint inStars = 1232*123;
     if (mode==1||iid%inStars==0)
     {
        t=0;
     }

    //pillars instances


    //calc
    float3 pos = pillar(qid,iid,grid.xy,0,t,0);
    float3 pos2=pos;
    
    //scatter
    
    //color
    pos_color p;
    p.color.a=1;
    p.color.rgb = noise3_u(pos*14*float3(122,1,112))/31+float3(6,2,3)/52;
    
    //p.color*=.5;
//    p.color*=base_color*(pow(length(pos)/16,4)+.1);
    //p.color*=1+sin(grid.x*PI*8);
    p.color=lerp(p.color,p.color.bgra,sin(length(pos)));
//    p.color=lerp(p.color,base_color/144,1-saturate(pow(length(pos)/6,11)));


    if (mode==1)
    {
        float s=hash(iid)*33+11;
        s=noise(iid)*62+11;
        //s*=1.5;
        p.pos=transform(pos,grid.zw,s);
        p.color*=2.;
        p.sz=172;
    }
    else
    {
        p.pos = transform(pos,grid.zw,1.2);
       //p.color=-noise(pos*.3+12)*.04+.02;;
       // p.color +=min(0,sign(1./noise(-pos2*.2-2.6)))/91.;
         p.sz=2;
         //p.color*=1.2;

         if (iid%inStars==0)
         {
              p.pos = transform_unisize(pos,grid.zw,75.5);
               p.sz=2;
               p.color*=7;
         }

    }
      /*    if (iid==0)
         {
              p.pos = transform(0,grid.zw,16.5);
               p.sz=2;
               p.color*=(float4(5,-.1,-1,1));
               p.color*=10;
         }*/
  
   
    //density compensation
    if (mode==0)
    {
    p.color*=1*saturate(p.pos.w/27);
    //p.color*=0;

    }

    if (mode==1)
    {
    p.color*=.3*saturate(21/p.pos.w);
    //p.color=.02;
    }
    
   // p.color/=min(pow(p.pos.w,.5)*.1+1.5,5);
    return p;
}

#include <../lib/particleVS_main2.shader>
