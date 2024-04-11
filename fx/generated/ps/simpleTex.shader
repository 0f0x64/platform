#include<../lib/constBuf.shader>
#include<../lib/io.shader>
Texture2D tex1:register(t0);SamplerState sam1:register(s0);cbuffer params:register(b1)
{
float tone;
}
float4 PS(VS_OUTPUT t):SV_Target{float4 P=float4(t.uv,sin(t.uv.x*12),1);float2 f=t.uv*5;f.y-=2;P=tex1.Sample(sam1,f);return P;}