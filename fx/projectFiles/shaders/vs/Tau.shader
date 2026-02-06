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

float3 getRandomPointInTriangle(float3 g, float3 g1, float3 g2, float r)
{
    float2 randoms;
    randoms.x = hash(r); 
    randoms.y = hash(randoms.x); 
    float sqrtR1 = sqrt(randoms.x);
    float u = 1.0f - sqrtR1;
    float v = randoms.y * sqrtR1;
    return u * g + v * g1 + (1.0f - u - v) * g2;
}

float fresnel(float3 normal, float3 viewDir, float power)
{
    // 1. Вычисляем косинус угла (0 на краях, 1 в центре)
    float dotProduct = dot(normal, viewDir);
    
    // 2. Инвертируем и ограничиваем (1 на краях, 0 в центре)
    float fresnel =  saturate(dotProduct);
    
    // 3. Возводим в степень для настройки резкости перехода
    return pow(fresnel, power);
}

pos_color CalcParticles(uint qid,uint iid,float4 grid)
{

float4 _ColorHot = float4(1.0, 0.9, 0.7,1);
float4 _ColorCold = float4( 0.1, 0.0, 0.4,1);

float form=smoothstep(0,1,smoothstep(0,1,.5+.5*sin(time.x/32)));
form =smoothstep(1,0,saturate((time.y/500)));
float3 pos;
    pos_color p1;

    uint inStars = 9223;
    float t = time.x * 1.;
 
    // if (iid%inStars==0)
         {
             t=0;
         }
    float4 a = float4(1.4 + sin(t*0.1)*0.1, -2.3, 2.4, -2.1 + cos(t*0.15)*0.1);
    float4 b = float4(0.8, -1.2 + sin(t*0.2)*0.05, 1.3, -1.5);

    float3 p = hash31((float)iid * 0.123) -.5;
    p=normalize(p)/1.5;
    float3 p_prev = p;
    
    for (int i = 0; i < 18; i++) {
        p_prev = p;
        float3 next = noise3(p*8+22+a/b+(time.x/32)*0);
        
        p -=next/(i+1)*(1-form);
        //p=rot3(p,p*3*(1-form));
        p=lerp(p,rot3(p,p*3),(1-form));
        p+=(rot3(p-p/2,p*3)+p/2)/11*form;
        p -= next/(i+1)*form;


    }
    
    p+=noise3(p*11+(time.x/32))/14*saturate(length(p)/1);
    
    float delta = length(p - p_prev)*6;
    float heat = exp(-delta * 10); 

    p*=12;
    p=p.zxy*lerp(14,8,form);

    float3 jitter = (hash31((float)iid) - 0.5) * 0.02;
    p += jitter/(pow(length(p),2)+.1)*172;

    float py = frac(iid/292100.-time.x/1000+hash(iid)/10);

    float3 g0=mesh[iid%3990]/4.8;
    float3 g=mesh[iid%3990]/4.8;
    float3 g_1=mesh[(iid+hash(iid)*12)%3990]/4.8;
    float3 g_2=mesh[(iid+hash(iid)*122+123)%3990]/4.8;
    float3 g_1_2=g_1;
    float3 g_2_2=g_2;
    //if (distance(g,g_1)>2) g_1=g+normalize(g-g_1)*125;
    //if (distance(g,g_2)>2) g_2=g+normalize(g-g_2)*125;
    float sd=.085;
    g_1=lerp(g_1,g,saturate(distance(g,g_1)*sd));
    g_2=lerp(g_2,g,saturate(distance(g,g_2)*sd/2.));

    float3 nrml = cross((g_1-g+.1),(g_2-g+.1));
    nrml=normalize(nrml);

    

    float3 f0=mesh[iid%3990]/4.8;
    float3 f1=mesh[(iid+126)%3990]/4.8;
    
    float3 gL=lerp(f0,f1,hash(iid/22.)*saturate(6.8/distance(f0,f1)));
    float3 gR=getRandomPointInTriangle(g,g_1,g_2,hash(iid));
    g=lerp(gL,gR,pow(hash(iid),.5));

    //g=lerp(g0,0,hash(iid/123.));

    //g.y+=hash(iid/1213.)/2;
    //g_1=g+normalize(g-g_1)*3*iid/(2000. * 1000.);
    //g+=noise3(iid/(2000. * 1000.)*22);
    //g=lerp(g,g_1,iid/(2000. * 1000.));
    

    //g.y+=noise(time.x/50)*22;
    //g+=hash33(g+hash(iid))*.15;
    float margin = iid%2>(noise(g+time.x/10+222)*3+1);
    //margin*=iid%inStars==0 ? 0:1;
   // g.xyz+=margin*noise3(iid/292100.+g/3)*12*(py);
    //g.xyz+=margin*noise3(g/3)*2;
    //g.y+=margin*pow(frac(iid/292100.+time.x/20),1)*7;

    float3 nn=g/4;
    for (int i = 0; i < 8; i++) {
        p_prev = nn;
        float3 next = noise3(nn*.8+22+a/b+(time.x/32)*0);
        
        nn -=next/(i+1)*(1-form);
        //nn=rot3(nn,nn*3*(1-form));
        nn=lerp(nn,rot3(nn,nn*.3),(1-form));
        nn+=(rot3(nn-nn/2,nn*1)+nn/2)/1.*form;
        nn += next/(i+1)*form;


    }

    //g=lerp(g,nn.yxz*9,margin);

    //g=lerp(g,nn*9,sm2(sm2(form)) );
    //g*=2;
    float3 g2=normalize(hash3(iid))*84*hash(iid)+noise3(g/2)*25;

    p=lerp(g2,p,saturate(length(p)/2));
    float girl = saturate(pow(length(p)/28,.75)-1.9);
    p=lerp(g2,p,saturate(pow(length(p)/18,.75)-2));
    p=lerp(g,p,saturate(pow(length(p)/13,.75)-2));

    p=g;
    heat*=(.8+girl);

    float3 nebulaColor = lerp(_ColorCold.rgb, _ColorHot.rgb, pow(heat, 4.0));
    p1.color = float4(nebulaColor*.2, 1);

    //p1.color.rgb=lerp(py/1000,p1.color.rgb,saturate(length(p)/30));
    //p1.color.rgb=(1-py*2)/1.;
    //p1.color.rgb=nebulaColor*=.2;
    //p1.color.rgb=lerp(.2*nebulaColor,.02*float3(1,2,3),1-girl);
        //p1.color = float4(nebulaColor, 1);
    pos=p;
    

     qid *= skipper;
     t=time.x*.004;

     if (mode==1||iid%inStars==0)
     {
        t=0;
     }

         float4 vv= mul(float4(0,0,1,1),view[0]);
         vv.xyz=normalize(vv.xyz);
    //nrml4=mul(nrml4,proj[0]);
    //nrml=transform_unisize(nrml,grid.zw,2.)
    //nrml=normalize(nrml.rgb);
    

    if (mode==0)    
    {
            p1.pos = transform_unisize(pos,grid.zw,2.);
            p1.sz=11.2;

          //  p1.color.rgb=(fresnel(nrml,vv,1))*.02;
    }
    else
    {
        p1.pos = transform(pos,grid.zw,121.2);
        p1.color*=.035;
        //p1.color*=.15*(1-saturate(pow(length(p)/21,.75)));
        p1.sz=1.2;
       // p1.color=lerp(p1.color,1,form);
       p1.color=0;

    }
    
    //density compensation
    //p1.color/=min(pow(p1.pos.w,1.1)*.21+.5,11);
    return p1;
}

#include <../lib/particleVS_main2.shader>
