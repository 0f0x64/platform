#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
Texture2D geo:register(t0);SamplerState sam1:register(s0);float4 PS(VS_OUTPUT o):SV_Target{uint f,l;geo.GetDimensions(f,l);float g=5;l*=g;f*=g;float2 P=o.uv;float4 s=geo.Sample(sam1,P+float2(-1./f,0)),n=geo.Sample(sam1,P+float2(1./f,0)),B=geo.Sample(sam1,P+float2(0,-1./l)),V=geo.Sample(sam1,P+float2(0,1./l));float3 v=normalize(cross(normalize(n.xyz-s.xyz),normalize(V.xyz-B.xyz))),c=v;return float4(c,1);}