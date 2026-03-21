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
    pos+=noise3(a*351*float3(4,25,67))*1.7;

    pos = rot3(pos,noise3(pos*.8+float3(0,t,0))/6);
    pos+= noise3(pos)*.8;
    pos = rot3(pos,noise3(pos.zyx*1.6+float3(0,-t,0))/12);
    float3 pos3=pos;
    //pos+=(frac(pos/7)-.5)*4;
    //pos=lerp(pos,normalize(pos)*10,saturate(length(pos/37)));

    //pos*=.9;


    pos=lerp(pos,normalize(pos)*18+pos/32,saturate(length(pos/12)));
//    pos+= noise3(pos/6)*.8;
int ic=6;
pos.y=min(pos.y,0);
pos.x+=hash(iid%ic)*24-12;
pos.z+=hash(iid%ic)*25-12;
pos.x-=1;
pos.z+=1;
pos/=2.5;
pos/=-sin(iid%ic+1)+2;

pos.y-=sin(iid%ic+33)*16;

pos.y-=12*frac((iid%13==0)*time.x*hash(iid)*.0251);
pos.y-=1*sin(time.x/10+sin(iid%ic+33));
    //pos.y-=5;


    return pos/3;
}



pos_color CalcParticles(uint qid,uint iid,float4 grid)
{
     qid *= skipper;
     float t=time.x*.4;
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
    p.color = float4(noise3_u(111+float3(113,11,111)*221+177+sin(pos2*.48)),1)/110.+.0015;
    p.color*=base_color;

    if (mode==1)
    {
        float s=hash(iid)*33+11;
        //s*=1.5;
        p.pos=transform(pos,grid.zw,s);
        p.color*=.6;
        p.sz=172;
    }
    else
    {
        p.pos = transform_unisize(pos,grid.zw,1.75);
       //p.color=-noise(pos*.3+12)*.04+.02;;
       // p.color +=min(0,sign(1./noise(-pos2*.2-2.6)))/91.;
         p.sz=1;
         p.color*=1.2;

         if (iid%inStars==0)
         {
              p.pos = transform_unisize(pos,grid.zw,151.5);
               p.sz=2;
               p.color*=13;
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
