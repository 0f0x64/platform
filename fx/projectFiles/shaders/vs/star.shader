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
    float4 PosRad;
    int triMode;
}

float toRad(float a)
{
    return a*PI/180.;
}

float3 rand3D(float3 p) 
{
    float3 s = float3(
        dot(p, float3(127.1, 311.7, 74.7)), 
        dot(p, float3(269.5, 183.3, 246.1)),
        dot(p, float3(113.5, 271.9, 124.3))
    );
    return frac(sin(s) * 43758.5453);
}

float4 cells3D(float3 localPos)
{   
    // Масштабируем трехмерное пространство
    localPos *= 10.0;
    
    float3 iuv = floor(localPos);
    float3 guv = frac(localPos);

    float mDist = 1.0;
        
    float3 mPos = 0;
    // Цикл по соседним ячейкам в 3D (3x3x3 = 27 итераций)
    for (float z = -1.0; z <= 1.0; z++) 
    {
        for (float y = -1.0; y <= 1.0; y++) 
        {
            for (float x = -1.0; x <= 1.0; x++) 
            {            
                float3 neighbor = float3(x, y, z); 
                float3 pointPos = rand3D(iuv + neighbor);			
                
                // Анимация движения точек внутри своих мини-кубов
                pointPos = 0.5 + 0.5 * sin(time.x / 342.0 + 6.2831 * pointPos);		
                
                
                // Вектор разности в 3D
                float3 diff = neighbor + pointPos - guv;            
                
                // Расчет трехмерной дистанции с вашей кастомной вариацией масштаба ячеек
                float dist = length(diff) * (1.0 + rand3D(iuv + neighbor + 100.0).x * 0.25);
                if (dist<mDist) mPos = diff;
                
                //if (dist<mDist) mPos = pointPos - guv;
                //if (dist<mDist) mPos = neighbor - guv;

                mDist = min(mDist, dist);
            }
        }
    }
    
    // Эффект свечения / метасфер
    //mDist = 0.1 / (1.0 - clamp(mDist, 0.0, 1.0));       
    mDist = 0.1 / (1.0 - mDist);       
    return float4(mPos,mDist);
    
    // Смешивание интенсивности
    float color = lerp(0.0, 1.0, mDist) * 1.5;        

    return color;    
} 

float3 pillar(uint qid,uint iid,float2 grid,float a, float t, float h)
{
    float3 h2=float3(hash(iid/200.),hash(iid/140.),hash(iid/120.))-.5;
    float3 pos = h2;
    float3 k = float3(iid/123.,iid/234.,iid/345.)/13.;
    //k=noise3(111/(k+1))*75;
    pos=noise3(k);
    pos=normalize(pos)*1.;

    pos*=1+rot3(pos,noise3(pos*4.5)*17)/4;
    pos=rot3(pos,noise3(pos*2-time.x*.0005)*2);
    pos=rot3(pos,noise3(pos/222)*2);

    pos*=.81+pow(.65/(frac(length(pos/2)*7-time.x*0.0005)+.017),.75)*.1;


    float3 pos2=pos;
    
        //pos+=noise3(pos*12)*1.;
    //pos=noise3(h2*22);
    pos*=1+h2*frac(time.xxx*.001+h2*222)/(length(pos*42)+6);
    float4 c = cells3D(pos/3.);
    //pos=lerp(pos,pos+c.xyz,.1/c.w);
    pos+=.3*c.w*normalize(pos);
    //pos+=c.xyz*(.1/c.w);
    //pos+= pos*c.xyz/(c.w+7);
    pos=rot3(pos,time.xxx/1522.+length(noise3(pos)));
    //pos2=pos+normalize(pos)*noise(frac(pos+time.x/222));
        //pos+=normalize(pos)*cells3D(pos2/3-time.x/111).w*.2;
//        pos=rot3(pos,noise3(pos*3-time.x/12)/12);
        //pos=normalize(pos)*2;
      //pos=pos*.2+normalize(pos)*.6;
        //pos=pos*frac(time.xxx*.005+pos)*.2+normalize(pos);
    
    return pos/2.32;
}

float4 pillar3(uint qid,uint iid,float2 grid,float a, float t, float h)
{
    float3 pos = float3(hash(iid/200.),hash(iid/140.),hash(iid/120.))-.5;
    pos=normalize(pos)*2;

    pos+=pos*noise3(pos);
    pos*=4;
    a=hash(iid/1000.);
    pos=rot3(pos,pos/3);
    pos+= noise3(pos)*.8;
    //pos = rot3(pos,noise3(pos.zyx*1.6+float3(0,-t,0))/12);
    float3 pos2=pos;
    
        pos=4/(abs(pos)+.03)*sign(pos);
        float dst=7/(length(pos));
        pos+=normalize(pos)*pow(dst,4);

        pos=lerp(pos,normalize(pos)*22,saturate(length(pos/82)));

        pos+=3/(rot3(pos,pos/3));
        float tt=frac(length(pos)+time.x/40);
        pos+=normalize(pos)*tt;
        pos*=1+frac(length(pos))/5;
        
        pos/=3;

                        float ttt=pow(length(pos)/8,11.5);
       //pos*=4;
       //pos-=22221/pow(pos,3);
       //pos/=6;
       //pos+=normalize(pos)*frac(length(pos2)+time.x/22)*5;
   
    return float4(pos/2,ttt);
}

pos_color2 CalcParticles(uint qid,uint iid,float4 grid)
{
     qid *= skipper;
     float t=time.x*.004;
     uint inStars = 300;
     if (mode==1||iid%inStars==0)
     {
        t=0;
     }

    //pillars instances


    //calc
    //float3 pos = pillar(qid,iid,grid.xy,0,t,0);
    float3 pos = 0;

    float phi = grid.x * 2.0f * PI; 
    float theta = grid.y * PI;
    float3 spherePos;
    pos.x = sin(theta) * cos(phi);
    pos.y = cos(theta); // Ось Y направлена вверх (полюс)
    pos.z = sin(theta) * sin(phi);

   
    float4 pw=0;

     if (triMode==1)
     {
         pw=pillar3(qid,iid,grid.xy,0,t,0)*.21;
     pos = pw.xyz;
     pos*=2.2;
     }


    pos*=PosRad.w;
    float3 pos0 = pos;    

    pos.xyz+=PosRad.xyz;

    float3 pos2=pos;
    
    //scatter
    
    //color
    pos_color2 p;
    p.wpos = float4(pos0,1);
    p.color = float4(noise3_u(111+float3(113,11,111)*221+177+sin(pos2*.48)),1)/110.+.0015;
    p.color.rgb*=float3(7,3,1)*2;
    //p.color=lerp(p.color,p.color.bgra,saturate(pow(length(pos)/13,13)));
    if (triMode==1)
    {
    //p.color=lerp(p.color,p.color.bgra,saturate(pow(length(pos)/13,13)));

        if (mode==1)
        {
            float s=hash(iid)*33+11;
            //s*=1.5;
            p.pos=transform(pos,grid.zw,s);
            p.color*=1.3;
            p.sz=172;
            
        }
        else
        {
            p.pos = transform_unisize(pos,grid.zw,1.75);
           //p.color=-noise(pos*.3+12)*.04+.02;;
           // p.color +=min(0,sign(1./noise(-pos2*.2-2.6)))/91.;
             p.sz=1;
             p.color*=1.2;
             
             if (iid%2111==0)
             {
                  pos.xyz = float3(hash(iid/200.),hash(iid/140.),hash(iid/120.))-.5;
                  pos*=60;
                  pos+=normalize(pos)*4;
                  float tt=frac(length(pos)+time.x/140);
                  pos+=normalize(pos)*tt*42;
                  //pos

                  p.pos = transform_unisize(pos,grid.zw,151.5);
                   p.sz=2;
                   p.color*=3;
                   p.color*=1-tt;
             }else{
                 //p.color*=pow(saturate(1-length(pos)/72),.5);
                 //p.color*=saturate(1-pw.w);
             }

        }  
        //p.color=0;
    }

    if (triMode==0)
    {
        float4 posT = mul(float4(pos,1), view[0]);
        posT = mul(posT, proj[0]);
        p.pos=posT;
        p.color=base_color;
    }

    //p.color=10;
    //p.sz=1;

      /*    if (iid==0)
         {
              p.pos = transform(0,grid.zw,16.5);
               p.sz=2;
               p.color*=(float4(5,-.1,-1,1));
               p.color*=10;
         }*/
  
   
    //density compensation
    //p.color/=min(pow(p.pos.w,1.1)*.21+.5,1);
        
    return p;
}

//[
VS_OUTPUT_PARTICLE2 VS(uint vID : SV_VertexID,uint iID : SV_InstanceID)
{

    float4 grid = getGridInst2(vID,iID,gX,gY); 

    pos_color2 p = CalcParticles(vID,iID,grid);
    
    VS_OUTPUT_PARTICLE2 output ;

    output.pos = p.pos;
    output.wpos = p.wpos;
    if (triMode==0)
    {
    output.uv = grid.xy;
    }
    else{
        output.uv = grid.zw;
    }
    output.color = p.color;
    output.size = p.sz; 
    //output = { p.pos,p.wpos,grid.xy, p.color, p.sz};
    return output;

}
//]
