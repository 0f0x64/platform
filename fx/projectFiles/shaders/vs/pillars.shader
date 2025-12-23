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

pos_color pillars_array(uint qid,float4 grid)
{
     qid *= skipper;
     float t=time.x*.01;

     if (mode==1)
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
    float3 pos = pillar(grid.xy,a,t,h);
    float3 pos2=pos;
    
    //scatter
    pos.x+=17;
    pos = rot3(pos,float3(-.9,0,.4));
    pos.y/=1.3;
    pos.y-=h*10-8;
    if (qid%cn) pos=rotY(pos,a);
    pos*=-0.5;
    pos.y*=1.4;
    pos.y-=h;
    
    //color
    pos_color p;
    p.rgba = float4(noise3_u(a*float3(13,15,12)*221+77+sin(pos2*1.4)),1)/30.+.0015;

    if (mode==1)
    {
        p.pos=transform(pos,grid.zw,92);
        p.rgba*=2;
        p.sz=172;
    }
    else
    {
        p.pos = transform_unisize(pos,grid.zw,1.5);
    //   p.rgba+=-noise(pos*.12*float3(1,2,3)+2)*.1;;
       // p.rgba +=min(0,sign(1./noise(-pos2*.2-2.6)))/91.;
         p.sz=1;

    }
   
    //density compensation
    p.rgba/=min(pow(p.pos.w,1.1)*.1+.5,11);
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
    
    outp = pillars_array(qid,grid);
 //   if (qid%outer==0) outp = outer_space(0, grid, qid);
//    if (qid%floor_==0) outp = floor_space(qid,grid);
 //   if (qid%star_==0) outp = star(qid,grid);
    
    return outp;
}

VS_OUTPUT VS(uint vID : SV_VertexID,uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    float2 map[6] = { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1 };
    float4 grid = {float2(iID % gX, floor(iID / gX))/float2(gX,gY), map[vID % 6]}; 
    
    pos_color p = pillars_array(iID,grid);
    
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
