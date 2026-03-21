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
    float theta = u * TWO_PI; // Angle around the major ring
    float phi = v * TWO_PI;   // Angle around the minor tube

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
    pos=torus(grid.x,grid.y,22,.1);
    
    //pos+=hash3(iid);
    pos.y-=42;
    //pos=rot3(pos,time.x/1011);
    //pos=rot3(pos,iid%9);
    //pos=rot3(pos,iid%11/3*float3(4,5,6));
    pos=rot3(pos,iid%15/3*float3(4,5,6));
    
    //a=hash(iid/1000.);
    //grid.y=pow(grid.y,.6);
    //pos=float3(sin(grid.x*PI*2)/4,(grid.y-.5)*72,cos(grid.x*PI*2)/4)*.6;
    //pos/=12/(pos+.0);
    //pos.x=rotZ(pos,pos.y);
    //pos+=noise3(pos+time.x/15);

    //pos+=((iid%16)-8)/9;
  //  pos=rot3(pos,iid%16);

    //pos=lerp(normalize(pos3)*10,pos,pow(smoothstep(0,1,saturate(length(pos/33))),3));
    //pos=lerp(normalize(pos3)*10,pos,step(.3,saturate(length(pos/33.5))));
pos=lerp(normalize(pos3)*10,pos*1.6,step(.3,saturate(length(pos/33.5))));
    pos+=noise3(pos*12);
    pos+=noise3(pos*2)*46;
    //pos/=1.1;
    pos=rot3(pos,noise3(pos/10+1122));
    pos=rot3(pos,noise3(pos/5+1122)/5);
    pos+=noise3(pos/4+112)*6;
    //pos/=1.2;
     //   pos.xz*=1+pow(abs(pos.y),.05);
    //pos=clamp(pos,-30,30);
    pos=normalize(pos)*16+pos/2;


    

    

  //  pos=rot3(pos,iid%25*float3(61,12,25));

    
  //  pos.y-=pow(max(length(pos.xz)-1,0),.1);
    //pos=rotX(pos,length(pos));
    //pos.y*=2/(sin(length(pos.xz)/16)+1.3);
    //pos=rot3(pos,1/pos);
    //pos.y+=sin(length(pos.xz)/9)*5;
    //pos.xz*=1+1/(length(pos.xz)/8);
    //pos+=.1/(noise3(pos)*length(pos)/3+.01);


    
    //pos3=rot3(pos3,length(pos)/122);

    //pos3*=3;
    //pos3=rot3(pos3,pos3/2);
    //pos3+= noise3(pos3)*.8;
    //pos3*=1+sin(time.x*5)/8;
    //pos3=rot3(pos3,time.xxx/2*float3(6,4,5)/3);
    //pos=lerp(normalize(pos)*12,pos,smoothstep(0,1,saturate(length(pos/44))));
  //  pos=lerp(normalize(pos)*8,pos,saturate(length(pos/44)));

    //pos=lerp(pos,normalize(pos3)*42,pow(saturate(length(pos/109)),4));
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
  
    //pos+= noise3(pos*3);
    //pos+=rot3(pos,pos/14);


    
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
