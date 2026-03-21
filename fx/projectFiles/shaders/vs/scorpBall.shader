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

float fastInvSmoothstep(float x)
{
    return x + (x - (x * x * (3.0 - 2.0 * x)));
}

float3 torus(float u, float v, float R, float r) {
    
//u = fastInvSmoothstep(u);

    float TWO_PI = 6.283185307;
    float theta = u * TWO_PI/2; // Angle around the major ring
    float phi = v * TWO_PI;   // Angle around the minor tube
    
    //r+=pow(sin(phi)+1,12)*pow(saturate(sin(theta*32)-.7)*4,1);
    //R+=pow(saturate(sin(theta*32)-.9)*4,4)*34;
//    r+=pow(saturate(sin(phi)-.5),3)*pow(saturate(sin(theta*32)-.8),1)*84;
    r*=pow(1-2*abs(u-.5),3);

    r+=1/abs(sin(theta*20+time.x/12)+1.1)/22;
    r+=1/abs(sin(phi+time.x/12)+1.1)*1/abs(sin(theta*17+time.x/12)+1.1)/22;
    //r+=4*noise(phi*r*2+time.x/112)*noise(theta*r*2+time.x/112);    
    
    float x = (R + r * cos(phi)) * cos(theta);
    float y = (R + r * cos(phi)) * sin(theta);
    float z = r * sin(phi);

    return float3(x, y, z);
}

float3 pillar(uint qid,uint iid,float2 grid,float a, float t, float h)
{
    float3 pos = float3(hash(iid/200.),hash(iid/140.),hash(iid/120.))-.5;
    uint inStars = 1232*2;

    float3 pos3=pos;
    float r2=pow(hash(iid%15),5)*23+22;
    float l=22;
    pos=torus(grid.x+hash(iid%7)/8,grid.y,l,1.);
 
    

    //pos.x+=pow(hash(iid%15),5)*32;
    pos.y-=l*1.7;

    pos=rot3(pos,iid%15/3*float3(4,5,6)+noise3(pos/12+time.x/52)/15);

    pos=rot3(pos,iid%6);

    pos=lerp(normalize(pos3)*22,pos,pow(smoothstep(0,1,saturate(length(pos/24))),5));
    pos+=hash3(iid/12)/3;

    if (length(pos)<19)
    {
        pos=normalize(hash3(iid))*19*(1-noise(pos/25)/3);
        pos=rot3(pos,noise3(pos/3));
        pos=rot3(pos,noise3(pos/2)/5);
        //pos+=noise(pos*5);
    }

    return pos/4;

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
    
    p.color*=.5;
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

       /*  if (iid%inStars==0)
         {
              p.pos = transform_unisize(pos,grid.zw,75.5);
               p.sz=2;
               p.color*=7;
         }*/

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
    
    p.color*=-2;
    p.color/=saturate(length(pos)/3);
    p.color=max(p.color,-.03);
    }

    if (mode==1)
    {
    p.color*=.3*saturate(21/p.pos.w);
    
    p.color*=-1;
    p.color*=saturate(length(pos)/52);
    }
    
    

   // p.color/=min(pow(p.pos.w,.5)*.1+1.5,5);
    return p;
}

#include <../lib/particleVS_main2.shader>
