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
    uint stars= qid%340;
    uint cn=7;
    float a= (qid%cn);
    a=a*PI/180.;
    a*=360./cn;
    float t=time.x*.1;
    float3 pos = pillar(grid.xy,0,t*.005,.5)/5;
    float3 pos2=pos;

        pos.y*=0;
        pos.xz*=9;
        pos.x*=.6;
        pos=rotY(pos,length(pos)*.3+a-time.x*.005);
        pos.y+=length(pos)/7;
        pos.y-=4.5;
        

        pos+=noise3(qid*float3(1,2,3)*11.1)*2;

        pos_color p;
        p.color = float4(1,2,3,1)/120.+.0015;

        if (stars==0)
        {
            p.pos = transform_unisize(pos,grid.zw,1.);
            p.sz=1;
        }
        else
        {
            if (mode==0)
            {
            float sz=28;
      
            p.pos = transform_unisize(pos,grid.zw,sz);
            p.sz=2;
            } 
            else
            {
                float sz=4;
                p.pos = transform(pos,grid.zw,sz);
                p.sz=2;
            }
        }

        p.color = float4(noise3_u(a*base_color.rgb+77+sin(pos2*11.4)),1)/50.+.00015;
        if (mode!=0&&stars!=0)
        {
           // p.color.rgb*=noise(1*rotY(pos,length(pos)-time.x*.005))*3.2+.012;

        }
        p.color*=base_color/2;
        
        if (stars==0&&mode==0)
        {
            p.color*=540;
            p.sz=1;
        }

        if (stars!=0&&mode==1) p.color*=2.5;
        if (stars!=0&&mode==0) p.color*=1.5;
 

    //density compensation
  //  p.color/=min(pow(p.pos.w,1.1)*.1+1.5,5);
    
    return p;
} 

#include <../lib/particleVS_main.shader>
