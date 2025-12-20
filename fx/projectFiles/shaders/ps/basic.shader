#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/utils.shader>

cbuffer params : register(b0)
{
    float hilight;
};

#define PI 3.1415926535897932384626433832795

float4 PS(VS_OUTPUT input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    float3 c = saturate(1.-2.*length(input.uv-.5));

    float2 uv=2*(input.uv-.5);
    float2 suv =abs(uv);
    float d=2./((length(uv.xy-uv.yx)+length(uv.xy+uv.yx))/2);
          d+=2./(suv.x+suv.y);
         // if (isinf(d)) d=1111;
         

    
          //d=min(d,pow(length(input.sz),55)*122);
          //d=pow(d,.7)*10;
          //d*=101;
//              d=min(d,210*length(input.sz));
    d*=saturate(1-max(suv.x,suv.y));

  //  d+=c*3;

          if (length(input.sz1)<=1.1) return float4(15*input.rgba.rgb/9.,1);
    //return 1;
    //if (length(input.sz)<1.1) return float4(0,0,0.,1);
    return float4(d*input.rgba.rgb/9.,1);

}
