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

pos_color CalcParticles(uint qid, float4 grid)
{
   // grid.x=frac(grid.x);
    float2 uv=grid.xy;
    uint cnt=42;
    uv=frac(uv*cnt);
    float b = quantize(grid.x,cnt);
    float ang=b*360.;
    float2 a = uv * PI * 2;
    a.x *= -1;
    float3 pos = 0;
    float r=hash(b*3111.123)*6+7;
    r/=2;
    float central = qid%15;
    

        pos.x-=r;
        float aaa=frac(a.y/PI+time.x*.0051*(qid%3+1)+qid/900.)*PI*2;
        //aaa+=sin(aaa*6+time.x+b*2222)*.1;
        //float aaa=(a.y+qid/100.);
        if (central!=0)
        {
            pos=rotZ(pos,aaa);
        } else{
            pos=rotZ(pos,aaa/110);
            pos.y=(aaa-PI)*10;
            pos.xz*=1+pow(abs(aaa-PI),7)*.0001;

        }
        pos.x+=r;

        if(central!=0)
        {
            pos+=rot3(pos,ang*float3(1,2,3)+(length(pos)*.20)-time.x*.03);
            //pos+=rot3(pos,ang*float3(1,2,3)*b/(length(pos)*.20+5)-time.x*.03);
            //pos+=rot3(pos,.1*ang/(length(pos)*2)-time.x/30);

        }

    pos=rotY(pos,ang*PI/180.);
        
    float3 q= smoothstep(0,1,saturate(length(pos)/5*(1+abs(pos.y))));
    pos=lerp(normalize(pos+noise3(float3(a,qid/1000.)))*3,pos,q);
    pos*=2;
    pos+=(pos/12)/((noise3(qid*float3(1,2,3)/68.+222)+.6)+.1);
    //pos+=length(pos/22)*(noise3(grid.xyx*float3(1,2,3)*75+222)+.6);

    if(central==0)
    {
        pos=rotX(pos,.1);
    }

    pos_color p;

    p.rgba = float4(float3(1,2,3),1)*.015+.0015;

    if (qid==0)
    {
        //central hilight
        p.rgba*=4.8;
        p.pos=transform(float3(0,0,0),grid.zw,552);
        p.sz=2;
    }
    else
    {
        p.pos = transform_unisize(pos/16,grid.zw,3.);
        p.sz=2;
        
       // p.rgba*=11/(abs(pos.y)*2+11.1);
    } 
    
    return p;
}

float tri(float x)
{
    return lerp(frac(x),1-frac(x),floor(frac(x/2)*2));
}

float3 double_star(float2 grid,float a, float t, float h,uint qid,uint star2)
{
    //float3 pos = float3(0,(grid.y-.5)*2,0);
    //float3 pos = float3(0,pow(cos((grid.y-.5)*PI*2),1),0);
    //grid.y+=sin(time.x*.01)/2;
    float y1=(grid.y-.5)*2;
    float y2=pow((smooth(grid.y)-.5)*2,3);
    float y=lerp(y1,y2,0.);
    float3 pos = float3(0,y,0);
    
    //pos=rot3(pos,grid.xyx*222);
    pos.z=0;
    float scale=sin(abs(grid.y-.5)*PI*2)/9+.1;
    float scale2=scale;
    scale2=pow(scale,3)*72;

    float tm=time.x;
        float dir=sign(y1)*((qid%2)-.5);
    //
    if (qid%star2==0)
    {
        pos.y=frac(pos.y-time.x*.001*dir*sign(y1))*sign(pos.y);
        pos-=noise3(qid/11100.)*scale2*1.2;
    }


    pos = rotZ(pos,pos.y*sign(pos.y)*7.85);

    if (qid%9==0&&qid%star2!=0) pos+=normalize(pos)*noise(pos*25);

    pos.x+=(qid%2)*2;
    pos=pos.yzx;
    pos*=4;
    pos.z-=4;
    //pos+=noise3(grid.xyy*112+time.x*.1)*2.*scale2;
    //float3 g1=float3(qid/11112.,qid/11122.,qid/11412.)*2;
    float3 g1 =noise3(pos*4)*pow(length(pos),1)*.1-.5;
    float y3=grid.y;
    float3 g=float3(y3/12.,y3/22.,y3/14.)*5134+g1;
    
    if (qid%star2!=0)
    {
        //pos+=noise3(pos*.8+dir*sign(y1)*tm.x*.00004+qid/1111.)*.07;
        pos+=noise3(g*.2+tm.x*.05*dir)*scale2*1.51;
        pos+=noise3(grid.xyy*8+dir*tm.x*.0001*+qid/41111.)*2.3*scale2;
        
    }

    return pos*2.5;
}

#include <../lib/particleVS_main.shader> 
