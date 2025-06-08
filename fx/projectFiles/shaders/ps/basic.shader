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
    //return 1;
    float3 c = saturate(1.-2.*length(input.uv-.5))*.3;
    //c.x+=noise(input.id.x*.001)*.5;
    //c.y+=noise(input.id.x*.0003)*.6;
    c.z*=noise(input.id.x*.2)*3.1;
    c*=float3(.05,.05,.5)*.61+.25;
return float4(c,1);

    float2 uv = input.uv;
    float3 albedo = float3(1, 1, 1);
    albedo = sin(uv.x*158)*(sin(uv.y*2));
    albedo=saturate(albedo)*float3(1,.5,.2);


    //return float4(albedo,1);
    float roughness = .71;
    float metalness = 0.;
    float3 F0 = .04;
    
    float3 nrml = normals.SampleLevel(sam1, input.uv,6).xyz;
    nrml += normals.SampleLevel(sam1, input.uv,0).xyz*.01;
    nrml=normalize(nrml);
    //nrml=rotY(nrml,time.x*.1);
    //nrml *= -(1 - isFrontFace * 2);
    float3 eye = input.vpos.xyz;
    eye = normalize(mul((view[0]), float4(eye, 1)));
    eye = normalize(eye);
    float3 ref = reflect(eye,nrml);
    float3 specR = env.SampleLevel(sam1, ref, roughness * 10);
    float3 diffR = env.SampleLevel(sam1, -nrml, 9.5)/3;
    
    float3 albedo_ = lerp(albedo, 0, metalness);
    F0 = lerp(F0, F0 * albedo, metalness);
    float3 F = FresnelSchlick(F0, eye, -nrml);
    F *= lerp(saturate(1 - roughness), 1, metalness);
    float3 specK = specR * F;
    float3 diffK = saturate(1.0 - F);
    diffK = albedo * diffK * diffR;

    float3 color = diffK + specK;
    float3 screenN = mul(nrml, view[0]).xyz;

   // color = 0; 
    
    //color *= saturate(1-2 * pow(dot(normalize(mul(eye, view[0])), normalize(mul(nrml, view[0]))), .5));// * saturate(sin(eye.x * 513 + eye.y * 521 + eye.z * 432 + time.x) * 4);
    //float3 nt = mul(input.vnorm.xyz, view[0]);
    float3 nt = mul(nrml, view[0]);
    float d = dot(normalize(input.vpos.xyz), normalize(nt));
    color += saturate(10 - 35 * d)* sin(d * 127 + time.x) * 4 * hilight;
    
    color.rgb = pow(ACESFilm(color.rgb),1/2.2);
    //return 1;
    return float4(color, 1);
}
