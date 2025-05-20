#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/utils.shader>
TextureCube env:register(t0);SamplerState sam1:register(s0);float4 PS(VS_OUTPUT s):SV_Target{float4 f=env.SampleLevel(sam1,float4(normalize(s.wpos.xyz),0),0);f.xyz=pow(ACESFilm(f.xyz),1/2.2);return f;}