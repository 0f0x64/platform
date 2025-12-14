#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/utils.shader>


TextureCube env : register(t0);
Texture2D normals : register(t1);
TextureCube albedo : register(t2);

SamplerState sam1 : register(s0);


cbuffer params : register(b0)
{
    float hilight;
};


float3 FresnelSchlick(float3 F0, float3 v, float3 n)
{
    float cosTheta = dot(-n, v);
    return saturate(F0 + (1.0 - F0) * pow(1.0 - saturate(cosTheta), 5.0));
}

float3 rotY(float3 pos, float a)
{
    float3x3 m =
    {
        cos(a), 0, sin(a),
        0, 1, 0,
        -sin(a), 0, cos(a)
    };
    pos = mul(pos, m);
    return pos;
}

#define PI 3.1415926535897932384626433832795

float hash( float n ) {
        return frac(sin(n)*43758.5453);
    }
     
    float noise( float3 x ) {
        // The noise function returns a value in the range -1.0f -> 1.0f
        float3 p = floor(x);
        float3 f = frac(x);
     
        f = f*f*(3.0-2.0*f);
        float n = p.x + p.y*57.0 + 113.0*p.z;
     
        float a= lerp(lerp(lerp( hash(n+0.0), hash(n+1.0),f.x),
               lerp( hash(n+57.0), hash(n+58.0),f.x),f.y),
               lerp(lerp( hash(n+113.0), hash(n+114.0),f.x),
               lerp( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
               
               return a+.1;
    }

float4 PS(VS_OUTPUT input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    float3 c = saturate(1.-2.*length(input.uv-.5));
    return float4(c*input.rgba,1);

}
