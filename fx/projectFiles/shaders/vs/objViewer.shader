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

float3 tube(float2 uv)
{
    float2 a = uv * PI * 2;
    a.x *= -1;
    //float3 pos = float3(sin(a.x), (uv.y-.5)*2, cos(a.x));
    float3 pos = float3(sin(a.x), 0, cos(a.x))*.1;
    pos=rot3(pos,noise3(float3(pos.x,uv.y,pos.z)*112));
    pos.y+=(uv.y-.5)*2;

    return pos;
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

    float p=4;
    float q=2;

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

    /*pos= hash3(qid*.05);
    float3 h=hash3(qid);
    pos = rot3(pos,h+qid/1122);
    float y_fade=0;
    pos=normalize(pos+float3(0,y_fade,0));*/

//  pos*=1+max(noise3_u(pos*12.6)*.2-noise3_u(pos*2.6)*.5,0);

float3 star(float3 pos)
{
    pos=normalize(pos)*22;
    float f=1-saturate(length(pos+float3(0,-10,0))*.00351*(noise(pos)+1.2));
    f=step(.5,pow(f,1.2));
    float3 ofs=float3(200,0,200)*0.3;

    pos = -ofs*f+lerp(pos+ofs*f,normalize(noise3(pos))*26*(noise(pos*2)+1),f/3);

    pos*=1+noise(pos*.1)*f*155;
    
    float cf=115/(length(pos)+1);

    pos= lerp(pos,rotX(pos,-time.x*.01251+length(pos)*.13/2)*cf,f);
    pos= lerp(pos,rotY(pos,-time.x*.01262+length(pos)*.14/2)*cf,f);
    pos= lerp(pos,rotZ(pos,-time.x*.01243+length(pos)*.15/2)*cf,f);


       pos.y*=1+(12/(length(pow(pos.xz,13))*.001+.02))*f*111.2;
       pos.xz=lerp(pos.xz,pos.xz*(3+pow(abs(pos.y)*.005,3)*f*.5),.75);
       pos=lerp(pos,normalize(pos)*122,.61);
         //pos.y*=1-.25*f;

    pos*=1-f/1.13;
    //pos=pos*(85/(length(pos)+45));
    return pos/22;
}

float3 outer_space(float3 pos,float4 grid)
{
        pos=shp(grid.xy);
        pos=rotZ(pos,120*3.14/180.);
        pos+=noise3(pos*2)/.2;
        pos+=noise3(pos*4)/3;
        pos=rot3(pos,noise(pos/22)*5);
        pos=normalize(pos)*25+pos*5.0;
        pos=rot3(pos,1/pos);
        return pos;
}

float3 floor_space(float3 pos,float4 grid, float a)
{
        pos/=float3(1,3,3);
        pos+=noise(pos*3+time.x*.05)*.5;
        pos=rot3(pos,float3(0,1.65,1.33));
        pos=rotY(pos,length(pos)*.36-(time.x*.00135)+a);
        pos.y*=length(pos)/2;
        pos.y-=1.5;
        pos.y-=length(pos)/5;
        pos*=2.36;
        pos.y-=1.5;
        return pos;
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

pos_color sagittarius_v2 (uint qid,float div, float4 grid)
{
    pos_color outp;

    //percentage
    uint outer=62;
    uint floor_=11;
    uint star_=15;
    uint star_2=4957;
    uint big_hl=2006;

    float t=time.x*.01;
    if (qid%floor_==0||qid%star_==0||qid%outer==0||qid%big_hl==0||qid%star_2==0) t=0;

    //pillars instances
    uint cn=7;
    float a= (qid%cn);
    a=a*PI/180.;
    a*=360./cn;
    float h=(sin(a*3)+2)/2;

    //calc
    float3 pos = pillar(grid,a,t,h);
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
    //animation
    pos+=noise(pos2+time.x*.05)*.125;
    
    if (qid%outer==0) pos = outer_space(pos2, grid);
    if (qid%floor_==0) pos = floor_space(pos2,grid,a);
    if (qid%star_==0) pos = star(pos2*.7);

    //camera
    float4 posT;

    if (qid%big_hl!=0)
    {
        posT = mul(float4(pos,1), view[0]);
        posT = mul(posT, proj[0]);
    }

    //size
    float2 scale = float2(proj[0]._m00,proj[0]._m11);
    float2 gzw=(grid.zw-.5)*(noise(sin(pos2*1.1)*11.5)*.3+.55);
    
    if (qid%big_hl==0&&qid%star_!=0)
    {
        if (qid%floor_!=0&&qid%outer!=0) gzw*=72;
        if (qid%floor_==0&&qid%outer!=0) gzw*=25;
        if (qid%floor_!=0&&qid%outer==0) gzw*=42;
    }
    
    if (qid%star_!=0)
    {
        if (qid%outer==0&&qid%floor_!=0) gzw*=(qid%150)==0 ? abs(242*(noise(sin(pos*1.1)*1.5)))+1 :1.;
        if (qid%outer!=0&&qid%floor_==0) gzw*=(qid%150)==0 ? abs(32*(noise(sin(pos*1.1)*1.5)))+1 :1.4;
    }

    if (qid%big_hl!=0)
    {
        posT = mul(float4(pos,1), view[0]);
        posT = mul(posT, proj[0]);
    }

    if (qid%outer!=0&&qid%floor_!=0&&star_ ==qid) 
    {
       posT = mul(float4(0,0,0,1), view[0]);
       posT = mul(posT, proj[0]);
       gzw*=370 ;    
    }

    posT.xy+=gzw*.016*posT.w*scale;
    outp.pos=posT;
    
    //color
    outp.rgba=float4(float3(.4,.1,.2)*noise3(pos.xzx*.3+a*.15+.85),1)/3.5+.015;

    if (qid%big_hl==0) outp.rgba*=.35*1.5;

    if (qid%outer==0)
    {
        if (qid%123==0) outp.rgba=2.6/(length(gzw)+1)*float4(4,5,6,1)/6*2;
        if (qid%star_!=0)
        {
            if (qid%big_hl!=0) outp.rgba+=qid%150==0 ? float4(2,3,5,1)*length(pos)/11:.74*float4(1,2,3,1)/2;     
            else outp.rgba+=.474*.7;     
        }
    }
    if (qid%star_==0) outp.rgba=float4(1,2,5,1)*.051;
    if (qid%floor_==0) outp.rgba=float4(1,2,5,1)*.04/4;
  
    outp.rgba/=posT.w/15+.1;
    if (qid%star_2==0&&qid%big_hl!=0) outp.rgba=(noise(pos)+1.2)*1.5;
     
    outp.rgba*=1.;
    return outp;
}

float tri(float x)
{
    return lerp(frac(x),1-frac(x),floor(frac(x/2)*2));
}

float3 double_star(float2 grid,float a, float t, float h,uint qid,uint star2)
{
    //float3 pos = float3(0,(grid.y-.5)*2,0);
    //float3 pos = float3(0,pow(cos((grid.y-.5)*PI*2),1),0);
    //grid.y+=sin(time.x*.01)/2;
    float y1=(grid.y-.5)*2;
    float y2=pow((smooth(grid.y)-.5)*2,3);
    float y=lerp(y1,y2,0.);
    float3 pos = float3(0,y,0);
    
    //pos=rot3(pos,grid.xyx*222);
    pos.z=0;
    float scale=sin(abs(grid.y-.5)*PI*2)/9+.1;
    float scale2=scale;
    scale2=pow(scale,3)*72;

    float tm=time.x;
        float dir=sign(y1)*((qid%2)-.5);
    //
    if (qid%star2==0)
    {
        pos.y=frac(pos.y-time.x*.001*dir*sign(y1))*sign(pos.y);
        pos-=noise3(qid/11100.)*scale2*1.2;
    }


    pos = rotZ(pos,pos.y*sign(pos.y)*7.85);

    if (qid%9==0&&qid%star2!=0) pos+=normalize(pos)*noise(pos*25);

    pos.x+=(qid%2)*2;
    pos=pos.yzx;
    pos*=4;
    pos.z-=4;
    //pos+=noise3(grid.xyy*112+time.x*.1)*2.*scale2;
    //float3 g1=float3(qid/11112.,qid/11122.,qid/11412.)*2;
    float3 g1 =noise3(pos*4)*pow(length(pos),1)*.1-.5;
    float y3=grid.y;
    float3 g=float3(y3/12.,y3/22.,y3/14.)*5134+g1;
    
    if (qid%star2!=0)
    {
        //pos+=noise3(pos*.8+dir*sign(y1)*tm.x*.00004+qid/1111.)*.07;
        pos+=noise3(g*.2+tm.x*.05*dir)*scale2*1.51;
        pos+=noise3(grid.xyy*8+dir*tm.x*.0001*+qid/41111.)*2.3*scale2;
        
    }

    return pos*2.5;
}

pos_color fish (uint qid,float div, float4 grid)
{
    pos_color outp;

    //percentage
    uint outer=262;
    uint star_2=4957;
    uint big_hl=1203;

    float t=time.x*.01;
    if (qid%outer==0||qid%big_hl==0||qid%star_2==0) t=0;

    //calc
    float3 pos = double_star(grid,0,t,0,qid,star_2);
//    pos = rot3(pos,float3(-.9,0,.4));

    //animation
    //pos+=noise(pos+time.x*.05)*.125;
    
    if (qid%outer==0) pos = outer_space(pos, grid);
//    if (qid%floor_==0) pos = floor_space(pos2,grid,a);
  //  if (qid%star_==0) pos = star(pos2*.7);

    //camera
    float4 posT;

    if (qid%big_hl!=0)
    {
        posT = mul(float4(pos,1), view[0]);
        posT = mul(posT, proj[0]);
    }

    //size
    float2 scale = float2(proj[0]._m00,proj[0]._m11);
    float2 gzw=(grid.zw-.5)*(noise(sin(pos*1.1)*11.5)*.3+.55);
    
    if (qid%big_hl==0)
    {
        if (qid%outer==0) gzw*=42;
        else gzw*=42;
    }
    
    if (qid%outer==0) gzw*=(qid%150)==0 ? abs(242*(noise(sin(pos*1.1)*1.5)))+1 :1.;

    if (qid%big_hl!=0)
    {
        posT = mul(float4(pos,1), view[0]);
        posT = mul(posT, proj[0]);
    }

    posT.xy+=gzw*.016*posT.w*scale;
    outp.pos=posT;
    
    //color

    float dir=(qid%2-.5);
    float v=15;
    float q= .5+lerp(grid.y,1-grid.y,qid%2)/2;
    q=.5+dir*(lerp(saturate((1-grid.y)*v),saturate(grid.y*v),qid%2));
    //float q= saturate(.5+(1-grid.y)*v);
    //outp.rgba=float4(float3(.1,.1,.1)*noise3(pos*.1-.8),1)/3.5+.015;
    outp.rgba=.0125*float4(lerp(float3(1,0.2,0.1),float3(0.1,0.2,1),q)*(noise3(pos*1.1)*.5+.6),1);
    //outp.rgba=.5*float4(lerp(float3(1,.1,.1),float3(.1,.1,1),q),1)/23.5-.001;
    //outp.rgba+=float4(lerp(float3(1,.1,.1),float3(.1,.1,1),saturate(pos.yyy/4+.1)),1)/23.5-.001;

    //if (qid%big_hl==0) outp.rgba*=.35*1.5;

    if (qid%outer==0)
    {
        if (qid%123==0) outp.rgba=2.6/(length(gzw)+1)*float4(4,5,6,1)/6*2;
        else outp.rgba+=.474*.7;     
    }
  
    outp.rgba/=posT.w/15+.1;
    if (qid%star_2==0&&qid%big_hl!=0) outp.rgba=(noise(pos)+1.2)*1.5;
     
    outp.rgba*=1.;
    return outp;
}

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    float div=1;
    float4 grid = getGridP(vID, 1, int2(gX,gY));
    float2 uv = grid.xy;
    
    uint qid = floor(vID/6)/div;
    
    //pos_color p = sagittarius_v2(qid,div,grid);
    pos_color p = fish(qid,div,grid);

    output.pos=p.pos;

    output.vnorm = 0;
    output.wpos = 0;
    output.vpos = 0;
    output.uv = grid.zw;
    output.id = float4(floor(vID/6)/div,0,0,0) ;
    output.rgba = p.rgba;

    return output;
}
