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

float3 pillar(uint qid,uint iid,float2 grid,float a, float t, float h)
{
    float3 pos = float3(hash(iid/200.),hash(iid/140.),hash(iid/120.))-.5;
    float3 pos3=pos;
    pos=normalize(pos)*2;

    uint inStars = 10000;
    float t2= (iid%inStars==0||mode!=0) ? 0: time.x;

    pos+=pos*noise3(pos*3);

    pos*=4;
    
    a=hash(iid/1000.);
    pos=rot3(pos,pos/3);

    pos+= noise3(pos*2+t2/42)*.8;
    //pos = rot3(pos,noise3(pos.zyx*.6+float3(0,-t,0))/12);
    

    float3 pos2=pos;
    
    if (iid%3!=0)
    {
        pos=4/(abs(pos)+.03)*sign(pos);

        float3 hole=float3(0,0,0)*13;
        float dst=4/(distance(pos,hole));
        pos-=normalize(hole-pos)*pow(dst,4);
        

        pos=lerp(pos,normalize(pos)*42,.5+1.5*sin(length(pos/112)));
        
        pos+=3.4/rot3(pos,pow(pos,3)/1113);
        
        pos/=2;

        pos+=5/rot3(pos,noise3(sin(pos/4+time.x/540)));
        pos=rot3(pos,noise3(sin(pos/9+time.x/540)));
        pos+=1/rot3(pos,noise3(sin(pos/3+time.x/540)));

        //pos=lerp(pos,normalize(pos3)*22,saturate(length(pos/72)));


        
    }else{

    pos*=1+length(1/pos);
    pos/=3;
    pos=rot3(pos,length(pos)/3);

    }



    //pos*=.9;
    float dst=3;
/*    for (int i=0;i<32;i++)
    {
        float4 j=(i+1)*float4(11,12,13,14)+time.x*.003;
        float3 hole=float3(sin(j.x),cos(j.y),sin(j.z)*cos(j.w));
        hole=normalize(hole)*(8.3+.5*sin((pos)+time.x*.01));
        dst=3/(distance(pos,hole));
        pos-=sign(i%3-.5)*normalize(hole-pos)*pow(dst,3);
       // pos=rot3(pos,saturate((hole*pos)/.1)*12);
    }*/
  
    //pos+= noise3(pos/6)*.8;

    
    return pos/2;
}



pos_color CalcParticles(uint qid,uint iid,float4 grid)
{
     qid *= skipper;
     float t=time.x*.004;
     uint inStars = 10000;
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
    p.rgba = float4(noise3_u(111+float3(113,11,111)*221+177+sin(pos2*.48)),1)/90.+.0015;
    p.rgba*=base_color/3;
    p.rgba=lerp(p.rgba,p.rgba.bgra,.25+.5*sin(length(pos)*2));
    //p.rgba=lerp(p.rgba,p.rgba.grba,.5+.5*cos(length(pos)*1.01+2));
    p.rgba.g+=.003;
    if (mode==1)
    {
        float s=hash(iid)*33+11;
        //s*=1.5;
        p.pos=transform(pos,grid.zw,s);
        p.rgba*=1.3;
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
