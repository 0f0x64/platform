#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
Texture2D geo:register(t0);SamplerState sam1:register(s0);float4 PS(VS_OUTPUT o):SV_Target{uint f,l;geo.GetDimensions(f,l);float g=1;l*=g;f*=g;float2 P=o.uv;float4 s=geo.Sample(sam1,P+float2(-1./f,0)),B=geo.Sample(sam1,P+float2(1./f,0)),D=geo.Sample(sam1,P+float2(0,-1./l)),G=geo.Sample(sam1,P+float2(0,1./l));float3 V=normalize(cross(s.xyz-B.xyz,D.xyz-G.xyz));return float4(V,1);}