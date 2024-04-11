#include<../lib/constBuf.shader>
#include<../lib/io.shader>
Texture2D tex1:register(t0),tex2:register(t1);SamplerState sam1:register(s0);cbuffer params:register(b1)
{
float mix;
}
float4 PS(VS_OUTPUT t):SV_Target{float2 f=t.uv;f.y-=2;float4 s=tex1.Sample(sam1,f),m=tex2.Sample(sam1,f);return lerp(s,m,mix);}