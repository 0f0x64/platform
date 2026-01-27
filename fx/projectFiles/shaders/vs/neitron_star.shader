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

    p.color = float4(float3(1,2,3),1)*.015+.0015;

    if (qid==0)
    {
        //central hilight
        p.color*=4.8;
        p.pos=transform(float3(0,0,0),grid.zw,552);
        p.sz=2;
    }
    else
    {
        p.pos = transform_unisize(pos/16,grid.zw,3.);
        p.sz=2;
        
       // p.color*=11/(abs(pos.y)*2+11.1);
    } 
    
    return p;
}



#include <../lib/particleVS_main.shader> 
