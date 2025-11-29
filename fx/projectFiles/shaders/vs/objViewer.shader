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
    
    float qid = floor(vID/6);

    float3 pos;
    pos = noise3(qid*float3(.0001,.0002,.0003)*5);
    pos = noise3(pos.xyz*13)*10;

    pos=lerp(pos,noise(pos.xyz*1.2)/(pos*4),noise(pos*2.3));

    pos=rotX(pos,2.22*noise(pos)+time.y*.0001);
    pos=rotY(pos,1.33*noise(pos)+time.y*.0002);
    pos=rotZ(pos,.44* noise(pos)+time.y*.0003);

    //float ff= (time.y-21*60)*.001;
    //ff=pow(ff,5)*(length(pos)*112);
   // pos=rotZ(pos,ff);
    //pos=rotY(pos-pos/4,ff)+pos/4;
    //pos=rotZ(pos-pos/4,ff)+pos/4;

  
    //pos=lerp(pos.xyz,normalize(pos.xyz)*38,1.5*noise(pos)+.85+.3);
    pos=lerp(pos*22,normalize(pos)*38,(4.5*noise(pos)+.85)/12+17-time.y*.01-3);
    
   // pos=normalize(pos)*72;

    


 //   pos=rotX(pos,time.x*.005);
  //  pos=rotY(pos,time.x*.006);
   // pos=rotZ(pos,time.x*.007);

    //pos.y*=pow(2/(length(pos.xz)+1),1.03);
    //pos.y*=5/(length(pos.xz)+1)*2.3;

    

    float f=175;
    //pos.x+=time.x;    
    //pos=frac((pos+f/2)/f)*f-f/2;
  


    output.vnorm = 0;
    output.wpos = float4(pos.xyz, 0);
    output.vpos = mul(float4(pos.xyz, 1), view[0]);

    output.pos = mul(float4(pos,1), view[0]);
        output.pos = mul(output.pos, proj[0]);
    
        float2 scale = float2(proj[0]._m00,proj[0]._m11)*(output.pos.w)*.01;
        float2 gzw=(grid.zw-.5)*scale;

    
    float br2 = (qid%6000) ==  0  ? 1 : 0;
    float st = (qid%3206) ==  0  ? 1 : 0;
    float br = (qid%1000000) ==  0  ? 1 : 0;
    output.pos.xy+=(grid.zw-.5)*br*32.5*2*scale;
    output.pos.xy+=(grid.zw-.5)*br2*14.5*(sin(qid*.1)+1.1)*4.5*scale/output.pos.z*50;

    output.pos.xy+=(gzw)*lerp(pow(.25*(sin(qid*.000001)+2.1),3.),.2,st)*1.75;
    
    
    

    output.uv = grid.zw;
    output.id = float4(floor(vID/6),0,0,0) ;

    float3 n =.125*(abs(noise3(frac(pos.xyz*.035)))+.5);
    float4 neb = float4(n*1.2,1);
    output.rgba = neb;
    output.rgba = br2 ? .1*4: output.rgba ;
    output.rgba = (st ? 64*(1-br)*output.rgba : output.rgba);
    output.rgba = br ? float4(2.95,2,5,1) : output.rgba;
 //   output.rgba*=pow(1-length(pos/f*1.8),.5);
    
    output.rgba/=max(length(scale)*1.5,1);
    output.rgba*=saturate(output.pos.w*.04);
    

    output.rgba *= min(length(scale)*.5,1);
    //output.rgba = (st*(1-br) ? n*16 : output.rgba);

    return output;
}
