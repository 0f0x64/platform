#include<../../lib/constBuf.shader>
#include<../../lib/io.shader>
Texture2D tex:register(t0);SamplerState s1:register(s0);cbuffer params:register(b0)
{
float r,g,b,a;
float bold;
}
float4 PS(VS_OUTPUT_POS_UV a):SV_Target{float4 r=tex.SampleLevel(s1,a.uv,0);r.w=saturate(dot(r.xyz,1));r*=float4(r,g,b,a);return float4(r);}