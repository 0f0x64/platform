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
    float sz;
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

float4 transform(float3 pos,float2 grid, float size)
{
    float2 uv = grid-.5;
    float2 scale = float2(proj[0]._m00,proj[0]._m11);
    float4 posT;
    posT = mul(float4(pos,1), view[0]);
        float4 pt = mul(posT, proj[0]);
        float2 sz = uv*.002*(posT.z/posT.w)*size;
        sz = uv*size*.1;
        posT.xy+=sz;
        posT = mul(posT, proj[0]);

        return posT;
}

float4 transform_unisize(float3 pos,float2 grid,float size)
{
    float2 uv = grid-.5;
    float2 scale = float2(proj[0]._m00,proj[0]._m11);
    float4 posT;
    posT = mul(float4(pos,1), view[0]);
        float4 pt = mul(posT, proj[0]);
        float2 sz = uv*.002*(posT.z/posT.w)*size;

        sz*=normalize(scale)*2;
        posT = mul(posT, proj[0]);
        posT.xy+=sz;

        return posT;
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
pos_color star(uint qid, float4 grid)
{
    float3 pos = pillar(qid,0,0,1);
    pos=shp(grid);
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
 

    pos*=1-f/1.13;

    pos_color p;

    p.rgba = float4(float3(1,2,3),1)*.1+.0015;

    if (qid==0)
    {
        //hilight
        p.rgba/=14;
        p.pos=transform(float3(0,0,0),grid.zw,11102);
        p.sz=2;
    }
    else
    {
        p.pos = transform_unisize(pos/22.,grid.zw,3.);
        p.sz=2;
    } 
    
    return p;
}

pos_color outer_space(float3 pos,float4 grid, uint qid)
{

        uint glow = qid%(2364);

        pos=shp(grid.xy);
       // pos.xz*=.1;
       
       // pos=rotZ(pos,1*3.14/180.);
        //pos+=noise3(pos*112)/.2;
       // pos+=noise3(pos*14)/3;
     //   pos=rot3(pos,noise(pos/22)*5);
      ///  pos=normalize(pos)*25+pos*5.0;
     //   pos=rot3(pos,pos+555*noise3(pos*.21+77));
       // pos+=.2*rot3(pos,4*pos+.001*noise3(pos*3));
        pos+=.7*rot3(pos,31/pos+.1*noise3(pos*3));
        pos=lerp(normalize(pos)*33,pos,.45);;
       
        pos_color p;

        p.rgba = float4(float3(7,8,9),1)*.051+.0015;
    if (glow==0)
    {
        //hilight
        p.pos=transform(pos,grid.zw,102);
       
     
         p.sz=2;
    }
    else
    {
        p.pos = transform_unisize(pos,grid.zw,1.);
       
         
      //  p.rgba +=min(0,sign(noise(-pos2*.12-1.6)))/61.;
   //p.rgba=0;
         p.sz=1;

    } 
    
    return p;
}



pos_color floor_space(uint qid,float4 grid)
{
     uint glow = qid%(20394);
    uint cn=7;
    float a= (qid%cn);
    a=a*PI/180.;
    a*=360./cn;
    float t=time.x*.1;
    float3 pos = pillar(grid.xy,a,t,.5);  
    float3 pos2=pos;
        pos/=float3(1,3,3);
        pos+=noise(pos*3+time.x*.05)*.5;
        pos=rot3(pos,float3(0,1.65,1.33));
        pos=rotY(pos,length(pos)*.36-(time.x*.00135)+a);
        pos.y*=length(pos)/2/2;
        pos.y-=1.5;
        pos.y-=length(pos)/5;
        pos*=2.36;
        pos.y-=1.5;
       

        pos_color p;
        p.rgba = float4(1,2,3,1)/120.+.0015;


    
        p.pos = transform_unisize(pos,grid.zw,2.);
       ;

       // p.rgba +=min(0,sign(1./noise(-pos2*.2-2.6)))/91.;
   
         p.sz=1;

   
   
    //density compensation
    p.rgba/=min(pow(p.pos.w,1.1)*.1+.5,11);
    return p;
}





pos_color pillars_array(uint qid,float4 grid)
{
    uint glow = qid%(10394);

     float t=time.x*.01;
     if (glow ==0)t=0;
    //pillars instances
    uint cn=7;
    float a= (qid%cn);
    a=a*PI/180.;
    a*=360./cn;
    float h=(sin(a*3)+2)/2;

    //calc
    float3 pos = pillar(grid.xy,a,t,h);
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
    //extra animation
    
    


    pos_color p;
 p.rgba = float4(noise3_u(a*float3(13,15,12)*221+77+sin(pos2*1.4)),1)/30.+.0015;
    if (glow==0)
    {
        //hilight
        p.pos=transform(pos,grid.zw,92);
        p.rgba*=2;
     
         p.sz=172;
    }
    else
    {
        p.pos = transform_unisize(pos,grid.zw,1.5);
       ;

       // p.rgba +=min(0,sign(1./noise(-pos2*.2-2.6)))/91.;
   
         p.sz=1;

    }
   
    //density compensation
    p.rgba/=min(pow(p.pos.w,1.1)*.1+.5,11);
    return p;
}

pos_color sagittarius_v2 (uint qid,float div, float4 grid)
{
    pos_color outp;


    //percentage
    uint outer=612;
    uint floor_=11;
    uint star_=15;
    uint star_2=4957;
    uint big_hl=2006;


    float3 pos=0;
    
    outp = pillars_array(qid,grid);
    if (qid%outer==0) outp = outer_space(0, grid, qid);
    if (qid%floor_==0) outp = floor_space(qid,grid);
    if (qid%star_==0) outp = star(qid,grid);

    /*float3 pos2=pillar(grid,0,t,1);
    float a=0;

    if (qid%outer==0) pos = outer_space(0, grid);
    if (qid%floor_==0) pos = floor_space(qid,pos2,grid);
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
    float2 gzw=(grid.zw-.5);
    
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
        posT = mul(float4(pos,1), view[0]);
        float4 pt = mul(posT, proj[0]);
        float2 sz = gzw*.002*(posT.z/posT.w);

    if (qid%big_hl==0)
    {
        posT.xy+=sz;
        posT = mul(posT, proj[0]);
    }else
    {
        sz*=normalize(scale)*2;
        posT = mul(posT, proj[0]);

        posT.xy+=sz;
    }
    
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
     */

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

/*pos_color fish (uint qid,float div, float4 grid)
{
    pos_color outp;

    //percentage
    uint outer=262;
    uint star_2=4957;
    uint big_hl=1111111203;

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

   // if (qid%big_hl!=0)
    {
        posT = mul(float4(pos,1), view[0]);
        posT = mul(posT, proj[0]);
    }

    //size
    float2 scale = float2(proj[0]._m00,proj[0]._m11);
    float aspect_ = proj[0]._m11 / proj[0]._m00;
    float2 gzw=(grid.zw-.5);//*(noise(sin(pos*1.1)*11.5)*.3+.55);
    
    
    if (qid%outer==0) gzw*=(qid%150)==0 ? abs(242*(noise(sin(pos*1.1)*1.5)))+3 :1.;
    if (qid%big_hl==0&&qid%outer!=0)
    {
        if (qid%outer==0) gzw*=42;
        else gzw*=42;
    }

        posT = mul(float4(pos,1), view[0]);
        float4 pt = mul(posT, proj[0]);
        float2 sz = gzw*.002*(posT.z/posT.w);
        
    if (qid%big_hl==0)
    {

        
        posT.xy+=sz;
        posT = mul(posT, proj[0]);
    }else
    {
        sz*=normalize(scale)*2;
        posT = mul(posT, proj[0]);

        posT.xy+=sz;
    }
    
    
    outp.sz=2;
    if (qid%outer==0&&qid%150!=0) 
    {
        outp.sz=1;
    }

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
        if (qid%123==0) outp.rgba=2.6/(length(gzw)+1)*float4(4,5,6,1)/6;
        else 
        outp.rgba+=.474*.7;     
    }
  
   // outp.rgba/=posT.w/15+.1;
    //if (qid%star_2==0&&qid%big_hl!=0) outp.rgba=(noise(pos)+1.2)*1.5;
     
    outp.rgba*=1.;
    return outp;
}*/

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    float div=1;
    float4 grid = getGridP(vID, 1, int2(gX,gY));
    float2 uv = grid.xy;
    
    uint qid = floor(vID/6)/div;
    
    pos_color p = sagittarius_v2(qid,div,grid);
    
    //density compensation
   // p.rgba/=min(pow(p.pos.w,1.1)*.1+.5,11);
    
    output.pos=p.pos;

    output.vnorm = 0;
    output.wpos = 0;
    output.vpos = 0;
    output.uv = grid.zw;
    output.id = float4(floor(vID/6)/div,0,0,0) ;
    output.rgba = p.rgba;
    output.sz1 = float4(p.sz,0,0,0);

    return output;
}
