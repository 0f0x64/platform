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
    float glow_p;
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

pos_color star(uint qid, float4 grid)
{
    float2 uv=grid.xy;
    uint cnt=7;
    uv=frac(uv*cnt);
    float b = quantize(grid.x,cnt);
    float ang=b*360.;
    float2 a = uv * PI * 2;
    a.x *= -1;
    float3 pos = 0;
    float r=18*hash(b)+1;
    
    if (qid%12!=0)
    {
        r=hash(b*3111.123)*18+4;
        pos+=7*noise3(qid*float3(1,2,3)/1000)*(pow(sin(a.y),3)+.1);
        pos.x-=r;
        float aaa=frac(a.y-PI+time.x*.01)*PI*2;
        pos=rotZ(pos,aaa);
        pos.x+=r;
    }
    else
    {
        pos+=7*noise3(qid*float3(1,2,3)/1000)*pow(sin(a.y),3);
        float d=11*hash(qid/11000.)+1111;
        pos.x+=d;
        pos=rotZ(pos,(frac(abs(a.y)+time.x*.005)-.5)*.1);
        pos.x-=d;
        pos=rotY(pos,qid);
        pos.xz*=pos.xz;
    }

    pos+=rot3(pos,ang*float3(0,1,0)+length(pos)*.10-time.x*.01);
    pos=rotY(pos,ang*PI/180.);
    float3 q= smoothstep(0,1,saturate(length(pos)/15*(1+abs(pos.y))));
    pos=lerp(normalize(pos+noise3(float3(a,qid/10000.)))*3,pos,q);
    pos*=2;
   
    pos_color p;

    p.color = float4(float3(1,2,3),1)*.1+.0015;

    if (qid==0)
    {
        //hilight
        p.color*=1;
        p.pos=transform(float3(0,0,0),grid.zw,552);
        p.sz=2;
    }
    else
    {
        p.pos = transform_unisize(pos/16,grid.zw,4.);
        p.sz=2;
        p.color*=11/(abs(pos.y)*2+11.1);
    } 
    
    return p;
}

pos_color outer_space(float3 pos,float4 grid, uint qid)
{

    uint glow = qid%(64);

    pos=shp(grid.xy);
    pos+=.7*rot3(pos,31/pos+.1*noise3(pos*3));
    pos=lerp(normalize(pos)*33,pos,.45);;
    
    pos_color p;

    p.color = float4(float3(7,8,9),1)*.051+.0015;

    if (glow==0)
    {
        //hilight
        p.pos=transform(pos,grid.zw,102);
        p.color/=14;
        p.sz=2;
    }
    else
    {
        p.pos = transform_unisize(pos,grid.zw,1.);
        p.sz=1;
        p.color*=3.8*(hash(qid))+.01;
    } 
    
    return p;
}



pos_color floor_space(uint qid,float4 grid)
{
     uint glow = qid%11234;
     uint stars= qid%3400;
    uint cn=7;
    float a= (qid%cn);
    a=a*PI/180.;
    a*=360./cn;
    float t=time.x*.1;
    float3 pos = pillar(grid.xy,0,t*.005,.5)/5;
    float3 pos2=pos;

        pos.y*=.4*pow(length(pos),1)*4;
        pos.xz*=9;
        
        pos=rotY(pos,length(pos)*.3+a+time.x*.005);
        pos.y+=length(pos)/7;
        pos.y-=4.5;
        

       pos+=noise3(qid*float3(1,2,3)*11.1)*2;

        pos_color p;
        p.color = float4(1,2,3,1)/120.+.0015;

        if (stars==0)
        {
               p.pos = transform_unisize(pos,grid.zw,1.);
               p.sz=1;
        }
    else
    {
        float sz=18;
        if (glow==0) sz=619;
        p.pos = transform_unisize(pos,grid.zw,sz);
        p.sz=2;
    }
        
       

   p.color = float4(noise3_u(a*float3(1,2,3)+77+sin(pos2*11.4)),1)/50.+.00015;
   if (glow!=0&&stars!=0)
   {
     p.color.rgb*=noise(2*rotY(pos,length(pos)-time.x*.005))*3.2+.2;

   }
        p.color*=float4(1,2,3,1)/2;
        
        if (stars==0)
        {
            p.color*=540;
             p.sz=1;
        }

        if (glow==0) p.color*=4;

    //density compensation
  //  p.color/=min(pow(p.pos.w,1.1)*.1+1.5,5);
    
    return p;
}





pos_color pillars_array(uint qid,float4 grid)
{
    uint glow = qid%(10394);


     float t=time.x*.01;

         if (glow_p==0)
    {
        qid =qid*10394;
        t=0;
    }


     //if (glow ==0)t=0;


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
 p.color = float4(noise3_u(a*float3(13,15,12)*221+77+sin(pos2*1.4)),1)/30.+.0015;
    if (glow_p==0)
    {
        //hilight
        p.pos=transform(pos,grid.zw,92);
        p.color*=2;
     
         p.sz=172;
    }
    else
    {
        p.pos = transform_unisize(pos,grid.zw,1.5);
    //   p.color+=-noise(pos*.12*float3(1,2,3)+2)*.1;;

       // p.color +=min(0,sign(1./noise(-pos2*.2-2.6)))/91.;
   
         p.sz=1;

    }
   
    //density compensation
    p.color/=min(pow(p.pos.w,1.1)*.1+.5,11);
    return p;
}

pos_color sagittarius_v2 (uint qid,float div, float4 grid)
{
    pos_color outp;


    //percentage
    uint outer=612;
    uint floor_=23;
    uint star_=15;
    uint star_2=4957;
    uint big_hl=2006;

    float3 pos=0;
    
    outp = pillars_array(qid,grid);
 //   if (qid%outer==0) outp = outer_space(0, grid, qid);
//    if (qid%floor_==0) outp = floor_space(qid,grid);
 //   if (qid%star_==0) outp = star(qid,grid);
    
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
    float y2=pow((smoothf(grid.y)-.5)*2,3);
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
    //outp.color=float4(float3(.1,.1,.1)*noise3(pos*.1-.8),1)/3.5+.015;
    outp.color=.0125*float4(lerp(float3(1,0.2,0.1),float3(0.1,0.2,1),q)*(noise3(pos*1.1)*.5+.6),1);
    //outp.color=.5*float4(lerp(float3(1,.1,.1),float3(.1,.1,1),q),1)/23.5-.001;
    //outp.color+=float4(lerp(float3(1,.1,.1),float3(.1,.1,1),saturate(pos.yyy/4+.1)),1)/23.5-.001;

    //if (qid%big_hl==0) outp.color*=.35*1.5;

    if (qid%outer==0)
    {
        if (qid%123==0) outp.color=2.6/(length(gzw)+1)*float4(4,5,6,1)/6;
        else 
        outp.color+=.474*.7;     
    }
  
   // outp.color/=posT.w/15+.1;
    //if (qid%star_2==0&&qid%big_hl!=0) outp.color=(noise(pos)+1.2)*1.5;
     
    outp.color*=1.;
    return outp;
}*/

VS_OUTPUT VS(uint vID : SV_VertexID,uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    float div=1;
    //float4 grid = getGridP(vID, 1, int2(gX,gY));
    float4 grid;
    float2 map[6] = { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1 };
    grid.zw= map[vID % 6];
    grid.xy= float2(iID % gX, floor(iID / gX))/float2(gX,gY);
    float2 uv = grid.xy;
    
    uint qid = iID;
    
    pos_color p = sagittarius_v2(qid,div,grid);
    
    //density compensation
   // p.color/=min(pow(p.pos.w,1.1)*.1+.5,11);
    
    output.pos=p.pos;

    output.vnorm = 0;
    output.wpos = 0;
    output.vpos = 0;
    output.uv = grid.zw;
    output.id = float4(iID/div,0,0,0) ;
    output.color = p.color;
    output.sz1 = float4(p.sz,0,0,0);

    return output;
}
