#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>


cbuffer params : register(b0)
{
    float sx,sy,sz;
};


float3 rotX(float3 pos,float a)
{
    float3x3 m =
    {
        1, 0,       0,
        0, cos(a), -sin(a),
        0, sin(a), cos(a)
    };
    pos = mul(pos, m);
    return pos;
}

float3 rotY(float3 pos, float a)
{
    float3x3 m =
    {
        cos(a), 0, sin(a),
        0, 1, 0,
        -sin(a), 0, cos(a)
    };
    pos = mul(pos, m);
    return pos;
}

float3 rotZ(float3 pos, float a)
{
    float3x3 m =
    {
        cos(a), -sin(a),0,
        sin(a), cos(a), 0,
        0, 0, 1
    };
    pos = mul(pos, m);
    return pos;
}

float calcWalls(float2 uv)
{
    float walls = 0;
    float2 uv5 = float2(0, 0);
    for (int i = 0; i < 5; i++)
    {
  
        float2 xy = uv - .5 - .5 * float2(sin(i * 4. + time.x * .01), .5*sin(i * 5. + time.x * .012));
        float2 uv4 = float2(atan2(xy.y, xy.x) * 2, 2 / (length(xy) - .1) + time.y * .1);
        //walls += pow(saturate(1 - 2 * length(xy)), 2) * 12;
        uv5 += uv4*2;
        
    }
  
//  walls = pow((1 - 2 * (abs(sin(uv5.x)) * abs(sin(uv5.y / 13)))), 9) * 2;
    walls = pow((1 - 1 *(abs(sin(uv5.x)) * abs(sin(uv5.y / 13)))), 9) * 2;

    return walls;
}

float3 getTpos(float2 a, float R,float p,float q)
{    
        return float3((R + cos(a.x * q)) * cos(a.x * p), 
                      (R + cos(a.x * q)) * sin(a.x * p),
                      sin(a.x * q));
}

float3 torusKnot(float2 a)
{
    
    float R = 1.5;
    float r = .24;

    float p=1;
    float q=3;

    float3 pos = getTpos(a,R,p,q);
    float3 T = pos - getTpos(a+float2(.01,0),R,p,q);
    float3 N = pos+T;
                    
    float3 B = cross(T,N);
    N = cross(B,T);
    
    B= normalize(B);
    N= normalize(N);

    pos += r * (cos(a.y) * N + sin(a.y) * B);

    return pos;
}

static float lcoef=0;
static float3 sym=float3(0,0,0);

void select(float3 pos,float3 modPos,float rad, float p)
{
    float3 sPos = lerp(pos,abs(pos),sym);
    float3 sModPos = lerp(modPos,abs(modPos),sym);
    float coef = rad/distance(sPos,sModPos);
    coef=smoothstep(0,1,coef);
    lcoef=pow(coef,p);
}

float3 move(float3 pos,float3 ofs)
{
    float3 newpos = lerp(pos,pos+ofs,lcoef);
    return newpos;
}

float3 zoom(float3 pos,float3 modPos, float3 ofs)
{
    float3 newpos = lerp(pos,(pos-modPos)*ofs+modPos,lcoef);
    return newpos;
}

float3 sphere(float2 uv)
{
    //uv=frac(uv);
    //uv.y=sin(uv.y*PI);
    //uv.x=sin(uv.x*PI*2);
    float2 a = (uv-.5) * PI * 2;
    a.x *= -1;
    float3 pos = float3(sin(a.x), sin(a.y / 2), cos(a.x));
    pos.xz *= cos(a.y/2 );
    pos = pos.zxy;
    
    pos=rotZ(pos,pos.x);

    pos.z*=2;
    pos.x*=.35;
    pos.y*=.35;
     pos.y*=1-pos.z*.1;

    sym=float3(0,0,0);
    select(pos,float3(0,0,2.3),.4,3);
    pos = zoom(pos,float3(0,0,2.2),float3(3,3,1.3));
   
    //head and neck
    select(pos,float3(0,0,2.3),.7,1.2);
    pos = rotX(pos,lcoef*.2);
    pos = move(pos,float3(0,lcoef*.3,-lcoef*.1));

    //tail
   select(pos,float3(.0,0.,-1.6),.9,25);
    pos = zoom(pos,float3(.0,0.,-1.6),float3(.2,.2,1));
    select(pos,float3(.0,0.,-1.6),.9,11);
    pos=rotX(pos,-1.8*sin(pos.z*1+3.5)*lcoef);
     pos=rotY(pos,-.8*sin(pos.z*1+3.5)*lcoef);
     //tail ainm
    select(pos,float3(.0,1.8,-1.2),.6,11);
    //pos=rotY(pos,-1.8*sin(time.x*.1)*lcoef);
     //pos=rotY(pos,-.8*sin(pos.z*1+3.5)*lcoef);
     //pos = move(pos,float3(0,0,-4));


    sym=float3(1,0,0);
   select(pos,float3(.15,-.1,1.),.13,8);
    pos = move(pos,float3(0,-.7,0));
    lcoef=pow(lcoef,.2);
  //  pos = zoom(pos,float3(0,0,0.),float3(1.5,1,1));

    sym=float3(1,0,0);
     select(pos,float3(.35,-.1,-.4),.15,5);
    pos = move(pos,float3(0,-.7,0));
    lcoef=pow(lcoef,.2);
//    pos = zoom(pos,float3(0,0,0.),float3(1.5,1,1));

    pos.xz*=.7;

    return pos;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.uv;
    //uv.x-=time.x*.001;
    
     float2 a = (uv-.5) * PI * 2;
    a.x *= -1;

    float3 pos = sphere(uv)*2;
    float3 posX = sphere(uv+float2(.05,0))*2;
    float3 posY = sphere(uv+float2(0,0.05))*2;
    float3 t = normalize(posX-pos);
    float3 b = normalize(posY-pos);
    float3 n= -cross(t,b);

    float a1= sin(a.x*4096/4)+sin(a.y*4096/4)*.26;
    float3 ofs= n*a1;
    
    ofs+= (-b)*a1*(.5+.2*sin(dot(b,t)*444));
    
    float m =saturate(1-abs(uv.y-.5)*2.1);
    m=pow(m,.25);
    pos+=ofs*m*.5;
    pos.y-=a1*.4*saturate(-pos.y);
    
    pos+=a1*float3(sin(pos.x*1214),sin(pos.y*2112),sin(pos.y*1122))*.05*m;
//    pos = rotY(pos, time.x * 0.01);

    //
    //return 1;
    return float4(pos, 1.);

}
