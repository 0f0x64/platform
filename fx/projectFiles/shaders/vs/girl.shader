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
    float4 modelPos;
    float4 triCount;
    float4 brightness;
    float4 tickness;
}

struct Vertex {
    float4 pos;
};

// Our StructuredBuffer at t0
StructuredBuffer<Vertex> vbf : register(t0);

struct Index {
    float4 i;
};
 
// Our StructuredBuffer at t0
StructuredBuffer<Index> ibf : register(t1);

float toRad(float a)
{
    return a*PI/180.;
}

float smin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return lerp(b, a, h) - k * h * (1.0 - h);
}

float3 hash33(float3 p)
{
    return float3(hash(p.x),hash(p.y),hash(p.z))-.5;
}

float3 hash31(float p) {
   float3 p3 = frac(p * float3(.1031, .1030, .0973));
   p3 += dot(p3, p3.yzx + 33.33);
   return frac((p3.xxy + p3.yzz) * p3.zyx); 
}

float3 sm2(float x)
{
    return smoothstep(0,1,smoothstep(0,1,x));
}

float3 sm3(float3 x)
{
    return smoothstep(0,1,smoothstep(0,1,x));
}

float CalculateTriangleArea(float3 p0, float3 p1, float3 p2)
{
    float3 edge1 = p1 - p0;
    float3 edge2 = p2 - p0;
    
    // ƒлина векторного произведени€ дает площадь параллелограмма
    // ƒелим на 2, чтобы получить площадь треугольника
    return length(cross(edge1, edge2)) * 0.5f;
}

float3 getRandomPointInTriangle(float3 g, float3 g1, float3 g2, float r, int iid,float s)
{
    

    float2 randoms;
    //randoms.x = r; 
    randoms.x = hash(r); 
    randoms.y = r/12;//hash(randoms.x); 
    //randoms.y = hash(randoms.x); 
    //randoms.y = lerp(r/22.,hash(randoms.x),smoothstep(0,1,saturate(s/61)));; 
    //randoms.y = lerp(randoms.y,randoms.x,pow(randoms.y,.24));
    //randoms.y = lerp(randoms.y,randoms.x,smoothstep(0,1,saturate(s/151)));
    //randoms.y = lerp(randoms.y,randoms.x,pow(saturate(s/61),11));
    //randoms.y = randoms.x;

    float3 cp= (g+g1+g2)/3.;
    //randoms  = rotZ(float3(randoms-.5,0),time.x/10.)+.5;

    float sqrtR1 = sqrt(randoms.x);
    float u = 1.0f - sqrtR1;
    float v = randoms.y * sqrtR1;
    float3 result =  u * g + v * g1 + (1.0f - u - v) * g2;
    float3 rt = 2*rot3(result-cp,(noise(result/12.+(time.x/18.))*float3(4,5,3)))+cp;
    //result=lerp(result,rt,smoothstep(0,1,1-saturate(s/22)));
    
    
    //result+=hash33(result);
    //result = lerp(result,(g + g1 + g2) / 3.0f,0.5); 
    return result;
}

float3 CalculateNormal(float3 p0, float3 p1, float3 p2)
{
    // Ќаходим два вектора из одной точки
    float3 edge1 = p1 - p0;
    float3 edge2 = p2 - p0;
    
    // ¬ычисл€ем векторное произведение и нормализуем результат
    // ѕор€док (edge1, edge2) определ€ет направление нормали (правило правой руки)
    return normalize(cross(edge1, edge2));
}

float3 SphericalToCartesian(float3 p)
{
    float radius=p.x;
    float theta=p.y;
    float phi=p.z;
    float3 cartesian;
    // ¬ HLSL обычно Y Ч это верх (Up), поэтому:
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
    
    // ≈сли точка в центре координат, углы не определены
    if (radius <= 0.0) return float3(0, 0, 0);

    // theta = угол в плоскости XZ (азимут)
    // »спользуем atan2(z, x) дл€ полного диапазона [-PI, PI]
    float theta = atan2(p.z, p.x);
    
    // phi = угол от оси Y (зенит) [0, PI]
    // acos возвращает значени€ от 0 до PI
    float phi = acos(p.y / radius);
    
    return float3(radius, theta, phi);
}

pos_color CalcParticles(uint vid,uint iid,float4 grid)
{

    float3 pos;
    pos_color p1;

    //uint4 ind4 = meshInd[iid%4046];
    uint4 ind4 = ibf[iid%(int)triCount.x].i;
    uint ind[] = {ind4.x,ind4.y,ind4.z};
//  float3 _p0 = mesh[ind[0]].xyz;
//  float3 _p1 = mesh[ind[1]].xyz;
//  float3 _p2 = mesh[ind[2]].xyz;

    float3 _p0 = vbf[ind4.x].pos.xyz;
    float3 _p1 = vbf[ind4.y].pos.xyz;
    float3 _p2 = vbf[ind4.z].pos.xyz;


    float s =CalculateTriangleArea(_p0,_p1,_p2);
    pos = getRandomPointInTriangle(_p0,_p1,_p2,iid/11231.,iid,s);
    float3 nrml = CalculateNormal(_p0,_p1,_p2);
    pos+=nrml*tickness.xxx/100;

    //pos=_p0+normalize(hash33(iid/1213.*float3(1,2,3)))*11;
    float f= hash(iid/1231.)+.1;
    //pos+=nrml*(f)*4*(hash33(pos)+.3);
    //pos+=nrml*frac(time.x/13.+(iid/4046)/113.)*1;
    //pos-=nrml*noise(iid/123123.+time.x/23.)*2;

    float3 pt= mul((float3x3)view[0],float3(0,0,1));
    //float3 nrml_t= mul((float3x3)view[0],nrml);

    //if (dot(pt,nrml)>0) pos = _p0;
    //pos = lerp (pos,_p0,saturate(6*dot(pt,nrml)));
    bool hF = iid>1000000;
    float3 l=0;
    float lf=0;
    if (false)
    {
        float fhash = 1721.;
        float3 h=hash3(iid/(int)fhash)*PI;
        h.y=abs(h.y);
        h=rotX(h,hash(iid/(int)fhash)/1.2);
        pos=normalize(h);
        //grow
        lf =hash(iid/123.)*3.5;
        l=normalize(pos+float3(0,-.0,-.0))*lf;
        pos+=l;
        //gravity
        pos.y-=pow(length(l),2.2)*.7;

        float r=2.5;
        float pf=smoothstep(0,1,length(pos.xz)/5);
        pos=lerp(normalize(pos)*r*(pf*pf+1),pos,pf);

        //face occluder
        float a =atan2(pos.x,pos.y)/PI;
        float b = saturate(abs(a));
        pos=lerp(pos,rotY(pos,b*sign(-a)),saturate(pos.z/3));

        //neck occluder
        float3 hole=float3(0,-2.4,1);
        pos-=normalize(hole-pos)*pow(2/distance(pos,hole),.5);

        //
        //waves
        pos.z+=sin(-pos.y-time.x/12.)*lf/8;
        //pos.z-=max(-pow(pos.y,2),0)/4;
        //pos.xz*=1+pow((-pos.y+1),.4);

        //grow point
        pos=lerp(pos*.9,pos,saturate(lf/2));

        //noise
        pos+=noise3(pos*lerp(5,1,saturate(lf))+time.x/12)*max(lf/4,0);

        //neck occluder
        hole=float3(0,-12.4,0);
        pos-=2*normalize(hole-pos)*pow(5/distance(pos,hole),.95);

        pos=lerp(hash33(iid/1123.)+float3(0,3.,0),pos,saturate(lf/1.2));

        //divide forward and back
        pos.z+=sign(pos.z+1.5)*max(-sin(pos.y/3+1)*4-2.6,0);

        pos*=5.6;
        pos.z*=1.2;
        pos.x-=2.8;
        pos.z+=3;
        pos.y-=7;
        pos.y+=124;
    }



    if (mode==2)    
    {
        ind4 = ibf[vid/3].i;
        uint ind2[] = {ind4.x,ind4.y,ind4.z};

        pos = vbf[ind2[vid%3]].pos.xyz;
        p1.color=1.15;
        p1.sz=1.2;

    float3 _p0 = vbf[ind4.x].pos.xyz;
    float3 _p1 = vbf[ind4.y].pos.xyz;
    float3 _p2 = vbf[ind4.z].pos.xyz;
        float3 nrml = CalculateNormal(_p0,_p1,_p2);
        //pos-=nrml*1;
        //pos-=nrml*1;

        //pos.y-=124;
    }
    
    pos+=modelPos.xyz;
    pos.y-=-91;
    pos*=.01;

    //p1.pos=float4(pos,1);
    //pos.y-=11;
    //pos.z-=1;
    //pos = mesh[iid%2025].xyz*.1;

    //if (mode==0) 

    /*
    //hands
    {
    float3 pos2=pos;
    pos2.x=abs(pos2.x);
    float3 hole=float3(12,+2.4,-2);
    float3 hole2=float3(4,+4.,-2);
    float c=lerp(1,0,saturate(distance(pos2,hole)/8));
    pos=lerp(pos,rotZ(pos-hole2,sin(time.x/6)/6)+hole2,c);
    }

    //head
    {
    float3 pos2=pos;
    pos2.x=abs(pos2.x);
    float3 hole=float3(0,12.4,0);
    float3 hole2=float3(0,+12.4,0);
    float c=lerp(1,0,saturate(distance(pos2,hole)/6));
    pos=lerp(pos,rotY(pos-hole2,sin(time.x/22)/1)+hole2,c);
    pos=lerp(pos,rotX(pos-hole2,sin(time.x/21)/3)+hole2,c);
    }

    //body
    {
    float3 pos2=pos;
    pos2.x=abs(pos2.x);
    float3 hole=float3(0,-2.4,0);
    float3 hole2=float3(0,-2.4,0);
    float c=lerp(1,0,saturate(distance(pos2,hole)/14));
    pos=lerp(pos,rotY(pos-hole2,sin(time.x/16)/5)+hole2,c);
    pos=lerp(pos,rotX(pos-hole2,sin(time.x/15)/7)+hole2,c);
    }
    */

   // pos.y+=sin(time.x/8);
    //pos.x+=sin(time.x/31);
      //  pos=rotY(pos,time.y/272.);

    if (mode==0)    
    {
            p1.color=pow(s,.125)/4.;

            float sz = 4.2;

            if (hF)
            {
                sz=clamp(9./(lf),1.2,9);
                p1.color/=5;
              //  p1.color*=(9+6*noise(pos*3))/8;
             // p1.color*=12;
              
            }
       

            p1.pos = transform_unisize(pos,grid.zw,sz);
            p1.sz=1.2;
            p1.color/=.2*p1.pos.w*3;

            if (hF) {
                p1.color*=1+lf;
            p1.color/=.29*p1.pos.w;
            }

    }

    if (mode==2)
    {
        float4 posT = mul(float4(pos,1), view[0]);
        posT = mul(posT, proj[0]);
        p1.pos=posT;
    }
    
    p1.color.rgb*=brightness.rrr/100.;
    p1.color.rgb*=float3(1,2,3)/4;
    //pos.y-=55*sin(time.x/12.)*c;

    /*else
    {
        p1.pos = transform(pos,grid.zw,21.2);
        p1.color=.15;
        p1.sz=1.2;
        p1.color*=0;
       // p1.color=lerp(p1.color,1,form);

    }*/

    //density compensation
    //p1.color/=min(pow(p1.pos.w,1.1)*.21+.5,11);
    return p1;
}

#include <../lib/particleVS_main2.shader>
