#include<../lib/constBuf.shader>
#include<../lib/io.shader>
TextureCube env:register(t0);SamplerState sam1:register(s0);float4 PS(VS_OUTPUT s):SV_Target{return env.SampleLevel(sam1,float4(normalize(s.wpos.xyz),0),0);}