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
}

float toRad(float a)
{
    return a*PI/180.;
}

float3 n3(uint iid)
{
    float q=1;
    float a=11234.123/q;
    float b=12345.234/q;
    float c=13456.345/q;
    float v1 = hash(iid/a);
    float v2 = hash(iid/b);
    float v3 = hash(iid/c);
    return float3(v1,v2,v3);
}

float quantize2(float x, float q)
{
    return floor(x*q)/q;
}

float2 gety(float y, int branch_count,float power)
{
    float s=2;
    float y2=pow(y,power);
    float y0=pow(s,floor(y2*branch_count));
    float y1=pow(s,floor(y2*branch_count)+1);
    return float2(y0,y1);
}

float3 pillar(uint qid,uint iid,float2 grid,float a, float t, float h)
{
//grid.y+=.015+noise(grid.x*PI*11);
//grid.x+=.015*noise(grid.y*PI*14);

    //grid.y+=time.x*.00005;
    grid=frac(grid);
    //grid.y*=2;
    
    
grid.y=pow(grid.y,.65);
//grid.y-=1-time.y*.001;
//grid.y=pow(grid.y,1.65);

//grid.y-=.2;


    int branch_count =3;
    float power= 2.3;
    float2 yy=gety(grid.y,branch_count,power);
    float3 pos = float3(grid.x-.5,grid.y,0);

    pos.x=quantize2(grid.x,floor(yy.x))+.5/(floor(yy.x));

    float3 pos2 = float3(grid.x-.5,grid.y,0);

    pos2.x=quantize2(grid.x,floor(yy.y))+.5/(floor(yy.y)); 

    float q= frac(pow(grid.y,power)*branch_count);

    q=smoothstep(0,1,q);
    //q=smoothstep(0,1,q);
//    pos.z+=noise(pos.x*12+54)/2*grid.y;
  //  pos2.z+=noise(pos2.x*12+54)/2*grid.y;


    pos=lerp(pos,pos2,q);
        //pos.z+=noise(pos.x)/2;
        //pos=rotX(pos,pow(pos.y/3,2));
    


    pos.x-=.5;
    
    float3 ofs3=float3(quantize2((grid.x),3)-.33,0,0);
    pos=ofs3+rotY(pos-ofs3,pow(grid.y,4)*(pos.x*2+2));
    
    pos.x/=2;
    pos=rotY(pos,sin(grid.y*12));
    
    //pos=rotY(pos,noise(grid.y*33));

    //pos.x/=2;

    //pos.y-=pow(abs(pos.x*3),5);
    //pos.y*=2-hash(quantize2(iid%15,3)+1113);
    //pos.y*=2;
    //pos.x*=1.4-hash(quantize2(iid%15,3)+43);
    //pos.z*=1.4-hash(quantize2(iid%14,3)+63);
    float3 ofs = float3(
        hash(iid*2),
        hash(iid*3),
        hash(iid*4)
    );
    ofs-=.5;
    ofs*=(1-pow(grid.y,2.07-power))/16.;

    pos+=ofs;

    pos=rotZ(pos,pos.x*PI*.4);
    float c=3;
    pos=rotY(pos,(iid%c)*PI/c);
    pos.y-=pow(length(pos.xz),1.7)*1.4;
    
    //float3 pos2 = n3(iid/pow(2,branch_count+1))-.5;
    //pos=lerp(pos,pos2,frac(y));


    float3 ofs2 = float3(
        hash(iid*2),
        hash(iid*3)-.2,
        hash(iid*4)
    );
    ofs2-=.5;
    ofs2=normalize(ofs2);
    ofs2.y-=.4;
    ofs2.y*=2;

    //pos+=ofs2*(pow(grid.y,65))*.4;

//        pos.x=0;
//        pos.y=(frac(y))*6;
  //      pos.z=0;

    //    pos=rotZ(pos,y/2.);
    pos.y/=1.5;
    pos+=noise3(pos*253*grid.y+222)*pow(grid.y,2)/5;
    pos.y-=.25;
    float c2=1+saturate(-pos.y-.1)*22;
    
    pos.y-=saturate(-pos.y-.1)*noise(pos*2+atan2(pos.x,pos.z)*3);
    pos.xz*=c2;
    return pos*8;
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
    float4 prgba = float4(noise3_u(pos2/.4)*float3(3.4,3.5,1.),1)/74.+float4(0,.015,0,0);
    float4 prgba2 = float4(noise3_u(pos2/.4)*float3(1.4,.5,0.),1)/4.+.0015;    
    p.rgba = lerp(prgba,prgba2,saturate(.1-pos.y/13));
    //p.rgba*=float4(1,5,1,1)*12;
    //p.rgba=3;

    if (mode==1)
    {
        p.pos=transform(pos,grid.zw,22);
        p.rgba*=pow(pos.y+2,2)/20;
        p.sz=172;
    }
    else
    {
        p.pos = transform_unisize(pos,grid.zw,1.5);
       //p.rgba=-noise(pos*.3+12)*.04+.02;;
       // p.rgba +=min(0,sign(1./noise(-pos2*.2-2.6)))/91.;
         p.sz=1;

         if (iid%inStars==0)
         {
         //     p.pos = transform_unisize(pos,grid.zw,11.5);
           //    p.sz=2;
               //p.rgba*=113;
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
