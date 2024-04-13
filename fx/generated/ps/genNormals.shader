#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
Texture2D geo:register(t0);SamplerState sam1:register(s0);float4 PS(VS_OUTPUT o):SV_Target{uint f,l;geo.GetDimensions(f,l);float g=1;l*=g;f*=g;float2 s=o.uv;s.y=1-s.y;float4 P=geo.Sample(sam1,s+float2(-1./f,0)),y=geo.Sample(sam1,s+float2(1./f,0)),B=geo.Sample(sam1,s+float2(0,-1./l)),D=geo.Sample(sam1,s+float2(0,1./l));float3 n=normalize(cross(P.xyz-y.xyz,B.xyz-D.xyz)),V=n;return float4(V,1);}