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

float3 pillar(uint qid,uint iid,float2 grid,float a, float t, float h)
{
    float3 pos = float3(hash(iid/200.),hash(iid/140.),hash(iid/120.))-.5;
    pos=normalize(pos)*2;
    pos+=pos*noise3(pos);

    pos*=4;
    //t=0;


    //pos/=10;
    //return pos;
    //heigth
    a=hash(iid/1000.);
    //pos+=noise3(a*351*float3(4,25,67))*1.7;

    pos = rot3(pos,noise3(pos*.8+float3(0,t,0))/6);
    pos+= noise3(pos)*.8;
    pos = rot3(pos,noise3(pos.zyx*1.6+float3(0,-t,0))/12);
    float3 pos3=pos;
    pos=(frac(pos/7)-.5)*14;
    //pos=lerp(pos,normalize(pos)*5+pos/2,saturate(length(pos/7)+.2));


    //pos*=1+noise3(pos*1.6+1111/(a+1)+t)*.24;
    //pos = rotY(pos,length(pos)*2);
    //pos*=1+noise3(pos+1111/(a+1)+t)*.3;
    //pos*=1+noise3(pos*3.5)/5;
    
    /*if (mode==0)
    {
        if (iid%5==0)
        {
            pos=pos3/40;
            pos*=1+noise(pos)*6;
            pos=rot3(pos,pos*22+time.x*.05);
        }
    }*/

    return pos*2;
}



pos_color CalcParticles(uint qid,uint iid,float4 grid)
{
     qid *= skipper;
     float t=time.x*.004;
     uint inStars = 10000;
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
    p.color = float4(noise3_u(float3(113,115,1)*221+177+sin(pos2*.48)),1)/110.+.0015;
    p.color*=base_color/2;

    if (mode==1)
    {
        float s=hash(iid)*33+11;
        p.pos=transform(pos,grid.zw,s);
        p.color*=12;
        p.sz=172;
    }
    else
    {
        p.pos = transform_unisize(pos,grid.zw,1.5);
       //p.color=-noise(pos*.3+12)*.04+.02;;
       // p.color +=min(0,sign(1./noise(-pos2*.2-2.6)))/91.;
         p.sz=1;

         if (iid%inStars==0)
         {
              p.pos = transform_unisize(pos,grid.zw,151.5);
               p.sz=2;
               p.color*=3;
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
    //p.color/=min(pow(p.pos.w,1.1)*.21+.5,11);
    return p;
}

#include <../lib/particleVS_main2.shader>
