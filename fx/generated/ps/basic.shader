#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/utils.shader>
TextureCube env:register(t0);Texture2D normals:register(t1);TextureCube albedo:register(t2);SamplerState sam1:register(s0);cbuffer params:register(b0){float hilight;};float3 FresnelSchlick(float3 f,float3 h,float3 s){float x=dot(-s,h);return saturate(f+(1.-f)*pow(1.-saturate(x),5.));}float3 rotY(float3 f,float h){float3x3 x={cos(h),0,sin(h),0,1,0,-sin(h),0,cos(h)};return mul(f,x);}
#define PI 3.1415926535897932384626433832795
float hash(float h){return frac(sin(h)*43758.5453);}float noise(float3 h){float3 f=floor(h);h=frac(h);h=h*h*(3.-2.*h);float x=f.x+f.y*57.+113.*f.z;x=lerp(lerp(lerp(hash(x),hash(x+1.),h.x),lerp(hash(x+57.),hash(x+58.),h.x),h.y),lerp(lerp(hash(x+113.),hash(x+114.),h.x),lerp(hash(x+170.),hash(x+171.),h.x),h.y),h.z);return x+.1;}float4 PS(VS_OUTPUT h,bool f:SV_IsFrontFace):SV_Target{float3 x=saturate(1.-2.*length(h.uv-.5));x*=.13;return float4(x*h.xyzw*2,1);}