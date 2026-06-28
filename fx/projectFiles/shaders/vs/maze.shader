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

float quantize2(float x, float q)
{
    return floor(x*q)/q;
}

float3 torus(float u, float v, float R, float r) {
    float TWO_PI = 6.283185307;
    float theta = u * TWO_PI/3; // Angle around the major ring
    float phi = v * TWO_PI;   // Angle around the minor tube

    float x = (R + r * cos(phi)) * cos(theta);
    float y = (R + r * cos(phi)) * sin(theta);
    float z = r * sin(phi);

    return float3(x, y, z);

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
    float r = 3.;

    float p=1;
    float q=3.;

    float3 pos = getTpos(a,R,p,q);
    float3 T = pos - getTpos(a+float2(.01,0),R,p,q);
    float3 N = pos+T;
                    
    float3 B = cross(T,N);
    N = cross(B,T);
    
    B= normalize(B);
    N= normalize(N);
    float t=time.x/270;
    pos += r * (cos(a.y) * N + sin(a.y) * B);
    //pos=rot3(pos,t);
    return pos;
}

float3 SphericalToCartesian(float3 p)
{
    float radius=p.x;
    float theta=p.y;
    float phi=p.z;
    float3 cartesian;
    // В HLSL обычно Y — это верх (Up), поэтому:
    // x = r * sin(phi) * cos(theta)
    // y = r * cos(phi)
    // z = r * sin(phi) * sin(theta)
    
    float sinPhi = sin(phi);
    
    cartesian.x = radius * sinPhi * cos(theta);
    cartesian.y = radius * cos(phi);
    cartesian.z = radius * sinPhi * sin(theta);
    
    return cartesian;
}

float3 CartesianToSpherical(float3 p)
{
    float radius = length(p);
    
    // Если точка в центре координат, углы не определены
    if (radius <= 0.0) return float3(0, 0, 0);

    // theta = угол в плоскости XZ (азимут)
    // Используем atan2(z, x) для полного диапазона [-PI, PI]
    float theta = atan2(p.z, p.x);
    
    // phi = угол от оси Y (зенит) [0, PI]
    // acos возвращает значения от 0 до PI
    float phi = acos(p.y / radius);
    
    return float3(radius, theta, phi);
}


float3 pillar(uint qid,uint iid,float2 grid,float a, float t, float h)
{
    float3 sk[44];
    int cn=44;
    //iid=iid/55;
   /* for (int i=0;i<44;i++)
    {
        sk[i].x=sin(hash(i+11.13)*(time.y/1171+2111))*1;
        sk[i].y=cos(hash(i+11.13)*(time.y/1171+2111))*1;
        sk[i].y/=2;
        sk[i].z=cos(hash(i+11.13)*(time.y/1171+2111))*1;
        sk[i].xz *= 1-hash(i)/3;
        sk[i].z*=2;
        sk[i].z+=1;
        sk[i].y+=.3;
        //sk[i].y+=(hash(i)%2)/11.;
    }*/

        for (int i=0;i<44;i++)
    {
        sk[i].x=sin(hash(i+121.13)*(time.y/840+2111))*1;
        sk[i].y=sin(hash(i+111.13)*(time.y/212+2111))*1;
        sk[i].z=sin(hash(i+151.13)*(time.y/522+2111))*1;
    }


    float3 pos = 0;
    pos=sk[iid%cn];

    float3 form=.3/normalize(noise3(iid/123.*float3(1.1,2.3,3.5)))/275;
   // form=rot3(form,noise3(pos/iid));
    pos+=form;
    float ind=hash(iid/123.)*(cn-1.);
    float3 pos2=sk[ind];
    //float f=pow(1./distance(pos,pos2)*1.065*hash(iid/123.),115);
    float f=pow(1./(distance(pos,pos2)),115);
    float f2=f;
    f=saturate(f);
    f*=hash(iid/11112.);
    //f=pow(f,4);
    f=distance(pos,pos2);
    //if (f<.5) f=0;
    

    //f=min(f,1.3);
    float3 dst=0;
    //if (f>.6) dst=noise3(pos*112)/1;
    //if (f<.1) f=0;
    pos=lerp(pos,pos2,hash(f)*pow(f,.15));
    //pos+=noise3(4*f/pos+time.x/252)*min(pow(f,12),1);
    pos+=noise3(4*f/pos+time.x/52)*pow((f),12);
    
    
    if (f>.9) 
    {
        pos=normalize(noise3(float(iid)*float3(11,12,23)/11111.));
        //pos*=2;
        pos*=noise3(pos*5)+3;
        pos*=22;
    }
    //if (f==0) pos+=2/(noise3(pos*122))*(1-f)/20;
    //if (f==0) pos+=rot3(pos,1/(noise3(pos*122))*(1-f)/1222);
    //if (f<.1) pos+=noise3(pos*2222)*f*10;
    //if (f==0) pos=lerp(pos,sk[ind/23.],hash(ind/13.));
    //if (f==0) pos+=rot3(noise3(pos*322),1/pos);
    //if (f>1.1) pos=noise3(float(iid)*float3(5.1,3.1,7.1)/1112.)*3;

    //return pos*65;
    return pos*5;
}

float3 safe_frac_centered(float3 v)
{
    // floor(v + 0.5) сдвигает сетку так, чтобы центр ячейки был в нуле,
    // а вычитание из исходного вектора дает идеальный диапазон [-0.5, 0.5]
    return v - floor(v + 0.5f);
}

pos_color CalcParticles(uint qid,uint iid,float4 grid)
{
     qid *= skipper;
     float t=time.x*.004;
     uint inStars = 1232*1213;
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
    p.color.a=1;
    p.color.rgb = noise3_u(pos*14*float3(12,55,112))/31+float3(1,3,5)/52;
    
    //p.color*=.5;
//    p.color*=base_color*(pow(length(pos)/16,4)+.1);
    //p.color*=1+sin(grid.x*PI*8);
    p.color=lerp(p.color,p.color.bgra,sin(length(pos)));
//    p.color=lerp(p.color,base_color/144,1-saturate(pow(length(pos)/6,11)));
//pos+=noise(pos/12)*12-6;
pos*=.75;
// 1. Извлекаем векторы осей из первых трех строк матрицы view
    

     
    if (mode==1)
    {
        float s=hash(iid)*33+11;
        s=noise(iid)*62+11;
        //s*=1.5;
        p.pos=transform(pos,grid.zw,s);
        p.color*=0.1;
        p.sz=172;
    }
    else
    {
        p.pos = transform(pos,grid.zw,1.1);
       //p.color=-noise(pos*.3+12)*.04+.02;;
       // p.color +=min(0,sign(1./noise(-pos2*.2-2.6)))/91.;
         p.sz=2;
         p.color*=5;

         if (iid%inStars==0)
         {
              p.pos = transform_unisize(pos,grid.zw,75.5);
               p.sz=2;
               p.color*=2;
         } 

    }
      /*    if (iid==0)
         {
              p.pos = transform(0,grid.zw,16.5);
               p.sz=2;
               p.color*=(float4(5,-.1,-1,1));
               p.color*=10;
         }*/
  
         

    //density compensation
    if (mode==0)
    {
    p.color*=1*saturate(p.pos.w/11);
    //p.color*=0;

    }

    if (mode==1)
    {
    //p.color*=.3*saturate(21/p.pos.w);
    //p.color*=1*saturate(p.pos.w/1);
    //p.color=.02;
    }
    
   // p.color/=min(pow(p.pos.w,.5)*.1+1.5,5);
    return p;
}

#include <../lib/particleVS_main2.shader>
