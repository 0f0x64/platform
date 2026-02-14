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
    float4 mesh[4000];
}

cbuffer par2 : register(b4)
{
    float4 meshInd[4095];
}

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
    randoms.y = r/22;//hash(randoms.x); 
    //randoms.y = hash(randoms.x); 
    //randoms.y = lerp(r/22.,hash(randoms.x),smoothstep(0,1,saturate(s/151)));; 
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
    float3 rt = 5*rot3(result-cp,(noise(result/12.+(time.x/48.))*float3(4,5,3)))+cp;
    result=lerp(result,rt,smoothstep(0,1,1-saturate(s/22)));
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

    uint4 ind4 = meshInd[iid%4046];
    uint ind[] = {ind4.x,ind4.y,ind4.z};
    float3 _p0 = mesh[ind[0]].xyz;
    float3 _p1 = mesh[ind[1]].xyz;
    float3 _p2 = mesh[ind[2]].xyz;

    float s =CalculateTriangleArea(_p0,_p1,_p2);
    pos = getRandomPointInTriangle(_p0,_p1,_p2,iid/11231.,iid,s);
    float3 nrml = CalculateNormal(_p0,_p1,_p2);
    pos+=nrml;

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
    if (hF)
    {
        //start hemisphere
        float fhash = 5421.;
        float3 h=hash3(iid/fhash)*PI;
        //h.z/=.5;
        h=SphericalToCartesian(h);
        h.y=abs(h.y);
        pos=normalize(h);
        pos=rotX(pos,-.3);
        //grow
        l=normalize(pos+float3(0,-.15,-.75))*hash(iid/123.)*1.;
        l*=hash(iid/fhash)+1;
        float l2 = hash(iid/131.);
        pos+=l;
        pos+=noise3(l*5)*l;
        pos+=noise3(pos*7+time.x/14)/3*l;
        

        //pos.y+=l2*3;
        //pos=rotX(pos,l2*2);
        //pos.z/=1+l2;
        //pos.z-=l;
        //gravity
        pos.y-=pow(length(l),2.2)*.7;
        pos.z+=sin(-pos.y)*l/2;
        //pos.xz*=1+pow((-pos.y+1),.4);

        pos*=12;
        pos.x-=2;
        pos.z+=4;
        pos.y+=121;
    }



    if (mode==2)    
    {
        ind4 = meshInd[vid/3];
        uint ind2[] = {ind4.x,ind4.y,ind4.z};

        pos = mesh[ind2[vid%3]].xyz;
        p1.color=1.15;
        p1.sz=1.2;

        float3 _p0 = mesh[ind2[0]].xyz;
        float3 _p1 = mesh[ind2[1]].xyz;
        float3 _p2 = mesh[ind2[2]].xyz;
        float3 nrml = CalculateNormal(_p0,_p1,_p2);
        //pos-=nrml*1;
        //pos-=nrml*1;

        //pos.y-=1124;
    }
    
    pos.y-=11;
    pos*=.1;
    pos=rotY(pos,time.y/72.);
    //p1.pos=float4(pos,1);
    //pos.y-=11;
    //pos.z-=1;
    //pos = mesh[iid%2025].xyz*.1;

    if (mode==0)    
    {
            p1.color=pow(s,.125)/4.;

            float sz = 3.2;

            if (hF)
            {
                sz=6;
                p1.color/=5;
              //  p1.color*=(9+6*noise(pos*3))/8;
              p1.color*=pow(length(l),2)*4;
            }

            p1.pos = transform_unisize(pos,grid.zw,sz);
            p1.sz=1.2;
            p1.color/=.2*p1.pos.w;

    }

    if (mode==2)
    {
        float4 posT = mul(float4(pos,1), view[0]);
        posT = mul(posT, proj[0]);
        p1.pos=posT;
    }
    
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
