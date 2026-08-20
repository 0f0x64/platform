#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/utils.shader>

cbuffer params : register(b0)
{
    float hilight;
};

#define PI 3.1415926535897932384626433832795

float3 GetWorldSpaceCameraPos(float4x4 viewMatrix)
{
    float3x3 rot = float3x3(
        viewMatrix._11, viewMatrix._21, viewMatrix._31,
        viewMatrix._12, viewMatrix._22, viewMatrix._32,
        viewMatrix._13, viewMatrix._23, viewMatrix._33
    );
    
    float3 trans = float3(viewMatrix._41, viewMatrix._42, viewMatrix._43);
    return -mul(trans, rot);
}

float3 GetWorldViewDir(float3 worldPos, float3 cameraPos)
{
    return normalize(worldPos - cameraPos);
}

float CalculateFresnelFade(float3 localPos, float3 worldViewDir, float power)
{
    float3 normal = normalize(localPos);
    float fresnel = saturate(abs(dot(normal, worldViewDir)));
    return pow(fresnel, power);
}
float noise3D(float3 x)
{
    float3 p = floor(x);
    float3 f = frac(x);
    f = f * f * (3.0 - 2.0 * f);

    float n = p.x + p.y * 157.0 + 113.0 * p.z;
    return lerp(
        lerp(lerp(hash(n + 0.0), hash(n + 1.0), f.x),
             lerp(hash(n + 157.0), hash(n + 158.0), f.x), f.y),
        lerp(lerp(hash(n + 113.0), hash(n + 114.0), f.x),
             lerp(hash(n + 270.0), hash(n + 271.0), f.x), f.y), f.z);
}

float fbm(float3 p, int count, float freq_step)
{
    float sum = 0.0;
    float amp = 0.5;  
    float freq = 1.0;
    
    for(int i = 0; i < count; i++)
    {
        sum += noise3D(p * freq) * amp;
        freq *= freq_step;
        amp *= 0.5;
    }
    return sum;
}

float getSurfacePattern(float3 dir, float t)
{
    float scale = 80.0; 
    float speed = 0.35;
    float offset = fmod(t * speed, 389.0);
    
    float ts = offset;
    
    float3 p = dir * scale + offset;
    float3 warpOffset = float3(
        fbm(p + float3(ts, 0.0, 0.0), 2, 4.0),
        fbm(p + float3(0.0, ts, 0.0), 2, 4.0),
        fbm(p + float3(0.0, 0.0, ts), 2, 4.0)
    );
    float3 i = p + warpOffset * 2.0 - float3(ts, ts, ts);    
    float mid = fbm(i, 6, 2.0);
    
    scale = 15.0;
    speed = speed * 0.05; 
    ts = t * speed;
    offset = fmod(ts, 389.0);
    p = dir * scale + offset;
    
    warpOffset = float3(
        fbm(p + float3(1.0, 0.0, 0.0), 3, 3.0),
        fbm(p + float3(0.0, 1.0, 0.0), 3, 3.0),
        fbm(p + float3(0.0, 0.0, 1.0), 3, 3.0)
    );
    float3 b = p - warpOffset * 4.0;
    float light = 2.0 * smoothstep(0.4, 1.0, pow(fbm(b, 5, 1.0), 2.0));
    
    scale = 4.0;
    speed = speed * 0.25; 
    ts = t * speed;
    offset = fmod(ts, 389.0);
    p = dir * scale + offset;
    warpOffset = float3(
        fbm(p + float3(ts, 0.0, 0.0), 3, 3.0),
        fbm(p + float3(0.0, ts, 0.0), 3, 3.0),
        fbm(p + float3(0.0, 0.0, ts), 3, 3.0)
    );
    b = p + warpOffset * 4.0 - float3(ts * 0.2, ts * 0.2, ts * 0.2);
    float dark = 4.0 * smoothstep(0.1, 1.0, pow(fbm(b, 8, 2.0), 2.0));
    
    return abs(light + mid * 0.8 - dark * 0.2);
}

float4 PS(VS_OUTPUT_PARTICLE2 input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
float3 cam_pos = GetWorldSpaceCameraPos(view[0]);
    float3 rayDir = normalize(input.wpos.xyz - cam_pos); // Вектор от камеры вглубь шара
    
    // Настраиваем радиусы
    float3 O = input.wpos.xyz;
    float r_outer = length(O);
    float r_inner = r_outer * 0.75;
    
    float b = dot(O, rayDir);
    float c = (r_outer * r_outer) - (r_inner * r_inner);
    float discriminant = b * b - c;
    float t = -b - sqrt(max(discriminant, 0.0));

    float3 innerPos = O + rayDir * t;
    float3 n = normalize(innerPos);

    float noiseVal = pow(abs(getSurfacePattern(n, time.x)), 4.0);
    
    float3 colorDark = float3(0.15, 0.0, 0.0);
    float3 colorMid = float3(0.9, 0.3, 0.0);
    float3 colorHot = float3(1.0, 0.9, 0.3);
    
    float3 viewDir = -rayDir;
    
    float fresnel = CalculateFresnelFade(input.wpos.xyz, viewDir, 2.0);
    
    float mask_sphere = smoothstep(0.2, 0.24, fresnel - 0.3);    
    
    float edge_sphere = saturate(pow(1.0 - fresnel, 1.0));
    float mask_edge_sphere = 1.0 - step(0.5, edge_sphere);
    edge_sphere *= (edge_sphere + noiseVal) * mask_edge_sphere * 1.0;
    
    float3 finalColor = 4.0 * lerp(colorDark, colorHot, noiseVal * 2.0) * mask_sphere;
    finalColor += float3(0.3, 0.1, 0.0) * 16.0 * pow(fresnel, 2.0) * (1.0 - step(0.25, pow(fresnel, 2.0)));
    finalColor += float3(1.0, 0.5, 0.0) * edge_sphere * 4.0;

    return float4(11.0 * finalColor, 1.0);

} 