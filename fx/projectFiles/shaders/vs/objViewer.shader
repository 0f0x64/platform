#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>
#include <../lib/utils.shader>


Texture2D positions : register(t0);
Texture2D normals : register(t1);
SamplerState sam1 : register(s0);

cbuffer params : register(b0)
{
    float4x4 model;
    int gX;
    int gY;
}

float3 rotX(float3 pos,float a)
{
    float3x3 m =
    {
        1, 0,       0,
        0, cos(a), -sin(a),
        0, sin(a), cos(a)
    };
    pos = mul(pos, m);
    return pos;
}

float3 rotY(float3 pos, float a)
{
    float3x3 m =
    {
        cos(a), 0, sin(a),
        0, 1, 0,
        -sin(a), 0, cos(a)
    };
    pos = mul(pos, m);
    return pos;
}

float3 rotZ(float3 pos, float a)
{
    float3x3 m =
    {
        cos(a), -sin(a),0,
        sin(a), cos(a), 0,
        0, 0, 1
    };
    pos = mul(pos, m);
    return pos;
}

float hash( float n ) {
        return frac(sin(n)*43758.5453);
    }
     
    float noise( float3 x ) {
        // The noise function returns a value in the range -1.0f -> 1.0f
        float3 p = floor(x);
        float3 f = frac(x);
     
        f = f*f*(3.0-2.0*f);
        float n = p.x + p.y*57.0 + 113.0*p.z;
     
        float a= lerp(lerp(lerp( hash(n+0.0), hash(n+1.0),f.x),
               lerp( hash(n+57.0), hash(n+58.0),f.x),f.y),
               lerp(lerp( hash(n+113.0), hash(n+114.0),f.x),
               lerp( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
               return a-.5;
    }

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    float4 grid = getGridP(vID, 1, int2(gX,gY));
    float2 uv = grid.xy;
    
    float4 pos = float4(positions.SampleLevel(sam1, uv, 0).xyz,1);
    pos*=12.;
    pos.x+=noise(pos.xyz*.125+time.x*.003)*35;
    pos.y+=noise(pos.xyz*.131+time.x*.0014)*35;
    pos.z+=noise(pos.xyz*.153+time.x*.005)*35;

    
    pos = mul(pos, model);

    pos.z=1;
    //pos.xy*=3;

    pos.xyz=rotX(pos,pos.x*.005*(time.y*.01));
    pos.xyz=rotY(pos,pos.y*.003*(time.y*.01));
    pos.xyz=rotZ(pos,pos.z*.002*(time.y*.01));

    pos.x-=noise(pos.xyz*.0025)*75;
    pos.y-=noise(pos.xyz*.0031)*75;
    pos.z-=noise(pos.xyz*.0053)*75;
   
    

    float4 normal = float4(normals.SampleLevel(sam1, uv, 0).xyz, 1);
    normal = mul(normal, transpose(model));
    //---
  // pos.xyz = rotY(pos.xyz,time.x*.1);
    //normal.xyz = rotY(normal.xyz,time.x*.1);


    output.vnorm = normal;
    
    output.wpos = float4(pos.xyz, 0);
    output.vpos = mul(float4(pos.xyz, 1), view[0]);

    output.pos = mul(pos, view[0]);
    output.pos.xy+=(grid.zw-.5)*.5;
    output.pos = mul(output.pos, proj[0]);

   // output.pos.x*= aspect.x;
    output.uv = grid.zw;
    output.id = float4(floor(vID/6),0,0,0) ;
    return output;
}
