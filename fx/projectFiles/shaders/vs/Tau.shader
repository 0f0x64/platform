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
    float4 base_color;
}

float toRad(float a)
{
    return a*PI/180.;
}

float smin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return lerp(b, a, h) - k * h * (1.0 - h);
}

float3 pillar(uint qid,uint iid,float2 grid,float a1, float t, float h1)
{
    float3 hs = float3(hash(iid/200.),hash(iid/140.),hash(iid/120.))-.5;
    float s=10;
    float3 pos;
    pos.xz=normalize(hs.xz)*s*(1-.5*cos(hs.y*PI));;
    pos.y=hs.y*s;
    pos.xz*=cos(pos.y/PI);
    pos*=3;
    float sg=sign(iid%2-.5);
    
    //pos+=noise3(pos/2);
    //pos=rotY(pos,length(pos.xz)+time.x/124*sg);
    //pos+=noise3(pos/3);
    pos.x+=s*1.5;
    pos.x*=sg;
    
    
    pos.xz=lerp(pos.xz,pos.xz*float2(-1,1),pow(5/length(pos.xz),8));
    //pos.y-=pow(pos.y/3.8,6);
    //pos.y/=1-pos.y;
    //pos+=log(pos);
    
   
    

       
    return pos/5;
}



pos_color CalcParticles(uint qid,uint iid,float4 grid)
{
     qid *= skipper;
     float t=time.x*.004;
     uint inStars = 3623;
     if (mode==1||iid%inStars==0)
     {
        t=0;
     }

    //pillars instances


    //calc
    float3 pos = pillar(qid,iid,grid.xy,0,t,0);
    float3 pos2=pos;
    
    //scatter
    
    //color
    pos_color p;
    p.rgba = float4(noise3_u(111+float3(113,11,111)*221+177+sin(pos2*.48)),1)/90.+.015;
    //p.rgba*=base_color/3;
    //p.rgba=lerp(p.rgba,p.rgba.bgra,.25+.5*sin(length(pos.y)*2));
    //p.rgba=lerp(p.rgba,p.rgba.grba,.5+.5*cos(length(pos)*1.01+2));
    //p.rgba.g+=.003;
    if (mode==1)
    {
        float s=hash(iid)*33+11;
        //s*=1.5;
        p.pos=transform(pos,grid.zw,s);
        p.rgba*=0;
        p.sz=172;
    }
    else
    {
        p.pos = transform_unisize(pos,grid.zw,1.75);
       //p.rgba=-noise(pos*.3+12)*.04+.02;;
       // p.rgba +=min(0,sign(1./noise(-pos2*.2-2.6)))/91.;
         p.sz=1;
         p.rgba*=1.2;

         if (iid%inStars==0)
         {
              p.pos = transform_unisize(pos,grid.zw,31.5);
               p.sz=2;
               p.rgba*=23;
         }

    }
      /*    if (iid==0)
         {
              p.pos = transform(0,grid.zw,16.5);
               p.sz=2;
               p.rgba*=(float4(5,-.1,-1,1));
               p.rgba*=10;
         }*/
  
   
    //density compensation
    //p.rgba/=min(pow(p.pos.w,1.1)*.21+.5,11);
    return p;
}

VS_OUTPUT_PARTICLE VS(uint vID : SV_VertexID,uint iID : SV_InstanceID)
{
    float4 grid = getGridInst(vID,iID,gX,gY); 
    pos_color p = CalcParticles(vID,iID,grid);
    VS_OUTPUT_PARTICLE output = { p.pos,grid.zw, p.rgba, p.sz};
    return output;
}
