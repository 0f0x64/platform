#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
cbuffer params:register(b1)
{
float sx,sy,sz;
};
float3 sphere(float2 s){float2 f=s*PI*2;float3 r=float3(sin(f.x),sin(f.y/2),cos(f.x));r.xz*=cos(f.y/2);r=clamp(r,-.75,.75);r*=.45;return r;}float4 PS(VS_OUTPUT f):SV_Target{float2 r=f.uv-.5;float3 s=sphere(r)*2;return float4(s,1.);}