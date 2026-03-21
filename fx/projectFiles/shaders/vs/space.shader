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

float4 transform2(float3 pos,float2 grid, float size)
{
    float2 uv = grid-.5;
    float2 scale = float2(proj[0]._m00,proj[0]._m11);
    float4 posT;
    posT.w=1;
    posT.xyz = mul(pos/1.2, (float3x3)view[0]);

        float4 pt = mul(posT, proj[0]);
        float2 sz = uv*.002*(posT.z/posT.w)*size;
        sz = uv*size*.1;
        posT.xy+=sz;
        posT = mul(posT, proj[0]);

        return posT;
}

float4 transform_unisize2(float3 pos,float2 grid,float size)
{
    float2 uv = grid-.5;
    float2 scale = float2(proj[0]._m00,proj[0]._m11);
    float4 posT;
    posT.w=1;
    posT.xyz = mul(pos, (float3x3)view[0]);
        float4 pt = mul(posT, proj[0]);
        float2 sz = uv*.002*(posT.z/posT.w)*size;

        sz*=normalize(scale)*2;
        posT = mul(posT, proj[0]);
        posT.xy+=sz;

        return posT;
}

pos_color CalcParticles(uint qid, float4 grid)
{
    qid *= skipper;

    float3 pos=shp(grid.xy);
    pos+=.7*rot3(pos,31/pos+.1*noise3(pos*3));
    pos=lerp(normalize(pos)*33,pos,.45);;
    


    pos_color p;

    p.color = float4(float3(3,6,9),1)*.051+.0015;


    if (mode==1)
    {
        //hilight
        p.pos=transform2(pos,grid.zw,302);
        p.color/=5;
        p.sz=2;
    }
    else
    {
        p.pos = transform_unisize2(pos,grid.zw,1.);
        p.sz=1;
        p.color*=3.8*(hash(qid))+.01;

        if (qid%8==0)
        {
            p.pos=transform2(pos,grid.zw,63);
            p.color*=.051;
            p.sz=2;
        }
    } 


    
    return p;
}

#include <../lib/particleVS_main.shader>
