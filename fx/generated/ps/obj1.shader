#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
cbuffer params:register(b1)
{
float sx,sy,sz;
};
float3 sphere(float2 s){float2 f=s*PI*2;f.x*=-1;float3 y=float3(sin(f.x),sin(f.y/2),cos(f.x));y.xz*=cos(f.y/2);y*=.45;float b=64;y/=1-pow(abs(sin(s.x*b)+cos(s.y*b/2)),8)*.00021;b=64+11112*sin(s.x*12)*cos(s.y*22);y/=1-pow(abs(sin(s.x*b)+cos(s.y*b/2)),8)*2.1e-5;return y;}float4 PS(VS_OUTPUT s):SV_Target{float2 f=s.uv-.5;float3 b=sphere(f)*2;return float4(b,1.);}