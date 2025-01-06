#include<../lib/constBuf.shader>
#include<../lib/io.shader>
TextureCube env:register(t0);Texture2D tex2:register(t1);SamplerState sam1:register(s0);cbuffer params:register(b1)
{
float mix;
}
float4 PS(VS_OUTPUT s):SV_Target{float2 e=s.uv;return env.SampleLevel(sam1,float4(normalize(s.wpos.xyz),0),0);}