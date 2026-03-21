#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/utils.shader>
cbuffer params:register(b0){float hilight;};
#define PI 3.1415926535897932384626433832795
float4 PS(VS_OUTPUT_PARTICLE P,bool f:SV_IsFrontFace):SV_Target{float3 V=saturate(1.-2.*length(P.uv-.5));return float4(V*P.color.xyz,1);}