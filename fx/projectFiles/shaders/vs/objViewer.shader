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
    float div=4;
    float4 grid = getGridP(vID, 1, int2(gX,gY));
    float2 uv = grid.xy;
    
    float qid = floor(vID/6)/div;

    float3 pos;
    pos = noise3(qid*float3(.0001,.0002,.0003)*5);
    pos = noise3(pos.xyz*13)*10;

    pos=lerp(pos,noise(pos.xyz*1.2)/(pos*4),noise(pos*2.3));

    float rs = pow(length(pos.xy)*.04,2);

    pos=rotX(pos,2.22*noise(pos)+time.x*.01*rs);
    pos=rotY(pos,1.33*noise(pos)+time.x*.02*rs);
    pos=rotZ(pos,.44* noise(pos)+time.x*.03*rs);
    
    
    float ff= (time.y-21*60)*.00;
    ff=11/(length(pos));
    /*pos=rotX(pos,ff);
    pos=rotY(pos-pos/4,ff)+pos/4;
    pos=rotZ(pos-pos/4,ff)+pos/4;*/
    /*pos=rotX(pos,pos.x/4);
    pos=rotY(pos,pos.y/4);
    pos=rotX(pos,pos.y/4);*/



  float tf= -time.y*.002+3;
    //pos=lerp(pos.xyz,normalize(pos.xyz)*38,1.5*noise(pos)+.85+.3);
  //  pos=lerp(pos*22,normalize(pos)*38,(4.5*noise(pos)+.85)/12+tf);
    pos*=22;
    //pos=lerp(pos,normalize(pos)*192,pow(saturate(length(pos*.006)),.6)*.5);

    float hm = pow(sin(atan2(pos.x,pos.z)*2.),1);
    pos.y*=.1;
    pos.y*=length(pos*2)*.013;
    pos.y+=pow(sin(length(pos*.01))+.1,5)*44.5*hm;
    pos.y+=abs(noise(pos*.075))*22*pos.y*.18;
    //pos.y-=44;
    //pos*=2;



    float f=175;
   // pos.x+=time.x;    
  //  pos=frac((pos+f/2)/f)*f-f/2;
  


    output.vnorm = 0;
    output.wpos = float4(pos.xyz, 0);
    output.vpos = mul(float4(pos.xyz, 1), view[0]);

    output.pos = mul(float4(pos,1), view[0]);
        output.pos = mul(output.pos, proj[0]);
    
        float2 scale = float2(proj[0]._m00,proj[0]._m11);
        float2 gzw=(grid.zw-.5)*scale;

    
    float br2 = ((qid)%(316)) ==  0  ? 1 : 0;
    float st = (qid%(3206)) ==  0  ? 1 : 0;
    float br = (qid%(200000)) ==  0  ? 1 : 0;



    output.pos.xy+=(grid.zw-.5)*br*232.5*scale;
    output.pos.xy+=(grid.zw-.5)*br2*15.5*pow(sin(qid)*.5+.55,.25)*scale*2.*max(pow(length(pos),1.8)*.00003,1);
    output.pos.xy+=gzw*output.pos.w/250*(1-st)*1.;
    output.pos.xy+=(gzw)*1.2*output.pos.w/300*st;

    output.uv = grid.zw;
    output.id = float4(floor(vID/6)/div,0,0,0) ;

    float3 l1 = sin(pos.xyz*.0005)*18;
    l1 = (noise3(pos*.001)+1)*42;
    float3 l2= sin(length(pos)*.005*float3(1,2,5)-time.x*.0);
    float3 n =.125*(abs(noise3(l1-l2)*float3(6,2,4)*1)+.95);
    float4 neb = float4(n,1);
    output.rgba = neb;
    output.rgba = br2 ? .02*neb*3: output.rgba ;
    output.rgba = (st ? pow(div,.25)*14*(1-br)*output.rgba : output.rgba);
    output.rgba = br ? float4(3.95,2,4,1)*.05 : output.rgba;
    
    output.rgba.b+=pow(saturate(length(pos)*.0041),2.4)*.16;
  
    if (!st&!br2&!br) output.rgba/=output.pos.w*.01*pow(div,.5);
    if(st&&!br&&!br2) output.rgba=float4(1,1,2,1)*1.5;
    output.rgba*=saturate(output.pos.z*.001+.4);
    //output.rgba*=9;
    //output.rgba*=pow(1-length(pos/f*1.8),.5);

    return output;
}
