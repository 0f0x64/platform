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
    qid *= skipper;

    float3 pos=shp(grid.xy);
    pos+=.7*rot3(pos,31/pos+.1*noise3(pos*3));
    pos=lerp(normalize(pos)*33,pos,.45);;
    
    pos_color p;

    p.rgba = float4(float3(3,6,9),1)*.051+.0015;


    if (mode==1)
    {
        //hilight
        p.pos=transform(pos,grid.zw,302);
        p.rgba/=5;
        p.sz=2;
    }
    else
    {
        p.pos = transform_unisize(pos,grid.zw,1.);
        p.sz=1;
        p.rgba*=3.8*(hash(qid))+.01;

        if (qid%8==0)
        {
            p.pos=transform(pos,grid.zw,63);
            p.rgba*=.051;
            p.sz=2;
        }
    } 


    
    return p;
}

#include <../lib/particleVS_main.shader>
