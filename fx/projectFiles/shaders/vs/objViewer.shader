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
        //return float3(noise(p.x),noise(p.y),noise(p.z));
        return float3(noise(p.xyz),noise(p.yzx),noise(p.zxy));
    }

    float3 noise3_u(float3 p)
    {
        //return float3(noise(p.x),noise(p.y),noise(p.z));
        return float3(noise(p.xyz),noise(p.yzx),noise(p.zxy))+.5;
    }

struct pos_color
{
    float4 pos;
    float4 rgba;
};

pos_color base(float qid,float div, float4 grid)
{
    pos_color outp;

    float br2 = ((qid)%(316)) ==  0  ? 1 : 0;
    float st = (qid%(3206)) ==  0  ? 1 : 0;
    float br = (qid%(200000)) ==  0  ? 1 : 0;

    float3 pos;
    pos = noise3(qid*float3(.0001,.0002,.0003)*5);
    pos = noise3(pos.xyz*33)*10;
    pos=lerp(pos,normalize(1/(pos+1.6))*(.7-br*br2*st)*3,length(pos)/2);
    pos=lerp(pos,noise(pos.xyz*1.2)/(pos*4),noise(pos*12.3));

    float rs = length(pos)*.00+.5;

    pos=rotX(pos,2.22*noise(pos)+time.x*.01*rs);
    pos=rotY(pos,1.33*noise(pos)+time.x*.02*rs);
    pos=rotZ(pos,.44* noise(pos)+time.x*.03*rs);
    
    pos+=normalize(pos)*br2*2.;

    float tf= -time.y*.002+3;
    pos=lerp(pos*12,normalize(pos)*38,(4.5*noise(pos)+.85)/12+tf);

    float4 posT;
    posT = mul(float4(pos,1), view[0]);
    posT = mul(posT, proj[0]);
    
    float2 scale = float2(proj[0]._m00,proj[0]._m11);
    float2 gzw=(grid.zw-.5)*scale;
    posT.xy+=(grid.zw-.5)*br*142.5*scale;
    posT.xy+=(grid.zw-.5)*br2*15.5*pow(sin(qid)*.5+.55,.25)*scale*2*max(pow(length(pos),1.8)*.00003,1);
    posT.xy+=gzw*posT.w/250*(1-st);
    posT.xy+=(gzw)*1.2*posT.w/300*st;

    outp.pos = posT;


    //color
     float3 l1 = sin(pos.xyz*.0005)*18;
    l1 = (noise3(pos*.001)+1)*42;
    float3 l2= sin(length(pos)*.005*float3(1,2,5)-time.x*.0);
    float3 n =.125*(abs(noise3(l1-l2)*float3(6,2,4)*1)+.95);
    float4 neb = float4(n,1)*.7;
    outp.rgba = neb;
    outp.rgba = br2 ? .02*neb*7: outp.rgba ;
    outp.rgba = (st ? pow(div,.25)*14*(1-br)*outp.rgba : outp.rgba);
    outp.rgba = br ? float4(3.95,2,4,1)*.2 : outp.rgba;
    
    outp.rgba.b+=pow(saturate(length(pos)*.0041),2.4)*.16;
  
    if (!st&!br2&!br) outp.rgba/=outp.pos.w*.01*pow(div,.5);
    if(st&&!br&&!br2) outp.rgba=float4(1,1,2,1)*2.;
    outp.rgba*=saturate(posT.z*.001+.4);
    outp.rgba*=.13*2;
    return outp;
}

pos_color sagittarius (float qid,float div, float4 grid)
{
    pos_color outp;

    float br2 = ((qid)%(316)) ==  0  ? 1 : 0;
    float st = (qid%(3206)) ==  0  ? 1 : 0;
    float br = (qid%(200000)) ==  0  ? 1 : 0;

    float3 pos;
    pos = noise3(qid*float3(.0001,.0002,.0003)*5);
    pos = noise3(pos.xyz*33)*10;
    pos=lerp(pos,normalize(1/(pos+1.6))*(.7-br*br2*st)*3,length(pos)/2);
    pos=lerp(pos,noise(pos.xyz*1.2)/(pos*4),noise(pos*12.3));

    float rs = length(pos)*.00+.5;
   
    pos+=normalize(pos)*br2*4.;
    

    float tf= -time.y*.002+3;
    pos.xz*=7*(2+1.5*noise(pos*2))*(length(pos/2));

    pos.y+=100;
    float rock_mask = sin(-abs(length(pos)*.01)*2+11.5)*12*(1+sin(atan2(pos.x,pos.z)*8))*(saturate(length(pos)*.002));
    rock_mask*=3+sin(atan2(pos.x,pos.z)*5)*1;
    pos=rotY(pos,sin(atan2(pos.x,pos.z)*1)*.4);

    pos.y-=(pow(rock_mask,2))*(1+noise(pos*.4)*1.5)*.48;
    pos.y-=(pow(rock_mask,1))*(1+noise(pos*.4)*1.5)*12.48;
    pos.y+=sin(atan2(pos.x,pos.z)*4)*62;
    
    pos-=noise(pos*.05)*15;

    float lowf =pow(length(pos.xz)*.4,2.5)*.01;
    pos.y+=min(lowf,190);
    pos.y-=saturate(length(pos)*.007)*91;
    //pos.y-=pow(length(pos.xyz),2)*.001/1.2;
    pos.xz*=2-min(lowf/1111,1.1);
    pos.xz*=1+saturate(-pos.y*.003);
    
    pos=lerp(pos,pos*.1,.1)*.5;
    pos=lerp(pos,normalize(pos)*422,saturate(length(pos)*.0013)*1.2);
    pos.xz*=.87+max(-pos.y,.0)*.0001;
    pos.y*=1-max(-pos.y*2+noise(pos)*2,310)*.001;
    
    
    pos.y+=-1111/pow(length(pos)*.1,13);
    float f2=saturate(-pos.y*.001-.05);
    pos=rotY(pos+pos/  3,f2*112);
    

    //star
    float f=1-saturate(length(pos+float3(0,-10,0))*.00351*(noise(pos)+1.2));
    f=step(.5,pow(f,1.2));
    float3 ofs=float3(200,0,200)*0;

    pos = -ofs*f+lerp(pos+ofs*f,normalize(noise3(pos))*26*(noise(pos*2)+1),f/3);

    pos*=1+noise(pos*.1)*f*155;
    
    float cf=295/(length(pos)+1);

    pos= lerp(pos,rotX(pos,-time.x*.0251+length(pos)*.03)*cf,f);
    pos= lerp(pos,rotY(pos,-time.x*.0262+length(pos)*.04)*cf,f);
    pos= lerp(pos,rotZ(pos,-time.x*.0243+length(pos)*.05)*cf,f);


    if (!(st||br||br2)) 
    {
        pos.y*=1+(12/(length(pow(pos.xz,4))*.001+.2))*f*7.2;
       pos.xz=lerp(pos.xz,pos.xz*(1+pow((pos.y-470)*.005,3)*f*.15),.75);
       //pos*=1+normalize(pos)*2*f;
        // pos.y*=1-.25*f;
    }
    pos*=1-f/1.13;

   // pos.y*=1-pow(cos(length(pos*362))*f,5);    
    //pos.y-=(1/pos.y)*f;
    pos.y/=1.5;
    //pos*=1+f*2;
  
    pos.y+= 140*f;
    pos.y-=85;
  float3 pos2=pos;

    float4 posT;

    posT = mul(float4(pos,1), view[0]);
    posT = mul(posT, proj[0]);
    
    float2 scale = float2(proj[0]._m00,proj[0]._m11)*2.3;
    float2 gzw=(grid.zw-.5)*scale;
    posT.xy+=(grid.zw-.5)*br*212.5*scale;
    posT.xy+=(grid.zw-.5)*br2*15.5*pow(sin(qid)*.5+.55,.25)*scale*2*max(length(pos2)*.00003,1)*1.4*(noise(pos*.01)*3+1);
    posT.xy+=gzw*posT.w/250*(1-st);
    posT.xy+=(gzw)*1.2*posT.w/300*st;

    //posT.xy+=(gzw)*length(pos)*.0051;
    
    outp.pos = posT;


    //color
     float3 l1 = sin(pos2.xyz*.0005)*18;
    l1 = (noise3(pos2*.001)+1)*42;
    float3 l2= 1;
    float3 n =.125*(abs(noise3(pos*.001)*float3(8,4,2)*2)+.5);
    float4 neb = float4(n,1)*.7;
    outp.rgba = neb;
    outp.rgba = br2 ? .02*neb*7: outp.rgba ;
    outp.rgba = (st ? pow(div,.25)*14*(1-br)*outp.rgba : outp.rgba);
    outp.rgba = br ? float4(3.95,2,4,1)*.2 : outp.rgba;
    
    outp.rgba.b+=pow(saturate(length(pos2)*.0041),2.4)*.1;
    outp.rgba*=1.2-sin(pow(outp.rgba.gbra,2.5)*1);
    outp.rgba=lerp(outp.rgba,outp.rgba.bgra,f);
  
    if (!st&!br2&!br) outp.rgba/=outp.pos.w*.01*pow(div,.5);
    if(st&&!br&&!br2) outp.rgba=float4(1,1,2,1)*2.;
    outp.rgba*=saturate(posT.z*.001+.4);
    //outp.rgba+=.015*length(scale);
    outp.rgba*=.13*2;
    return outp;
}

float hash_s(int qid)
{
    return hash(qid)-.5;
}

float3 rot3(float3 pos, float3 angle)
{
    return rotZ(rotY(rotX(pos,angle.x),angle.y),angle.z);
}

float3 hash3(int qid)
{
    return float3(hash(qid*.27),hash(qid*.28),hash(qid*.29))-.5;
}

float smooth(float x)
{
    return x + (x - (x * x * (3.0 - 2.0 * x)));

}

float3 shp(float2 uv)
{
    float2 a = uv * PI * 2;
    a.x *= -1;
    float3 pos = float3(sin(a.x), sin(a.y ), cos(a.x));
    pos.xz *= cos(a.y );

    return pos;
}

float3 quantize(float3 x,float q)
{
    return floor(x*q)/q;
}

float3 getTpos(float2 a, float R,float p,float q)
{    
        return float3((R + cos(a.x * q)) * cos(a.x * p), 
                      (R + cos(a.x * q)) * sin(a.x * p),
                      sin(a.x * q));
}

float3 torusKnot(float2 a)
{
    
    float R = 3.5;
    float r = 1.;

    float p=1;
    float q=3;

    float3 pos = getTpos(a,R,p,q);
    float3 T = pos - getTpos(a+float2(.01,0),R,p,q);
    float3 N = pos+T;
                    
    float3 B = cross(T,N);
    N = cross(B,T);
    
    B= normalize(B);
    N= normalize(N);

    pos += r * (cos(a.y) * N + sin(a.y) * B);

    return pos;
}

pos_color sagittarius_v2 (int qid,float div, float4 grid)
{
    pos_color outp;

    float br2 = ((qid)%(11316)) ==  0  ? 1 : 0;
    float st = (qid%(3206)) ==  0  ? 1 : 0;
    float br = (qid%(200000)) ==  0  ? 1 : 0;

    float3 pos;
    pos= hash3(qid*.05);
    float3 h=hash3(qid);
    pos = rot3(pos,h+qid/1122);
    float y_fade=0;
    pos=normalize(pos+float3(0,y_fade,0));

    //float2 a = grid.xy * PI * 2;
    float na=time.x*.0;

    
    pos*=1+noise3(pos*111.6)*1.6;
    pos.y*=1.8;

    pos*=1+noise3(pos*1.6);
    pos = rot3(pos,2.2/(pow(pos,4)+1));
    pos.y*=1.5;
    pos*=1+noise3(pos);
    //pos.y*=1.3;
    pos*=1+noise3(pos*3.5+na)/3;
    pos*=1.3;
    
    
    pos*=1+max(noise3_u(pos*12.6)*.2-noise3_u(pos*2.6)*.5,0);
    //pos*=2;
    //pos=lerp(pos,normalize(pos)*22,1);
    //pos.y=lerp(pos,normalize(pos)*3,abs(pos.y)).y;

    //pos += torusKnot(a);

    //pos = shp(grid.xy);
    //pos*=noise(pos*2222);
    //pos=normalize(pos);
    //pos*=1+noise3(pos*1.6);
    //pos*=1+max(noise3_u(pos*15.6)*.2-noise3_u(pos*2.6)*.5,0);
    //pos*=5;


    float4 posT;

    posT = mul(float4(pos,1), view[0]);
    posT = mul(posT, proj[0]);

    float2 scale = float2(proj[0]._m00,proj[0]._m11);
    float2 gzw=(grid.zw-.5)*scale;
    gzw*=1+br2*92;
    int q = qid%217116;
    float s=sin(time.x/3+floor(qid/2))*34+60;
    if (q==2) gzw*=float2(s,1);
    if (q==1) gzw*=float2(1,s);
    gzw=rotZ(gzw.xyx,time.x*.0+sin(pos)*0+45).xy;
    posT.xy+=gzw*.004*posT.w*2*scale*2;

    //posT.y*=1+q/3;

    outp.pos=posT;
    outp.rgba=float4(noise3(pos*.1+2)+.1,1)/7+.04;
    
    //outp.rgba/=1+br2;
    outp.rgba/=.4*posT.z;
    //outp.rgba+=q==0? 5:0;
    if (q==1||q==2) outp.rgba*=50;
    int q2 = qid%1716;
    if (q2==0&&!br2) outp.rgba*=120;
    outp.rgba/=length(scale*scale*scale)+1;
    
    
    return outp;
}

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    float div=1;
    float4 grid = getGridP(vID, 1, int2(gX,gY));
    float2 uv = grid.xy;
    
    int qid = floor(vID/6)/div;
    
    //pos_color p = base(qid,div,grid);
    //pos_color p = sagittarius(qid,div,grid);
    pos_color p = sagittarius_v2(qid,div,grid);

    output.pos=p.pos;

    output.vnorm = 0;
    output.wpos = 0;
    output.vpos = 0;
    output.uv = grid.zw;
    output.id = float4(floor(vID/6)/div,0,0,0) ;
    output.rgba = p.rgba;

    return output;
}
