#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>
#include <../lib/utils.shader>

cbuffer params : register(b0)
{
    float4 pos1;
    float4 pos2;
}

float toRad(float a)
{
    return a*PI/180.;
}

float quantize2(float x, float q)
{
    return floor(x*q)/q;
}

pos_color CalcParticles(uint qid,uint iid,float4 grid)
{
     qid *= skipper;
     iid *= skipper;
     float t=time.x*.004;
     uint inStars = 1232*1213;
     if (mode==1||iid%inStars==0)
     {
        t=0;
     }

    //pillars instances


    //calc
    float3 pos = pillar3(qid,iid,grid.xy,0,t,0);
    float3 pos2=pos;
    
    //scatter
    
    //color
    pos_color p;
    p.color.a=1;
    p.color.rgb = noise3_u(pos*14*float3(12,55,112))/31+float3(1,3,5)/52;
    
    //p.color*=.5;
//    p.color*=base_color*(pow(length(pos)/16,4)+.1);
    //p.color*=1+sin(grid.x*PI*8);
    p.color=lerp(p.color,p.color.bgra,sin(length(pos)));
//    p.color=lerp(p.color,base_color/144,1-saturate(pow(length(pos)/6,11)));
//pos+=noise(pos/12)*12-6;
//pos*=.75;
// 1. Извлекаем векторы осей из первых трех строк матрицы view
    

     
    if (mode==1)
    {
        float s=hash(iid)*33+11;
        s=noise(iid)*62+11;
        //s*=1.5;
        p.pos=transform(pos,grid.zw,s);
        p.color*=.7;
        p.sz=172;
    }
    else
    {
        p.pos = transform(pos,grid.zw,1.1);
       //p.color=-noise(pos*.3+12)*.04+.02;;
       // p.color +=min(0,sign(1./noise(-pos2*.2-2.6)))/91.;
         p.sz=2;
         p.color*=2;

/*         if (iid%inStars==0)
         {
              p.pos = transform_unisize(pos,grid.zw,75.5);
               p.sz=2;
               p.color*=2;
         } 
         */
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
    p.color*=1*saturate(p.pos.w/11);
    //p.color*=0;

    }

    if (mode==1)
    {
    //p.color*=.3*saturate(21/p.pos.w);
    //p.color*=1*saturate(p.pos.w/1);
    //p.color=.02;
    }
    
   // p.color/=min(pow(p.pos.w,.5)*.1+1.5,5);
    return p;
}

#include <../lib/particleVS_main2.shader>
