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
                pointPos = 0.5 + 0.5 * sin(time.x / 22.0 + 6.2831 * pointPos);		
                
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
    return mDist*.5;
    
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
    float3 p = normalize(input.wpos.xyz);

    float3 cameraPos   = GetWorldSpaceCameraPos(view[0]);
    float3 worldViewDir = GetWorldViewDir(input.wpos, cameraPos);
    float edgeFade  = CalculateFresnelFade(input.wpos, worldViewDir, 3.);

    p+=rot3(p,noise3(p*26.41))*.12;
    float c=.1/cells3D(p);
    c+=edgeFade;
    c*=1+.1/cells3D((p*2));

    c=ACESFilm(max(c,0)*.13);
 c*=saturate(edgeFade*8);

    float3 c2=c*float3(15,1,.3)*5;

    //if (c.r*edgeFade*12<.03+hash(p*12).r*.01) discard;
        
    return float4(c2,1 ); 

}
