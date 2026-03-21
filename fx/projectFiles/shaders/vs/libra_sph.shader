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

float3 n3(uint iid)
{
    float q=1;
    float a=11234.123/q;
    float b=12345.234/q;
    float c=13456.345/q;
    float v1 = hash(iid/a);
    float v2 = hash(iid/b);
    float v3 = hash(iid/c);
    return float3(v1,v2,v3);
}

float quantize2(float x, float q)
{
    return floor(x*q)/q;
}

float2 gety(float y, int branch_count,float power)
{
    float s=2;
    float y2=pow(y,power);
    float y0=pow(s,floor(y2*branch_count));
    float y1=pow(s,floor(y2*branch_count)+1);
    return float2(y0,y1);
}

float3 pillar(uint qid,uint iid,float2 grid,float a, float t, float h)
{

float3 pos=0;

if (grid.y>.35)
{
    if (iid%5==0) {
        pos=float3(sin(grid.x*PI*2),(grid.y-.5)*2,cos(grid.x*PI*2))/13;
        pos.y=-0.065;
    }
    else
    {
    float g=(grid.y-.35)*2.1;
    grid.x=quantize2(grid.x,3)+.1*sign(iid%2-.5);
    pos=float3(sin(grid.x*PI*2),(1-grid.y)*8,cos(grid.x*PI*2))/18*float3(g,1,g);
    pos.y-=.065;
    }
    pos+=noise3(pos*22234.4+1222+time.x/22)*saturate(pos.y)/4;
    pos+=noise3(hash3(iid)*12.2)*.01;
}else
{
    grid.x+=sin(grid.x*2*PI-time.x/10)/4;
    pos=float3(sin(grid.x*PI*2),(grid.y-.5)*2,cos(grid.x*PI*2));
    pos.xz*=.5-abs(grid.y-.5);
    pos=normalize(pos);
    pos/=10;
    pos+=noise3(pos*614.4+222)/100;
    pos=rotY(pos,length(pos.xz)*222);

}


pos.x+=((iid%2)-.5)/1.9; 
pos.y-=.05;



    return pos*18;
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


    //calc
    float3 pos = pillar(qid,iid,grid.xy,0,t,0);
    pos.y+=iid%2==0 ? sin(time.x/50)/4 :-sin(time.x/50)/4;
    float3 pos2=pos;
    
    //scatter
    
    //color
    pos_color p;
    p.color = float4(noise3_u(pos2/.1/3)*float3(7.4,2.5,0.),1)/14.+.015;

    p.color/=3;

    if (mode==1)
    {
        p.pos=transform(pos,grid.zw,22);
        p.color*=3;
        p.color*=pow(pos.y+2,2)/18;
        p.sz=172;
    }
    else
    {
        p.pos = transform_unisize(pos,grid.zw,1.5);
         p.sz=1;

    }
 
   
    //density compensation
    //p.color/=min(pow(p.pos.w,1.1)*.21+.5,11);
    return p;
}

#include <../lib/particleVS_main2.shader>
