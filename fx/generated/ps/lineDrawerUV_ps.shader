#include<../../lib/io.shader>
#include<../../lib/constBuf.shader>
cbuffer params:register(b0)
{
float4 color;
};
float4 PS(VS_OUTPUT s):SV_Target{float P=sign(sin(s.uv.x*50-time.x)+.75);return float4(saturate(abs(sign(s.wpos.xyz))*P),color.w);}