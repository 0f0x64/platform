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

pos_color galaxy(uint qid,float4 grid)
{
    uint stars= qid%3400;
    uint cn=7;
    float a= (qid%cn);
    a=a*PI/180.;
    a*=360./cn;
    float t=time.x*.1;
    float3 pos = pillar(grid.xy,0,t*.005,.5)/5;
    float3 pos2=pos;

        pos.y*=.4*pow(length(pos),1)*4;
        pos.xz*=9;
        
        pos=rotY(pos,length(pos)*.3+a+time.x*.005);
        pos.y+=length(pos)/7;
        pos.y-=4.5;
        

        pos+=noise3(qid*float3(1,2,3)*11.1)*2;

        pos_color p;
        p.rgba = float4(1,2,3,1)/120.+.0015;

        if (stars==0)
        {
            p.pos = transform_unisize(pos,grid.zw,1.);
            p.sz=1;
        }
        else
        {
            float sz=18;
            if (mode==1) sz=29;
            p.pos = transform_unisize(pos,grid.zw,sz);
            p.sz=2;
        }

        p.rgba = float4(noise3_u(a*float3(1,2,3)+77+sin(pos2*11.4)),1)/50.+.00015;
        if (mode!=0&&stars!=0)
        {
            p.rgba.rgb*=noise(2*rotY(pos,length(pos)-time.x*.005))*3.2+.2;

        }
        p.rgba*=float4(1,2,3,1)/2;
        
        if (stars==0)
        {
            p.rgba*=540;
            p.sz=1;
        }

        if (stars!=0&&mode==1) p.rgba*=4;

    //density compensation
  //  p.rgba/=min(pow(p.pos.w,1.1)*.1+1.5,5);
    
    return p;
}

VS_OUTPUT VS(uint vID : SV_VertexID,uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    float2 map[6] = { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1 };
    float4 grid = {float2(iID % gX, floor(iID / gX))/float2(gX,gY), map[vID % 6]}; 
    
    pos_color p = galaxy(iID,grid);
    
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
