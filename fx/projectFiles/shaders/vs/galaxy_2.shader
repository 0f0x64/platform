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

    pos*=1+noise3(pos*4.6+1111/(a+1))*1.6;
    pos.y*=1.8;
   //pos*=1+noise3(pos*1.6+1111/(a+1)+t);
    pos = rot3(pos,2.2/(pow(pos,14)+1)+noise3(pos/2)*2);
    pos.y*=1.5;
    //pos*=1+noise3(pos+1111/(a+1)+t);
    //pos*=1+noise3(pos*3.5)/3;
    pos*=2;
    //pos = rot3(pos,noise3(pos/3)*4+112);
    return pos;
}

pos_color CalcParticles(uint qid,float4 grid)
{
    uint stars= qid%7345;
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
        pos.y-=pow(length(pos.xz),.34)*7;
        
        pos.y/=2;
        pos.y+=1.5;
        //pos.xz+=1*pos.y*(qid%2-.5);
        pos.y*=-((qid%2)-.5)*2;

        pos+=noise3(pos-time.x/42)*saturate(length(pos)/4);
        pos=rot3(pos,noise3(pos-time.x/72)/6);
        pos=rotY(pos,length(pos)*.3*(qid%2-.5)*2+a-time.x*.005*(qid%2-.5)*2);
        
        pos=rotX(pos,1/pow(length(pos),2));
        pos=rotZ(pos,14/pow(length(pos),4));

        //pos+=noise3(qid*float3(1,2,3)*11.1)*2;

        pos_color p;
        float col_i= pos.y/22+.5;
        float3 col_g=lerp(base_color.rgb,base_color.bgr,col_i);
        p.color = float4(noise3_u(a*col_g+77+sin(pos2*11.4)),1)/50.+.00015;

        if (stars==0)
        {
            p.pos = transform_unisize(pos,grid.zw,42.);
            p.sz=1;
        }
        else
        {
            if (mode==0)
            {
                float sz=1.1;
      
                p.pos = transform_unisize(pos,grid.zw,sz);
                p.color*=float4(col_g,1)*1;
                p.sz=1;
            } 
            else
            {
                float sz=40;
                p.pos = transform(pos,grid.zw,sz);
                p.sz=2;
                p.color*=float4(col_g,1)*pow(length(pos),3.)/4823;
            }
        }

        if (mode!=0&&stars!=0)
        {
           // p.color.rgb*=noise(1*rotY(pos,length(pos)-time.x*.005))*3.2+.012;

        }

        
        if (stars==0&&mode==0)
        {
            p.color*=34;
            p.sz=2;
        }

        if (qid==0)
        {
        p.pos=0;
        p.pos = transform(0,grid.zw,542);
        p.pos.y/=42;
        p.color*=float4(1,5,11,1)/3+pow(1/(length(pos)),2);
        }else
        {
            p.color*=1+pow(2/length(pos),2);
            
        }

        //if (stars!=0&&mode==1) p.color*=2.5;
        //if (stars!=0&&mode==0) p.color*=1.5;
 

    //density compensation
  //  p.color/=min(pow(p.pos.w,1.1)*.1+1.5,5);
//  p.color=pos.y/5+.5;  
  //p.color/=100;  
  //p.color=saturate(p.color);
    return p;
} 

#include <../lib/particleVS_main.shader>
