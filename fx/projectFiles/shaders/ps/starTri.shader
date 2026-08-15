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
                pointPos = 0.5 + 0.5 * sin(time.x / 52.0 + 6.2831 * pointPos);		
                
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
    return pow(mDist,1.05)*2.;
    
    // Смешивание интенсивности
    float color = lerp(0.0, 1.0, mDist) * 1.5;        

    return color;    
}

float cells3D2(float3 p)
{
return .1/(noise(sin(p+time.x/112)*22)/2+.5);
}

/*float4 PS(VS_OUTPUT_PARTICLE2 input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    float3 p = normalize(input.wpos.xyz);

    float3 cameraPos   = GetWorldSpaceCameraPos(view[0]);
    float3 worldViewDir = GetWorldViewDir(input.wpos, cameraPos);
    float edgeFade  = CalculateFresnelFade(input.wpos, worldViewDir, 3.);

    p+=rot3(p,noise3(p*26.41))*.12;
    //p*=rot3(p,noise3(.1/p*edgeFade*22))*1.1+1;
    float c=.1/cells3D(p);
   // c+=edgeFade;
    c*=1+.1/cells3D((p*2));

    //c=ACESFilm(max(c,0)*.13);
 c*=saturate(edgeFade*8);


    //float3 c2=c*float3(15,1,.3)*5;
    float3 c2=c*input.color.rgb*3;

    c2=pow(edgeFade,2)*2222*input.color.rgb*(.051/cells3D(p*1.4)*.8);  
    //c2=.0;
            
            //if (dot(input.color,1)<.1) discard;
    if (c.r*edgeFade*1112<.03+hash(p*12).r*.51) discard;
     
    //c2+=pow(1-edgeFade,11)*1;
    
    //return float4(c2,1); 
    return float4(c2,saturate(edgeFade*11)); 

}*/

float4 PS(VS_OUTPUT_PARTICLE2 input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    float3 p = normalize(input.wpos.xyz);

    float3 cameraPos   = GetWorldSpaceCameraPos(view[0]);
    float3 worldViewDir = GetWorldViewDir(input.wpos, cameraPos);
    
    // Мягкий базовый Френель
    float edgeFade = CalculateFresnelFade(input.wpos, worldViewDir, 3.);
    float t = time.x * 0.01;

    // Стабильный базис для радиальных лучей (без багов при смещении)
    float3 dirToPixel = normalize(input.wpos.xyz);

    // 2. Вектор направления от камеры к центру звезды (стабильная ось Z нашего виртуального экрана)
    // Так как центр звезды в (0,0,0), это просто инвертированная позиция камеры
    float3 viewAxis = normalize(-cameraPos);

    // 3. Строим стабильный мировой базис, перпендикулярный лучу зрения, 
    // но жестко привязанный к глобальной оси Y мира (0, 1, 0)
    float3 globalUp = float3(0.0, 1.0, 0.0);
    
    // Вектор "Вправо" всегда горизонтален миру, как бы вы ни крутили камеру по оси взгляда
    float3 worldRight = normalize(cross(viewAxis, globalUp));
    // Вектор "Вверх" достраивает честную плоскую систему координат
    float3 worldUp    = cross(worldRight, viewAxis);

    // 4. Проецируем 3D-направление пикселя на наш стабильный мировой экран.
    // Это полностью убирает зависимость от поворота камеры (roll) вокруг своей оси!
    float radialX = dot(dirToPixel, worldRight);
    float radialY = dot(dirToPixel, worldUp);
    float radialZ = dot(dirToPixel, viewAxis);

    float3 ppp=noise(p*4+time.x/1100)*.1;
     radialX +=ppp.x;
     radialY +=ppp.y;
     radialZ +=ppp.z;

    // Считаем полярный угол. Теперь лучи намертво прибиты к пространству сцены.
    float phi = abs(atan2(radialY, radialX));
    float phi2 = abs(atan2(radialY, radialZ));
    // Мягкая радиальная развертка: Y идет плавно от центра наружу
//    float2 radialUV = float2(phi / 6.2831, 1.0 - edgeFade);
    float2 radialUV = float2(phi / 6.2831, phi2 / 6.2831);
    //radialUV /=1+noise3(radialUV.xyy*5).xy;
    
    

    // =================================================================
    // ЗОНИРОВАНИЕ (Теперь мягкое, без резких ступеней)
    // =================================================================
    // Базовый диск плавно зануляется к краям
    float diskZone = saturate(edgeFade * 3.0 - 0.5); 
    
    // Корона активна везде, кроме самого центра, и плавно сходит на нет
    float coronaZone = saturate(1.0 - edgeFade); 

    // =================================================================
    // ТЕКСТУРА 1: ИСХОДНЫЙ ДИСК (cells3D)
    // =================================================================
    float3 p_disk = p + rot3(p, noise3(p * 16.41 + t * 0.5)) * 0.2;
    float cellTex = saturate( cells3D(p_disk * 12.4) * 0.18) * diskZone*4;
    
    // =================================================================
    // ТЕКСТУРА 2: МЯГКАЯ ЗА ТУХАЮЩАЯ КОРОНА
    // =================================================================
    //radialUV/=1+rot3(radialUV.xyy*noise3(p/4),noise3(p));
    // Уменьшаем частоту шума по оси лучей (95 -> 45), чтобы они были шире и мягче
    float radialNoise1 = noise3(float3(radialUV.x * 145.0, radialUV.y * 14.0, t * 4.10)) * 0.5 + 0.5;
    float radialNoise2 = noise3(float3(radialUV.x * 125.0, radialUV.y * 16.0, t * 3.3)) * 0.5 + 0.5;
    
    radialNoise1 = .321/cells3D(.05*float3(radialUV.x * 145.0, radialUV.y * 14.0, t * 1.10));
    radialNoise2 = .321/cells3D(.05*float3(radialUV.x * 125.0, radialUV.y * 16.0, t * 2.3));
    
    // МЯГКОЕ смешивание вместо жесткого pow(..., 2.2). Лучи получаются пушистыми
    float coronaTex = saturate((radialNoise1 + radialNoise2) * 0.25) * coronaZone;
    coronaTex*=.5+.3*sin(radialUV*16);
    // Плавное затухание короны к краям геометрии (умножаем на edgeFade)
    coronaTex *= edgeFade * 5.5;
    coronaTex *= 1-saturate(diskZone*2);
    
    // =================================================================
    // СМЕШИВАНИЕ И МЯГКИЙ РАСЧЕТ ЦВЕТА
    // =================================================================
    // Соединяем текстуры. Диск дает плотность в центре, корона — пушистость на краях
    
    float finalPlasma = (cellTex * 142) + (coronaTex * 4.0);

    // Градиент цвета: мягкий переход от оранжевого к дымчатому бело-желтому свечению
    //float3 centerColor = float3(1.0, 0.75, 0.5) * finalPlasma*114;
    float3 centerColor = finalPlasma*114;
    
    float3 edgeGlow    =  coronaZone * finalPlasma * 113.5;
    //return float4(edgeGlow,1);
    edgeGlow=max(edgeGlow,0)*saturate(1-diskZone*8);
    float3 finalColor = (centerColor + edgeGlow) * 222.0 * input.color.rgb;

    float coronaMask = saturate(pow(edgeFade,2));
    coronaMask=pow(.5+.5*cos(edgeFade*PI-.766),81)*10;
    coronaMask*=1-saturate(diskZone);
    finalColor+=pow(coronaMask,.8)*22222*(input.color+1);

    // =================================================================
    // РВАНЫЙ КРАЙ БЕЗ ГРЯЗИ
    // =================================================================
    // Возвращаем вашу исходную мягкую альфу для таяния краев
    float alphaMask = diskZone + coronaTex * 13.0;
    //finalColor=saturate(diskZone)*22222222;
    
    finalColor*=alphaMask;

    // Ослабляем discard, чтобы он не делал "огрызки", а лишь слегка прорежал текстуру
    if (finalPlasma * (edgeFade + 0.1) * 1.0 < 0.02 + hash(p * 12.0).r * 0.04) 
    {
     //  discard;
    }
     
    return float4(finalColor / 22222.0, saturate(alphaMask)); 
} 