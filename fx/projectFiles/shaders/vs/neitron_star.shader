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

pos_color star(uint qid, float4 grid)
{
    float2 uv=grid.xy;
    uint cnt=7;
    uv=frac(uv*cnt);
    float b = quantize(grid.x,cnt);
    float ang=b*360.;
    float2 a = uv * PI * 2;
    a.x *= -1;
    float3 pos = 0;
    float r=18*hash(b)+1;
    
    if (qid%12!=0)
    {
        r=hash(b*3111.123)*18+4;
        pos+=7*noise3(qid*float3(1,2,3)/1000)*(pow(sin(a.y),3)+.1);
        pos.x-=r;
        float aaa=frac(a.y-PI+time.x*.01)*PI*2;
        pos=rotZ(pos,aaa);
        pos.x+=r;
    }
    else
    {
        pos+=7*noise3(qid*float3(1,2,3)/1000)*pow(sin(a.y),3);
        float d=11*hash(qid/11000.)+1111;
        pos.x+=d;
        pos=rotZ(pos,(frac(abs(a.y)+time.x*.005)-.5)*.1);
        pos.x-=d;
        pos=rotY(pos,qid);
        pos.xz*=pos.xz;
    }

    pos+=rot3(pos,ang*float3(0,1,0)+length(pos)*.10-time.x*.01);
    pos=rotY(pos,ang*PI/180.);
    float3 q= smoothstep(0,1,saturate(length(pos)/15*(1+abs(pos.y))));
    pos=lerp(normalize(pos+noise3(float3(a,qid/10000.)))*3,pos,q);
    pos*=2;
   
    pos_color p;

    p.rgba = float4(float3(1,2,3),1)*.1+.0015;

    if (qid==0)
    {
        //hilight
        p.rgba*=1;
        p.pos=transform(float3(0,0,0),grid.zw,552);
        p.sz=2;
    }
    else
    {
        p.pos = transform_unisize(pos/16,grid.zw,4.);
        p.sz=2;
        p.rgba*=11/(abs(pos.y)*2+11.1);
    } 
    
    return p;
}



pos_color sagittarius_v2 (uint qid,float div, float4 grid)
{
    pos_color outp;


    //percentage
    uint outer=612;
    uint floor_=23;
    uint star_=15;
    uint star_2=4957;
    uint big_hl=2006;

    float3 pos=0;
    
    //outp = pillars_array(qid,grid);
 //   if (qid%outer==0) outp = outer_space(0, grid, qid);
//    if (qid%floor_==0) outp = floor_space(qid,grid);
  outp = star(qid,grid);
    
    return outp;
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

VS_OUTPUT VS(uint vID : SV_VertexID,uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    float2 map[6] = { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1 };
    float4 grid = {float2(iID % gX, floor(iID / gX))/float2(gX,gY), map[vID % 6]}; 
    
    pos_color p = star(iID,grid);
    
    //density compensation
   // p.rgba/=min(pow(p.pos.w,1.1)*.1+.5,11);
    
    output.pos=p.pos;

    output.vnorm = 0;
    output.wpos = 0;
    output.vpos = 0;
    output.uv = grid.zw;
    output.id = float4(iID,0,0,0) ;
    output.rgba = p.rgba;
    output.sz1 = float4(p.sz,0,0,0);

    return output;
}
