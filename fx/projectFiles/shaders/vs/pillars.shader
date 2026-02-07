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
}

float3 pillar(float2 grid,float a, float t, float h)
{
    float3 pos = shp(grid.xy);
    //heigth
    pos.y*=.8+h;
    pos.y+=h;

    pos*=1+noise3(pos*111.6+1111/(a+1))*1.6;
    pos.y*=1.8;
    pos*=1+noise3(pos*1.6+1111/(a+1)+t);
    pos = rot3(pos,2.2/(pow(pos,14)+1));
    pos.y*=1.5;
    pos*=1+noise3(pos+1111/(a+1)+t);
    pos*=1+noise3(pos*3.5)/3;
    pos*=2;
    return pos;
}

pos_color CalcParticles(uint qid,float4 grid)
{
     qid *= skipper;
     float t=time.x*.01;
     uint inStars = 10000;
     if (mode==1||qid%inStars==0)
     {
        t=0;
     }

    //pillars instances
    uint cn=7;
    float a= (qid%cn);
    a=a*PI/180.;
    a*=360./cn;
    float h=(sin(a*3)+2)/2;

    //calc
    float3 pos = pillar(grid.xy,a,t,h);
    float3 pos2=pos;
    
    //scatter
    pos.x+=17;
    pos = rot3(pos,float3(-.9,0,.4));
    pos.y/=1.3;
    pos.y-=h*10-8;
    if (qid%cn) pos=rotY(pos,a);
    pos*=-0.5;
    pos.y*=1.4;
    pos.y-=h;
    
    //color
    pos_color p;
    p.color = float4(noise3_u(a*float3(13,15,12)*221+77+sin(pos2*1.4)),1)/30.+.0015;

    if (mode==1)
    {
        p.pos=transform(pos,grid.zw,52);
        p.color*=8;
        p.sz=172;
    }
    else
    {
        p.pos = transform_unisize(pos,grid.zw,1.5);
    //   p.color+=-noise(pos*.12*float3(1,2,3)+2)*.1;;
       // p.color +=min(0,sign(1./noise(-pos2*.2-2.6)))/91.;
         p.sz=1;

         if (qid%inStars==0)
         {
              p.pos = transform_unisize(pos,grid.zw,41.5);
               p.sz=2;
               p.color*=15;
         }
    }
    p.pos.x+=0;
    //density compensation
    p.color/=min(pow(p.pos.w,1.1)*.21+.5,11);
    return p;
}

#include <../lib/particleVS_main.shader>
