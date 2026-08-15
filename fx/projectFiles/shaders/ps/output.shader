#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/utils.shader>


Texture2D screen : register(t0);
Texture2D screenMid : register(t1);
Texture2D screenLow : register(t2);
SamplerState sam1 : register(s0);
SamplerState sam2 : register(s1);
SamplerState sam3 : register(s2);


cbuffer params : register(b0)
{
    float hilight;
};



float4 PS(VS_OUTPUT input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    float4 c = screen.SampleLevel(sam1,input.uv,0);
    float4 c2 = screenLow.SampleLevel(sam2,input.uv,0);
    float4 c3 = screenMid.SampleLevel(sam3,input.uv,0);
    float af=1.-c.a;
    //c*=(c.a*c2.a*c3.a);
    c+=(c2+c3);

     //c=lerp(c2+c3,c+c2+c3,c.a);
    //c+=c2;
    //if (c.a>c2.a) c.rgb+=c2.rgb;
    //if (c.a>c3.a) c.rgb+=c3.rgb;

    //c=pow(c.a,.2);
    
    //c=lerp(c,c+c2+c3,c3.a);

    
    
    c.rgb=ACESFilm(max(c.rgb,0)*.2);
    return float4(c.rgb,1);

}
