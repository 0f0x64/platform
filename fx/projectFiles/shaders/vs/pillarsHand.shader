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

float toRad(float a)
{
    return a*PI/180.;
}

#include <../lib/hand_data.shader>

static float3 ofs=float3(.25,-1.2,0);

float3 pillar(uint qid,uint iid,float2 grid,float a, float t, float h)
{


    float3 pos = hand_vertex[iid%428];
    pos.x+=3450;
    pos.z-=1330;
    pos/=15;
    pos+=normalize(hash3(iid%1600))*.5;
    pos.y-=7;
    //pos/=10;
    //return pos;
    //heigth
    a=hash(iid/1000.);
    pos+=noise3(a*351*float3(4,25,67))*1.7;

    pos = rot3(pos,noise3(pos*1.5+float3(0,t,0))/6);
    pos+= noise3(pos)*.8;
    pos = rot3(pos,noise3(pos.zyx*1.6+float3(0,-t,0))/12);


    float q=length(pos+ofs.xyz)/4.;
    q=1-saturate(q);
    q=smoothstep(0,1,q);
    q=smoothstep(0,1,q);
    q=smoothstep(0,1,q);
    pos+=ofs;
    //pos+=q*noise3(pos*2)*.7;
    pos=rotX(pos,toRad(-26));
    pos=rotZ(pos,-q*5-t*q*.005);
    pos=rotZ(pos,-t*sign(q)/3);
    pos=lerp(pos,pos*float3(1,1,0),q);

    pos=rotX(pos,toRad(26));
    pos-=ofs;

    //pos*=1+noise3(pos*1.6+1111/(a+1)+t)*.24;
    //pos = rot3(pos,2.2/(pow(pos,14)+1));
    //pos*=1+noise3(pos+1111/(a+1)+t)*.3;
    //pos*=1+noise3(pos*3.5)/5;
 //pos=rotY(pos,pow(max(-pos.y/1,0),2));
 //pos.xz+=noise3(pos).y*pow(max(-pos.y/1,0),2)/2;
    return pos;
}



pos_color CalcParticles(uint qid,uint iid,float4 grid)
{
     qid *= skipper;
     float t=time.x*.07;
     uint inStars = 10000;
     if (mode==1||iid%inStars==0)
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
    float3 pos = pillar(qid,iid,grid.xy,0,t,h);
    float3 pos2=pos;
    
    
    //scatter
    
    //color
    pos_color p;
    p.color = float4(noise3_u(float3(113,115,1)*221+177+sin(pos2*.48)),1)/30.+.0015;
    
       pos=rotX(pos,toRad(65));
       pos.y-=.5;
       pos.z+=1.2;
    if (mode==1)
    {
        p.pos=transform(pos,grid.zw,22);
        p.color*=11;
        p.sz=172;
    }
    else
    {
        p.color*=2;
        p.pos = transform_unisize(pos,grid.zw,1.5);
       //p.color=-noise(pos*.3+12)*.04+.02;;
       // p.color +=min(0,sign(1./noise(-pos2*.2-2.6)))/91.;
         p.sz=1;

         if (iid%inStars==0)
         {
              p.pos = transform_unisize(pos,grid.zw,51.5);
               p.sz=2;
               p.color*=15;
         }else
         {
             float q=length(pos2.xyz+ofs.xyz)/4;
             if (q<.7&&iid%1400==0)
             {
              p.pos = transform_unisize(pos,grid.zw,51.5);
               p.sz=2;
               p.color*=15;

             }
         }
    }
    
   float q=length(pos2.xyz+ofs.xyz)/4;
    //q=saturate(q);
    p.color*=saturate(pow(abs(length(pos2+ofs)-q-3),2));
    p.color+=saturate(1-q)*.1*lerp(float4(3,2,2,0),float4(0,0,4,0),q*2-.4);
   p.color*=.2;
   
    //density compensation
    //p.color/=min(pow(p.pos.w,1.1)*.21+.5,11);
    return p;
}

#include <../lib/particleVS_main2.shader>
