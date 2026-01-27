#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/utils.shader>
Texture2D screen:register(t0),screenMid:register(t1),screenLow:register(t2);SamplerState sam1:register(s0);cbuffer params:register(b0){float hilight;};float4 PS(VS_OUTPUT s,bool d:SV_IsFrontFace):SV_Target{float4 f=screen.SampleLevel(sam1,s.uv,0),b=screenLow.SampleLevel(sam1,s.uv,0),t=screenMid.SampleLevel(sam1,s.uv,0);f+=b+t;f.xyz=ACESFilm(max(f.xyz,0)*.2);return float4(f.xyz,1);}