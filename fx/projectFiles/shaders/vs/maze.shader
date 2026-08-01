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
    float4 heroPosition;
    float4 base_color;
    float4 basePoint[3500];
    int particlesCount;
    int basePointsCount;


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

float3 pillar2(float3 pp2,uint qid,uint iid,float2 grid,float a, float t, float h)
{
    float3 pos=pp2;
    //float3 pos = float3(hash(iid/200.),hash(iid/140.),hash(iid/120.))-.5;
    uint inStars = 1232*2;

    float3 pos3=pos;
    float l=hash(iid%15)*20+20;
    pos=torusKnot(grid*PI*2);
    pos=rot3(pos,pos/12);
    pos=rot3(pos,(iid%15)/3*float3(4,5,6));
    
    pos*=5.6;

    pos+=rot3(pos,noise3(pos/10+1122));
    pos=rot3(pos,noise3(pos/5+1122)/5);
    pos+=noise3(pos/4+112)*6;

    float dst=3;
    for (int i=0;i<32;i++)
    {
        float4 j=(i+1)*float4(5,7,8,14)+0*.001;
        float3 hole=float3(sin(j.x),cos(j.y),sin(j.z)*cos(j.w));
        hole=normalize(hole)*62;
        dst=165/(distance(pos,hole));
        pos-=normalize(hole-pos)*pow(dst,1);
    }

    pos=CartesianToSpherical(pos);
    pos.x+=noise3(pos/15)*22;
    pos.x+=noise3(pos/15)*22;
    pos=SphericalToCartesian(pos); 
    
    for (int i=0;i<32;i++)
    {
        float4 j=(i+1)*float4(5,7,8,14)+t*.001;
        float3 hole=float3(sin(j.x),cos(j.y),sin(j.z)*cos(j.w));
        hole=normalize(hole)*62;
        dst=192/(distance(pos,hole));
        pos-=normalize(hole-pos)*pow(dst,1);
    }
    pos*=1-noise3(pos/5)*pow(length(pos)/422,2.6);
    //pos*=1+1/noise3(pos/3)*pow(length(pos)/422,5);
    
    return pos/6;
}

float3 pillar(uint qid,uint iid,float2 grid,float a, float t, float h)
{

float pcount = 3725442./14./6.;
float link =(iid%pcount)/pcount;
//-----

    float3 sk[44];
    int cn=7;
    float d=11111.;
    for (int i=0;i<14;i++)
    {
        sk[i].x=sin(hash(i+1.13)*(time.y/840+2111))*1;
        sk[i].y=sin(hash(i+1.13)*(time.y/212+2111))*1;
        sk[i].z=sin(hash(i+1.13)*(time.y/522+2111))*1;
      //  sk[i].x=noise((i/d+1.13)*(time.y/840+2111))*2;
       // sk[i].y=noise((i/d+2.13)*(time.y/212+2111))*2;
       // sk[i].z=noise((i/d+1.33)*(time.y/522+2111))*2;

    }



    float3 pos = sk[iid%cn];

 
//pos.y/=3;

float random_val = hash(float(iid)/1231231. + 341.123); // Изменили сид, чтобы не двоило с циклом
int ind = int(floor(random_val * float(cn-1))); 
float3 pos2 = sk[ind];

//float f = frac(noise3_u(iid/12311.)*distance(pos,pos2));
//float f = noise_u(iid);
float f = frac(iid/1.);
//float3 form = noise3(iid/float3(15,17,23)+31.123);
//form=normalize(form)/14;
//pos+=form;
//pos2+=form;

link=smoothstep(0,1,smoothstep(0,1,link));
float link2=frac(link*4);
pos = lerp(pos, pos2, link2);
float factor = sqrt(1-pow(link*2-1,2)); 
//factor = 1-abs(link*2-1);
//pos+=factor*noise3(pos*24+link*22+iid/2222222.)/14;   
pos+=pow(factor,2)*noise3(pos/link2*4)/cn;   
//pos = lerp(p*2,pos, saturate(2/(distance(pos,p)*2+.1)));
//pos+=1/p*pow(distance(pos,p),4)/5;

pos+=noise3(pos*2);

float3 p=pillar2(pos,qid,iid,grid,a,0,h);
//p=normalize(p);
//p=p/132+normalize(p)*2;
//p+=normalize(p)*3;
p/=38;
p+=normalize(p)*2;

//if (distance(pos,pos2)>.41) pos=p;
pos=lerp(pos,p,saturate(1/pow(distance(pos,pos2),22)/11));



    pos*=5;
    /*if (f>.9) 
    {
        pos=normalize(noise3(float(iid)*float3(11,12,23)/11111.));
        //pos*=2;
        pos*=noise3(pos*5)+3;
        pos*=22;
    }*/

    //if (f==0) pos+=2/(noise3(pos*122))*(1-f)/20;
    //if (f==0) pos+=rot3(pos,1/(noise3(pos*122))*(1-f)/1222);
    //if (f<.1) pos+=noise3(pos*2222)*f*10;
    //if (f==0) pos=lerp(pos,sk[ind/23.],hash(ind/13.));
    //if (f==0) pos+=rot3(noise3(pos*322),1/pos);
    //if (f>1.1) pos=noise3(float(iid)*float3(5.1,3.1,7.1)/1112.)*3;

    //return pos*65;
    return pos*5;
}

float3 pillar3(uint qid,uint iid,float2 grid,float a, float t, float h)
{


    int pc = basePointsCount-1;

    float ind = (pc*iid/(float)particlesCount);
    float3 pos = basePoint[int(ind)];
    float3 pos3=pos;
    float3 pos2 = basePoint[int(ind)+1];
    pos=lerp(pos,pos2,frac(ind));
    float3 pos_m=pos;
    //pos+=normalize(noise3(pos*1.+frac(iid/11.))/12;
    //pos+=noise(((iid*4)%particlesCount)-iid)/4;
    //pos+=2/(noise3(pos*2-32*(iid*3)%particlesCount)+3)-.7;
    //pos+=.01/sin(iid/1123.)*noise3(1./pos);

    
    float num = iid%7;
    float num2 = iid%(particlesCount/50.);
    //pos+=.0025/noise3(iid/213.*float3(4,5,6))/noise3(pos*2)/3;
 
    float3 ofs=pow(noise3(pos/num+pos/num2)*2,3)*2;
    ofs+=ofs/noise3(pos*12+33*num2+num)/51.;
    ofs=rotZ(ofs,num2/3222.);
    ofs=rotZ(ofs,(iid/(float)particlesCount)*22);
    pos+=ofs;
    //pos+=noise3(pos*5+time.x/23.)/8.;
    //pos+=pow(num+(noise3(pos*11))/4,1);
    float i01 = iid/(float)particlesCount;
    float ic = pow(1-sin(i01*PI),54)*52;
    pos+=noise3(pos*5)*ic;
    if (i01<.005) pos = basePoint[0]+normalize(noise3(pos*22))*2.;
    if (i01>.995) pos = basePoint[basePointsCount-1]+normalize(noise3(pos*22))*2.;
    //pos+=noise3(pos*3)*pow(distance(heroPosition,pos)/18.,5)*5;

    return pos;
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
     iid *= skipper;
     float t=time.x*.004;
     uint inStars = 1232*1213;
     if (mode==1||iid%inStars==0)
     {
        t=0;
     }

    //pillars instances


    //calc
    float3 pos = pillar3(qid,iid,grid.xy,0,t,0);
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
//pos*=.75;
// 1. Извлекаем векторы осей из первых трех строк матрицы view
    

     
    if (mode==1)
    {
        float s=hash(iid)*33+11;
        s=noise(iid)*62+11;
        //s*=1.5;
        p.pos=transform(pos,grid.zw,s);
        p.color*=.7;
        p.sz=172;
    }
    else
    {
        p.pos = transform(pos,grid.zw,1.1);
       //p.color=-noise(pos*.3+12)*.04+.02;;
       // p.color +=min(0,sign(1./noise(-pos2*.2-2.6)))/91.;
         p.sz=2;
         p.color*=2;

/*         if (iid%inStars==0)
         {
              p.pos = transform_unisize(pos,grid.zw,75.5);
               p.sz=2;
               p.color*=2;
         } 
         */
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
