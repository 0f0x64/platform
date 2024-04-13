#include<../lib/constBuf.shader>
#include<../lib/io.shader>
TextureCube env:register(t0);Texture2D normals:register(t1);TextureCube albedo:register(t2);SamplerState sam1:register(s0);float4 PS(VS_OUTPUT n):SV_Target{float3 e=normals.SampleLevel(sam1,n.uv,1).xyz,f=n.vpos.xyz;f=mul(view[0],float4(f,1));f=normalize(f);float3 P=reflect(f,e);return env.SampleLevel(sam1,P,0);}