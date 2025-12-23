#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>
#include <../lib/utils.shader>

cbuffer params : register(b0)
{
    float4x4 model;
    int gX;
    int gY;
    float glow_p;
    int skipper;
}

pos_color outer_space(uint qid, float4 grid)
{
    qid *= skipper;

    float3 pos=shp(grid.xy);
    pos+=.7*rot3(pos,31/pos+.1*noise3(pos*3));
    pos=lerp(normalize(pos)*33,pos,.45);;
    
    pos_color p;

    p.rgba = float4(float3(7,8,9),1)*.051+.0015;

    if (glow_p==0)
    {
        //hilight
        p.pos=transform(pos,grid.zw,102);
        p.rgba/=14;
        p.sz=2;
    }
    else
    {
        p.pos = transform_unisize(pos,grid.zw,1.);
        p.sz=1;
        p.rgba*=3.8*(hash(qid))+.01;
    } 
    
    return p;
}



VS_OUTPUT VS(uint vID : SV_VertexID,uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    float2 map[6] = { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1 };
    float4 grid = {float2(iID % gX, floor(iID / gX))/float2(gX,gY), map[vID % 6]}; 
    
    pos_color p = outer_space(iID,grid);
    
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
