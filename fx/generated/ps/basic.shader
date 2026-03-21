#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/utils.shader>
cbuffer params:register(b0){float hilight;};
#define PI 3.1415926535897932384626433832795
float4 PS(VS_OUTPUT_PARTICLE l,bool P:SV_IsFrontFace):SV_Target{float3 h=saturate(1.-2.*length(l.uv-.5));float2 e=2*(l.uv-.5),b=abs(e);float f=2./((length(e.xy-e.yx)+length(e.xy+e.yx))/2)+2./(b.x+b.y);f*=saturate(1-max(b.x,b.y));return length(l.size)<=1.1?float4(15*l.color.xyz/9.,1):float4(f*l.color.xyz/9.,1);}