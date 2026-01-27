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

float tri(float x)
{
    return lerp(frac(x),1-frac(x),floor(frac(x/2)*2));
}

pos_color CalcParticles(uint qid,uint iid,float4 grid)
{
uint star2=110;

     qid *= skipper;
     float t=time.x*.004;
     uint inStars = 10000;
     if (mode==1||iid%inStars==0)
     {
        t=0;
     }

    float y1=(grid.y-.5)*2.;
    float y2=pow((smoothf(grid.y)-.5)*2.,3.);
    float y=lerp(y1,y2,0.);
    float3 pos = float3(0,y,0);

    pos.z=0;
    float scale=sin(abs(grid.y-.5)*PI*2)/9+.1;
    float scale2=scale;
    scale2=pow(scale,3)*72;

    float tm=time.x;
        float dir=sign(y1)*((iid%2)-.5);
    //
    /*if (iid%star2==0)
    {
        pos.y=frac(pos.y-time.x*.001*dir*sign(y1))*sign(pos.y);
        pos-=noise3(iid/11100.)*scale2*1.2;
    }*/

    pos = rotZ(pos,pos.y*sign(pos.y)*7.85);

    //if (iid%9==0&&iid%star2!=0) pos+=normalize(pos)*noise(pos*25);

    pos.x+=(iid%2)*2;
    pos=pos.yzx;
    pos*=4;
    pos.z-=4;
    float3 g1 =noise3(pos*4)*pow(length(pos),1)*.1-.5;
    float y3=grid.y;
    float3 g=float3(y3/12.,y3/22.,y3/14.)*5134+g1;
    
    //if (iid%star2!=0)
    {
        //pos+=noise3((hash(iid/2112.)-.5)*float3(1,2,3)*12)*.2;
        pos+=noise3(g*.2+tm.x*.15*dir)*scale2*.51;
        pos+=noise3(grid.xyy*8+dir*tm.x*.0000*+iid/41111.)*2.3*scale2;
      //pos+=noise3(pos*11.2)*.151;
    }

    pos_color p;

    {
    float dir=(iid%2-.5);
    float v=15;
    float q= .5+lerp(grid.y,1-grid.y,iid%2)/2;
    q=.5+dir*(lerp(saturate((1-grid.y)*v),saturate(grid.y*v),iid%2));
    p.color=.0125*float4(lerp(float3(1,0.2,0.1),float3(0.1,0.2,1),q)*(noise3(pos*1.1)*.5+.6),1);
    }


    if (mode==0)
    {
        float sz=1.2;
        p.sz=1;
       
    
        p.pos = transform_unisize(pos,grid.zw,sz);
        
        p.color*=17;
    } 
    else
    {
        float sz=14;
        p.pos = transform(pos,grid.zw,sz);
        p.sz=2;
        p.color*=3;
    }


    return p;
}

#include <../lib/particleVS_main2.shader>
