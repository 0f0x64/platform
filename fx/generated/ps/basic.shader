#include<../lib/constBuf.shader>
#include<../lib/io.shader>
TextureCube env:register(t0);Texture2D normals:register(t1);TextureCube albedo:register(t2);SamplerState sam1:register(s0);float4 PS(VS_OUTPUT s):SV_Target{float3 e=normals.SampleLevel(sam1,s.uv,1).xyz,t=normalize(s.vpos.xyz);t=mul(t,view[0]);float3 T=reflect(t,e);T=-e.xyz;return env.SampleLevel(sam1,T,0);}