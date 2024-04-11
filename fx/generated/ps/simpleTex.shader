#include<../lib/constBuf.shader>
#include<../lib/io.shader>
Texture2D albedo:register(t0),metalness:register(t1);SamplerState sam1:register(s0);cbuffer params:register(b1)
{
float tone;
float p2;
}
float4 PS(VS_OUTPUT s):SV_Target{float4 p=float4(s.uv,sin(s.uv.x*12),1);float2 f=s.uv*5;f.y-=2;p=albedo.Sample(sam1,f);return p;}