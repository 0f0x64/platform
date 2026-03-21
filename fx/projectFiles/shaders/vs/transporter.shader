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

float3 pillar(uint qid,uint iid,float2 grid,float a, float t, float h)
{
    float3 pos = float3(hash(iid/200.),hash(iid/140.),hash(iid/120.))-.5;
    uint inStars = 1232*3;
    //pos=normalize(pos);
    float3 pos3=pos;
    //pos+=pos*noise3(pos);

    grid.y=frac(grid.y+time.x*.001* (iid%inStars==0 ? 11:1));

    //pos*=4;
    
    a=hash(iid/1000.);
    //pos+=noise3(a*351*float3(4,25,67))*1.7;

    //pos = rot3(pos,noise3(pos*.8+float3(0,t,0))/6);

   //grid.x=lerp(quantize2(grid.x-time.x/22,1),grid.x,.85);
   //grid.x=.5+.5*sin(grid.x*PI*2);
    
    //grid.y=pow(grid.y,7);
    //grid.x=1.;
    //grid.x=frac(grid.x+pos.y/100);
    pos=float3(sin(grid.x*PI*2),(grid.y-.5)*142,cos(grid.x*PI*2))*1;

    
    pos=rotY(pos,length(pos)/6);
    pos.xz+=(noise(pos*2+time.x/3)/5);

    pos+=noise3(pos+time.x/15);

    //pos.xz*=1+noise(pos.y*.2+time.x/3)*1;
    //pos.y+=noise(pos*1.3+time.x/112)*3;
        //pos.xz*=1+pow(abs(pos.y/47),3)*5;
    //pos.y=clamp(pos.y,-30,30);
    //pos.xz*=1+pow(abs(pos.y/17),13);


    //pos=rotY(pos,pos.y);

    //pos+=hash(length(pos))/2;

    pos=rot3(pos,iid%5*float3(1,22,15));

    
    //pos3=rot3(pos3,length(pos)/122);

    //pos3*=3;
    //pos3=rot3(pos3,pos3/2);
    //pos3+= noise3(pos3)*.8;
    //pos3*=1+sin(time.x*5)/8;
    //pos3=rot3(pos3,time.xxx/2*float3(6,4,5)/3);
    //pos=lerp(normalize(pos)*12,pos,smoothstep(0,1,saturate(length(pos/44))));
    pos=lerp(normalize(pos)*8,pos,saturate(length(pos/44)));

    pos=lerp(pos,normalize(pos3)*42,pow(saturate(length(pos/109)),4));
    //pos=lerp(pos,normalize(pos3)*22,pow(smoothstep(0,1,saturate(length(pos/114))),2));
    //pos=lerp(pos,normalize(pos),saturate(length(pos/72)));
    //pos*=1.6;
    
    //pos = rot3(pos,noise3(pos.zyx*1.6+float3(0,-t,0))/12);
    
    //pos+=(frac(pos/7)-.5)*4;


    //pos*=.9;
/*    float dst=3;
    for (int i=0;i<32;i++)
    {
        float4 j=(i+1)*float4(11,12,13,14)+time.x*.03;
        float3 hole=float3(sin(j.x),cos(j.y),sin(j.z)*cos(j.w));
        hole=normalize(hole)*(12.3+5*sin((pos)+time.x*.01));
        dst=3/(distance(pos,hole));
        pos-=normalize(hole-pos)*pow(dst,3);
       // pos=rot3(pos,saturate((hole*pos)/.1)*12);
    }*/
  
    //pos+= noise3(pos/6)*.8;

    
    return pos/4;
}



pos_color CalcParticles(uint qid,uint iid,float4 grid)
{
     qid *= skipper;
     float t=time.x*.004;
     uint inStars = 1232*3;
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
    p.color = float4(noise3_u(111+float3(113,11,111)*221+177),1)/11.+.015;
    
    p.color*=.1;
    p.color*=base_color*(pow(length(pos)/16,4)+.1);
    //p.color*=1+sin(grid.x*PI*8);
    p.color=lerp(p.color,p.color.bgra,grid.y);
    //p.color=lerp(p.color,base_color/144,1-saturate(pow(length(pos)/6,9)));


    if (mode==1)
    {
        float s=hash(iid)*33+11;
        //s*=1.5;
        p.pos=transform(pos,grid.zw,s);
        p.color*=11.3;
        p.sz=172;
    }
    else
    {
        p.pos = transform(pos,grid.zw,.75);
       //p.color=-noise(pos*.3+12)*.04+.02;;
       // p.color +=min(0,sign(1./noise(-pos2*.2-2.6)))/91.;
         p.sz=2;
         p.color*=1.2;

         if (iid%inStars==0)
         {
              p.pos = transform_unisize(pos,grid.zw,5.5);
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
    p.color/=min(pow(p.pos.w,1.1)*.1+1.5,5)/4;
    return p;
}

#include <../lib/particleVS_main2.shader>
