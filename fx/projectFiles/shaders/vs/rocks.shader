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

float2 random2( float2 p ) {
    return frac(sin(float2(dot(p,float2(127.1,311.7)),dot(p,float2(269.5,183.3))))*43758.5453);
}
//[
float cellnoise(float2 p)
{
    float2 st = p;
    float3 color = .0;

    // Scale
    st *= 3.;

    // Tile the space
    float2 i_st = floor(st);
    float2 f_st = frac(st);

    float m_dist = 1.;  // minimum distance

    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
            // Neighbor place in the grid
            float2 neighbor = float2((x),(y));

            // Random position from current + neighbor place in the grid
            float2 pt = random2(i_st + neighbor);

			// Animate the point
            pt = 0.5 + 0.5*sin(0 + 6.2831*pt);

			// Vector between the pixel and the point
            float2 diff = neighbor + pt - f_st;

            // Distance to the point
            float dist = length(diff);

            // Keep the closer distance
            m_dist = min(m_dist, dist);
        }
    }

    // Draw the min distance (distance field)
    color += m_dist;
    return color.x;
}
//]

float2 toPolar(float2 pos)
{
    return float2(atan2(pos.y,pos.x),length(pos));
}

float2 fromPolar(float2 pos)
{
    return float2(sin(pos.x),cos(pos.x))*pos.y;
}

float3 pillar(uint qid,uint iid,float2 grid,float a, float t, float h)
{
    float3 pos = float3(hash(iid/200.),hash(iid/140.),hash(iid/120.))-.5;
    pos=normalize(pos)*2;
    pos+=pos*noise3(pos)/2;

    pos*=4;
    //t=0;

    pos*=1-length(pos.xz)/4;
    //pos/=10;
    //return pos;
    //heigth
    a=hash(iid/1000.);
    //pos+=noise3(a*351*float3(4,25,67))*1.7;

    pos = rot3(pos,noise3(pos*.8+float3(0,time.x/47,0))/6);
    pos+= noise3(pos)*.8;
    pos = rot3(pos,noise3(pos.zyx*1.6+float3(0,-time.x/31,0))/12);
    float3 pos3=pos;
    //pos=(frac(pos/7)-.5)*14;
    //pos.xz=grid*8-4;
    pos.y=0;
    pos.y-=cellnoise(pos.xz/20)*3;
    pos.y-=cellnoise(pos.xz/6);
    pos.y-=32/pow(length(pos),2);
    
    pos.y+=4;

    pos.xz=toPolar(pos.xz);
    pos.y-=noise(pos*10-float3(0,time.x/11,0))/6;
    pos.y-=cellnoise(pos.xz/2-float2(0,time.x/100))/3;

    pos.xz=fromPolar(pos.xz);
    pos.y-=noise(pos*5)/3;
    pos.y*=-1;

    if (iid%8==0)
    {
        pos.xz=noise3(iid/1334)*12+normalize(pos.xz)*14;

        pos.xz/=3;
        pos.y=0;
        pos+= noise3(pos*4)*.8;
        pos.y+=cellnoise(pos.xz/14)*sign(pos.y)*2;
        pos=rotY(pos,length(pos));
        //pos+= noise3(pos*4)*.8;
        pos.y+=4;
        pos.y+=41/length(pos)-6;
        pos.x+=hash(iid)/2;

    }else
    {
        pos+=hash3(iid)*length(pos)/82;
    }

    pos.y-=2;

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
    p.color = float4(noise3_u(float3(113,115,1)*221+177+sin(pos2*.48-111)),1)/110.+.0015;
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
