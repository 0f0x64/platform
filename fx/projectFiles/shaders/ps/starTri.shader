#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/utils.shader>

cbuffer params : register(b0)
{
    float4 PosRad;
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

float CalculateFresnelFade(float3 normal, float3 worldViewDir, float power)
{
    float fresnel = saturate(abs(dot(normal, worldViewDir)));
    return pow(fresnel, power);
}
float3 rand3D(float3 p) 
{
    float3 s = float3(
        dot(p, float3(127.1, 311.7, 74.7)), 
        dot(p, float3(269.5, 183.3, 246.1)),
        dot(p, float3(113.5, 271.9, 124.3))
    );
    return frac(sin(s) * 43758.5453);
}

float cells3D(float3 localPos)
{   
    // Масштабируем трехмерное пространство
    localPos *= 10.0;
    
    float3 iuv = floor(localPos);
    float3 guv = frac(localPos);

    float mDist = 1.0;
        
    // Цикл по соседним ячейкам в 3D (3x3x3 = 27 итераций)
    for (float z = -1.0; z <= 1.0; z++) 
    {
        for (float y = -1.0; y <= 1.0; y++) 
        {
            for (float x = -1.0; x <= 1.0; x++) 
            {            
                float3 neighbor = float3(x, y, z);            
                float3 pointPos = rand3D(iuv + neighbor);			
                
                // Анимация движения точек внутри своих мини-кубов
                pointPos = 0.5 + 0.5 * sin(time.x / 42.0 + 6.2831 * pointPos);		
                
                // Вектор разности в 3D
                float3 diff = neighbor + pointPos - guv;            
                
                // Расчет трехмерной дистанции с вашей кастомной вариацией масштаба ячеек
                float dist = length(diff) * (1.0 + rand3D(iuv + neighbor + 100.0).x * 0.25);
                
                mDist = min(mDist, dist);
                
            }
        }
    }
    
    // Эффект свечения / метасфер
    //mDist = 0.1 / (1.0 - clamp(mDist, 0.0, 1.0));       
    //mDist = 0.1 / (mDist-1);       
    //mDist = 0.1 / (1.0 - mDist);       
    return pow(mDist,1.05)*5.;
    
    // Смешивание интенсивности
    float color = lerp(0.0, 1.0, mDist) * 1.5;        

    return color;    
}

float cells3D2(float3 p)
{
return .1/(noise(sin(p+time.x/112)*22)/2+.5);
}

float4 PS(VS_OUTPUT_PARTICLE2 input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    float3 nrml = normalize(input.wpos.xyz);

    float3 cameraPos   = GetWorldSpaceCameraPos(view[0]);
    float3 wpos = input.wpos.xyz;
    wpos*=1+.001/(noise3(normalize(wpos)*26-time.x/22.)/4-.134);
    wpos+=PosRad.xyz;
    float3 worldViewDir = GetWorldViewDir(wpos, cameraPos);
    float edgeFade = CalculateFresnelFade(nrml, worldViewDir, 3.);
    float t = time.x * 0.001;

    float3 dirToPixel = nrml;
    float3 viewAxis = normalize(PosRad.xyz-cameraPos);


    
    float diskZone = saturate(edgeFade * 2.5 - .65); 
    float coronaZone = saturate(1.0 - edgeFade*2); 

float3 toCamera = normalize(PosRad.xyz - cameraPos);

// 1. Твой оригинальный 3D вектор силуэта
float3 radialVector = nrml - toCamera * dot(nrml, toCamera);
//radialVector*=1+8*rot3(radialVector,noise3(normalize(input.wpos.xyz)*7-time.x/22)/14);
float2 stableAngle = normalize(radialVector.xy);

float3 uvNoise1 = float3(stableAngle.xy * .860, t * 1.10);
float3 uvNoise2 = float3(stableAngle.yx * 1.950, t * 2.30);

float noiseValue1 = .321 / cells3D(uvNoise1);
float noiseValue2 = .321 / cells3D(uvNoise2);

float rawRayNoise = saturate((noiseValue1 * noiseValue2) * 1.5);

float rayLengthMask = pow(edgeFade, 0.035); 

float coronaTex = rawRayNoise * rayLengthMask * coronaZone;
coronaTex*=1+noise3(radialVector*2);
//coronaTex=coronaZone;
// Твоя высокочастотная модуляция "игл" (тоже на стабильном векторе)
//coronaTex *= .5 + .4 * sin(stableAngle.x * 6.0) * cos(stableAngle.y * 24.0); 

float3 p_disk = nrml;
float cellTex = saturate(cells3D(noise3(p_disk*7) * .4) * 0.08) * diskZone * 3;
    //cellTex=pow(edgeFade*4,7)/12;
    //float3 uvwNoise1 = float3(radialU * 18.0, radialV * 1.5, t * 1.10);
    //float3 uvwNoise2 = float3(radialU * 12.0, radialV * 1.0, t * 2.3);

//float radialNoise1 = .321 / cells3D(uvwNoise1);
//float radialNoise2 = .321 / cells3D(uvwNoise2);

//float coronaTex = saturate((radialNoise1 + radialNoise2) * 0.25) * coronaZone;
//coronaTex *= .5 + .3 * sin(radialU * 6.0);   
//coronaTex *= .5 + .3 * cos(radialTangent.x * 5.0);
//    float coronaTex = saturate((radialNoise1 + radialNoise2) * 0.25) * coronaZone;
//    coronaTex*=.5+.3*sin(radialU*16);
    coronaTex *= edgeFade * 4.5;
    coronaTex *= 1-saturate(diskZone);
    
    float finalPlasma = (cellTex * 142) + (coronaTex * 4.0);
    float3 centerColor = finalPlasma*114;
    float3 edgeGlow    =  coronaZone * finalPlasma * 113.5;
    edgeGlow=max(edgeGlow,0)*saturate(1-diskZone*8);
    float3 finalColor = (centerColor + edgeGlow) * 222.0 ;
    float coronaMask = saturate(pow(edgeFade,2));
    coronaMask=pow(.5+.5*cos(edgeFade*PI-.766),81)*10;
    coronaMask*=1-saturate(diskZone);
    finalColor+=pow(coronaMask,.8)*22222;

    float alphaMask = diskZone + coronaTex * 13.0;
    finalColor*=alphaMask;
     finalColor *= input.color.rgb*input.color.a;

    return float4(finalColor / 22222.0, saturate(alphaMask)); 
} 