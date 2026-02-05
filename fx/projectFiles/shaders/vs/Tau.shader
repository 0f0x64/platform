#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>
#include <../lib/utils.shader>

#include <../lib/girl_data.shader>

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


pos_color CalcParticles(uint qid,uint iid,float4 grid)
{

float4 _ColorHot = float4(1.0, 0.9, 0.7,1);
float4 _ColorCold = float4( 0.1, 0.0, 0.4,1);

float form=smoothstep(0,1,smoothstep(0,1,.5+.5*sin(time.x/32)));
form =smoothstep(0,1,saturate((time.y/3000)));
float3 pos;
    pos_color p1;

    uint inStars = 9623;
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
    
    float delta = length(p - p_prev)*1;
    float heat = exp(-delta * 10); 

    p*=12;
    p=p.zxy*lerp(14,8,form);

    float3 jitter = (hash31((float)iid) - 0.5) * 0.02;
    p += jitter/(pow(length(p),2)+.1)*172;

    float py = frac(iid/292100.-time.x/1000+hash(iid)/10);
    float3 g=girl_vertex[iid%3990]/4.8;
    float3 g_1=girl_vertex[iid%3990+(iid/3990)%3]/4.8;
    g=lerp(g,g_1,.03);

    g.y+=noise(time.x/50)*12;
    g+=hash33(g+hash(iid))*.15;
    float margin = iid%2>(noise(g+time.x/10+222)*3+1);
    g.xyz+=margin*noise3(iid/292100.+g/3)*12*(py);
    g.xyz+=margin*noise3(g/3)*2;
    g.y+=margin*pow(frac(iid/292100.+time.x/20),1)*7;

    float3 nn=g/4;
    for (int i = 0; i < 8; i++) {
        p_prev = nn;
        float3 next = noise3(nn*8+22+a/b+(time.x/32)*0);
        
        nn -=next/(i+1)*(1-form);
        //nn=rot3(nn,nn*3*(1-form));
        nn=lerp(nn,rot3(nn,nn*1),(1-form));
        nn+=(rot3(nn-nn/2,nn*.1)+nn/2)/51*form;
        nn += next/(i+1)*form;


    }

    g=lerp(g,nn*33,sm2(sm2(form)) );
    //g*=2;
    float3 g2=normalize(hash3(iid))*84*hash(iid)+noise3(g/2)*25;

    //p=lerp(g2,p,saturate(length(p)/112));
    float girl = saturate(pow(length(p)/28,.75)-1.9);
    p=lerp(g2,p,saturate(pow(length(p)/18,.75)-2));
    p=lerp(g,p,saturate(pow(length(p)/13,.75)-2));

    //p=lerp(g,p,0);

    float3 nebulaColor = lerp(_ColorCold.rgb, _ColorHot.rgb, pow(heat, 3.0));
    p1.color = float4(nebulaColor, 1);

    p1.color.rgb=lerp(py/1000,p1.color.rgb,saturate(length(p)/30));
    p1.color.rgb=(1-py*2)/1.;
    p1.color.rgb=lerp(.2*nebulaColor,.02*float3(1,2,3),1-girl);
        //p1.color = float4(nebulaColor, 1);
    pos=p;
    

     qid *= skipper;
     t=time.x*.004;

     if (mode==1||iid%inStars==0)
     {
        t=0;
     }

    if (mode==0)    
    {
         if (iid%inStars==0)
         {
              p1.pos = transform(pos,grid.zw,75.5);
              p1.sz=1.2;
              p1.color*=2;
              p1.color*=lerp(0.03,1,form);
         }
         else{
            p1.pos = transform_unisize(pos,grid.zw,2.);
            p1.sz=1.2;
         }

    }
    else
    {
        p1.pos = transform(pos,grid.zw,121.2);
        p1.color*=.15*(1-saturate(pow(length(p)/21,.75)));
        p1.sz=1.2;
       // p1.color=lerp(p1.color,1,form);

    }

    //density compensation
    //p1.color/=min(pow(p1.pos.w,1.1)*.21+.5,11);
    return p1;
}

#include <../lib/particleVS_main2.shader>
