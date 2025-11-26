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

    float3 noise3(float3 p)
    {
        
        return float3(noise(p.x),noise(p.y),noise(p.z));
    }

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    float4 grid = getGridP(vID, 1, int2(gX,gY));
    float2 uv = grid.xy;
    
    float4 pos = float4(positions.SampleLevel(sam1, uv, 0).xyz,1);
    pos.xyz*=12.;
    pos-=.8;
    float qid = floor(vID/6);

    pos.xyz = noise3(qid*float3(.0001,.0002,.0003)*5);
    pos.xyz = noise3(pos.xyz*66+time.x*.0)*10;

    pos.xyz=lerp(pos.xyz,noise(pos.xyz*1.2)/(pos.xyz*4),noise(pos*2.3));

    pos.xyz=rotX(pos.xyz,2.22*noise(pos.xyz)+time.y*.0001);
    pos.xyz=rotY(pos.xyz,.33*noise(pos.xyz)+time.y*.0002);
    pos.xyz=rotZ(pos.xyz,.44*noise(pos.xyz)+time.y*.0003);


    //pos.xyz=rotX(pos.xyz,length(pos.xyz)*6);
    //pos.xyz=rotY(pos.xyz,length(pos.xyz)*6);
    //pos.xyz=rotZ(pos.xyz,length(pos.xyz)*6);


    //pos.xyz=lerp(pos.xyz,normalize(pos.xyz)*38,18.5*pow(noise(pos),3)+.5);
    pos.xyz=lerp(pos.xyz,normalize(pos.xyz)*38,1.5*noise(pos)+.85+.3);
    
    //pos.xyz=normalize(pos.xyz)*44;

    pos = mul(pos, model);

    float f=100;
    //pos.x+=time.x;
    //pos=frac((pos+f/2)/f)*f-f/2;
  

    float4 normal = float4(normals.SampleLevel(sam1, uv, 0).xyz, 1);
    normal = mul(normal, transpose(model));
    //---
  // pos.xyz = rotY(pos.xyz,time.x*.1);
    //normal.xyz = rotY(normal.xyz,time.x*.1);


    output.vnorm = normal;
    
    output.wpos = float4(pos.xyz, 0);
    output.vpos = mul(float4(pos.xyz, 1), view[0]);

    output.pos = mul(pos, view[0]);
    
    float br = (qid%20000) ==  0  ? 1 : 0;
    float br2 = (qid%6000) ==  0  ? 1 : 0;
    float st = (qid%1205) ==  0  ? 1 : 0;
    output.pos.xy+=(grid.zw-.5)*br*42.5;
    output.pos.xy+=(grid.zw-.5)*br2*14.5*(sin(qid*.1)+1.1);

    output.pos.xy+=(grid.zw-.5)*lerp(pow(.25*(sin(qid*.000001)+2.1),2.5),.4,st);
    
    
    output.pos = mul(output.pos, proj[0]);

   // output.pos.x*= aspect.x;
    output.uv = grid.zw;
    output.id = float4(floor(vID/6),0,0,0) ;
    output.rgba = br ? .35 : .25*(abs(noise(pos.xyz))+.5);
    output.rgba = br2 ? .1: output.rgba ;

    output.rgba += (st ? 4 : 0)*(1-br);

    //output.rgba*=pow(1-abs(pos.x/f*2),.5);
    output.rgba*=saturate(output.pos.w*.04);
    //output.rgba = (st*(1-br) ? 6 : output.rgba);

    return output;
}
