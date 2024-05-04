//automatically generated file: all used shaders as const char* strings

namespace shadersData {

const char* box = 
"#include <../lib/constBuf.shader>\n"
"#include <../lib/io.shader>\n"
"#include <../lib/constants.shader>\n"
"#include <../lib/utils.shader>\n"
"\n"
"//[\n"
"cbuffer geo : register(b6)\n"
"{\n"
"    float4 pos_size[256];\n"
"}\n"
"//]\n"
"\n"
"VS_OUTPUT_POS_UV VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)\n"
"{\n"
"    VS_OUTPUT_POS_UV output;\n"
"    float2 quad[6] = { 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1 };\n"
"    float2 p = (quad[vID] - float2(0, 1)) * pos_size[iID].zw*2. + pos_size[iID].xy * float2(2, -2) - float2(1, -1);\n"
"    output.pos = float4(p, 0, 1);\n"
"    output.uv = quad [vID];\n"
"    output.uv.y = 1 - output.uv.y;\n"
"    output.sz = pos_size[iID].zw;\n"
"    return output;\n"
"}\n"
"\n"
;

const char* letter = 
"#include <../lib/constBuf.shader>\n"
"#include <../lib/io.shader>\n"
"#include <../lib/constants.shader>\n"
"#include <../lib/utils.shader>\n"
"\n"
"//[\n"
"cbuffer params : register(b0)\n"
"{\n"
"    float width, height;\n"
"}\n"
"\n"
"cbuffer geo : register(b6)\n"
"{\n"
"    float4 pos_size[256];\n"
"}\n"
"//]\n"
"\n"
"VS_OUTPUT_POS_UV VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)\n"
"{\n"
"    VS_OUTPUT_POS_UV output;\n"
"    float2 quad[6] = { 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1 };\n"
"    float2 p = (quad[vID] - float2(0, 1)) * float2(width, height) + pos_size[iID].xy * float2(2, -2) - float2(1,-1);\n"
"    output.pos = float4(p, 0, 1);\n"
"    output.uv = float2(quad[vID].x, 1- quad[vID].y);\n"
"    output.uv.y /= 96.;\n"
"    output.uv.y+= pos_size[iID].z/96.;\n"
"    \n"
"    //output.sz = float2(width, height);\n"
"    return output;\n"
"}\n"
;

const char* lineDrawer = 
"#include <../lib/constBuf.shader>\n"
"#include <../lib/io.shader>\n"
"#include <../lib/constants.shader>\n"
"\n"
"//[\n"
"\n"
"cbuffer geo : register(b6)\n"
"{\n"
"    float4 position[4000];\n"
"}\n"
"\n"
"//]\n"
"\n"
"\n"
"VS_OUTPUT_POSONLY VS(uint vID : SV_VertexID)\n"
"{\n"
"    VS_OUTPUT_POSONLY output = (VS_OUTPUT_POSONLY) 0;\n"
"    output.pos.xy = position[vID].xy * float2(2, -2) + float2(-1,1);\n"
"    output.pos.z = 0;\n"
"    output.pos.w = 1;\n"
"\n"
"    return output;\n"
"}\n"
;

const char* lineDrawer3d = 
"#include <../lib/constBuf.shader>\n"
"#include <../lib/io.shader>\n"
"#include <../lib/constants.shader>\n"
"\n"
"//[\n"
"\n"
"cbuffer geo : register(b6)\n"
"{\n"
"    float4 position[4000];\n"
"}\n"
"\n"
"//]\n"
"\n"
"\n"
"VS_OUTPUT_POSONLY VS(uint vID : SV_VertexID)\n"
"{\n"
"    VS_OUTPUT_POSONLY output = (VS_OUTPUT_POSONLY) 0;\n"
"    float4 pos = float4(position[vID].xyz, 1);\n"
"    output.pos = mul(pos, mul(view[0], proj[0]));\n"
"\n"
"    return output;\n"
"}\n"
;

const char* objViewer = 
"#include <../lib/constBuf.shader>\n"
"#include <../lib/io.shader>\n"
"#include <../lib/constants.shader>\n"
"#include <../lib/utils.shader>\n"
"\n"
"Texture2D positions : register(t0);\n"
"Texture2D normals : register(t0);\n"
"SamplerState sam1 : register(s0);\n"
"\n"
"//[\n"
"cbuffer params : register(b0)\n"
"{\n"
"    float gX, gY;\n"
"}\n"
"//]\n"
"\n"
"float3 rotY(float3 p, float a)\n"
"{\n"
"    float3x3 r;\n"
"    r[0] = float3(cos(a), 0, sin(a));\n"
"    r[1] = float3(0, 1, 0);\n"
"    r[2] = float3(-sin(a), 0, cos(a));\n"
"    return mul(p, r);\n"
"}\n"
"\n"
"\n"
"VS_OUTPUT VS(uint vID : SV_VertexID)\n"
"{\n"
"    VS_OUTPUT output = (VS_OUTPUT) 0;\n"
"    \n"
"    float4 grid = getGrid(vID, 1, float2(gX,gY));\n"
"    float2 uv = grid.xy;\n"
"    \n"
"    float4 pos = positions.SampleLevel(sam1, uv, 0);\n"
"    pos = mul(pos, model);\n"
"    //---\n"
"    output.wpos = float4(pos.xyz, 0);\n"
"    output.vpos = mul(float4(pos.xyz, 1), view[0]);\n"
"    \n"
"    output.pos = mul(float4(pos.xyz, 1), mul(view[0], proj[0]));\n"
"    output.uv = grid.xy;\n"
"    return output;\n"
"}\n"
;

const char* quad = 
"#include <../lib/constBuf.shader>\n"
"#include <../lib/io.shader>\n"
"\n"
"VS_OUTPUT VS(uint vID : SV_VertexID)\n"
"{\n"
"    VS_OUTPUT output = (VS_OUTPUT) 0;\n"
"    float2 quad[6] = { -1, -1, 1, -1, -1, 1, 1, -1, 1, 1, -1, 1 };\n"
"    float2 p = quad[vID];\n"
"    output.pos = float4(p, 0, 1);\n"
"    output.uv = float2(1,-1)* p / 2. + .5;\n"
"    return output;\n"
"}\n"
;

const char* simpleCube = 
"#include <../lib/constBuf.shader>\n"
"#include <../lib/io.shader>\n"
"#include <../lib/constants.shader>\n"
"#include <../lib/utils.shader>\n"
"\n"
"//[\n"
"cbuffer params : register(b0)\n"
"{\n"
"    float  gX,gY;\n"
"}\n"
"//]\n"
"\n"
"float3 rotY(float3 p, float a)\n"
"{\n"
"    float3x3 r;\n"
"    r[0] = float3(cos(a), 0, sin(a));\n"
"    r[1] = float3(0, 1, 0);\n"
"    r[2] = float3(-sin(a), 0, cos(a));\n"
"    return mul(p, r);\n"
"}\n"
"\n"
"VS_OUTPUT VS(uint vID : SV_VertexID)\n"
"{\n"
"    VS_OUTPUT output = (VS_OUTPUT) 0;\n"
"    \n"
"    float4 grid = getGrid(vID, 1, float2(gX, gY));\n"
"    float2 uv = grid.xy-.5;\n"
"    \n"
"    float2 a = uv * PI * 2;\n"
"    float3 pos = float3(sin(a.x), sin(a.y / 2), cos(a.x));\n"
"    pos.xz *= cos(a.y / 2);\n"
"    pos = rotY(pos, 45 * PI / 180.);\n"
"    pos = clamp(pos, -.5, .5)*6;\n"
"\n"
"    //---\n"
"    output.wpos = float4(pos, 0);\n"
"    output.pos = mul(float4(pos, 1), mul(view[0], proj[0]));\n"
"    \n"
"    output.uv = grid.xy;\n"
"    return output;\n"
"}\n"
;

const char* basic = 
"#include <../lib/constBuf.shader>\n"
"#include <../lib/io.shader>\n"
"\n"
"TextureCube env : register(t0);\n"
"Texture2D normals : register(t1);\n"
"TextureCube albedo : register(t2);\n"
"\n"
"SamplerState sam1 : register(s0);\n"
"\n"
"//[\n"
"cbuffer params : register(b1)\n"
"{\n"
"    float r,g,b,a;\n"
"};\n"
"//]\n"
"\n"
"float3 FresnelSchlick(float3 F0, float3 v, float3 n)\n"
"{\n"
"    float cosTheta = dot(-n, v);\n"
"    return saturate(F0 + (1.0 - F0) * pow(1.0 - saturate(cosTheta), 5.0));\n"
"}\n"
"\n"
"\n"
"float4 PS(VS_OUTPUT input) : SV_Target\n"
"{\n"
"    float2 uv = input.uv;\n"
"    float3 albedo = float3(1, .5, .3);\n"
"    float roughness = 0;\n"
"    float metalness = 1;\n"
"    float3 F0 = .04;\n"
"    \n"
"    float3 nrml = normals.SampleLevel(sam1, input.uv,1).xyz;\n"
"    float3 eye = input.vpos.xyz;\n"
"    eye = normalize(mul((view[0]), float4(eye, 1)));\n"
"    eye = normalize(eye);\n"
"    float3 ref = reflect(eye,nrml);\n"
"    float3 specR = env.SampleLevel(sam1, ref, roughness * 10)*2;\n"
"    float3 diffR = env.SampleLevel(sam1, -nrml, 8.5)*2;\n"
"    \n"
"    float3 albedo_ = lerp(albedo, 0, metalness);\n"
"    F0 = lerp(F0, F0 * albedo, metalness);\n"
"    float3 F = FresnelSchlick(F0, eye, -nrml);\n"
"    F *= lerp(saturate(1 - roughness), 1, metalness);\n"
"    float3 specK = specR * F;\n"
"    float3 diffK = saturate(1.0 - F);\n"
"    diffK = albedo * diffK * diffR;\n"
"\n"
"    float3 color = diffK + specK;\n"
"\n"
"\n"
"    return float4(color, 1);\n"
"}\n"
;

const char* box_ps = 
"#include <../lib/constBuf.shader>\n"
"#include <../lib/io.shader>\n"
"\n"
"//[\n"
"cbuffer params : register(b1)\n"
"{\n"
"    float aspect;\n"
"    float rad;\n"
"    float r, g, b, a;\n"
"    float soft;\n"
"    float edge;\n"
"    float outlineBrightness;\n"
"}\n"
"//]\n"
"\n"
"float roundedBoxSDF(float2 CenterPosition, float2 Size, float Radius)\n"
"{\n"
"    return length(max(abs(CenterPosition) - Size + Radius, 0.0)) - Radius;\n"
"}\n"
"\n"
"float calcRA(float2 uv, float2 sz, float radius)\n"
"{\n"
"    float2 ca = float2(1, aspect);\n"
"    float2 s = sz*ca;\n"
"    float r1 = min(s.x, s.y) * (0.001 + radius);\n"
"    float r2 = roundedBoxSDF(uv*ca*sz, s*.5, r1)  / r1;\n"
"    return -r2;\n"
"\n"
"}\n"
"\n"
"float4 PS(VS_OUTPUT_POS_UV input) : SV_Target\n"
"{\n"
"    float4 color = float4(r, g, b, a);\n"
"    float2 uvs = (input.uv - .5);\n"
"    float d = calcRA(uvs, input.sz, rad);\n"
"    float embossMask = sign(d) - saturate(d * edge * input.sz);\n"
"    float emboss = embossMask * dot(atan(uvs - .1), -.25);\n"
"    color.rgb += emboss;\n"
"    float outline = 1 - saturate(d * 64. * input.sz.y); // * float3(r, g, b);\n"
"    color.rgb += outline*outlineBrightness;\n"
"    return float4(color.rgb, saturate(d*soft)*color.a);\n"
"}\n"
;

const char* cubemapCreator = 
"#include <../lib/constBuf.shader>\n"
"#include <../lib/io.shader>\n"
"#include <../lib/constants.shader>\n"
"\n"
"//[\n"
"cbuffer params : register(b1)\n"
"{\n"
"    float p;\n"
"}\n"
"//]\n"
"\n"
"struct PS_OUTPUT\n"
"{\n"
"    float4 c0 : SV_Target0;\n"
"    float4 c1 : SV_Target1;\n"
"    float4 c2 : SV_Target2;\n"
"    float4 c3 : SV_Target3;\n"
"    float4 c4 : SV_Target4;\n"
"    float4 c5 : SV_Target5;\n"
"    \n"
"};\n"
"\n"
"PS_OUTPUT PS(VS_OUTPUT input) \n"
"{\n"
"\n"
"    float2 uv = input.uv;\n"
"    float4 color = 1;//    float4(uv, 0, 1);\n"
"\n"
"    PS_OUTPUT cube;\n"
"    float a = PI*4;\n"
"    float walls = saturate(1 - 2 * length(uv - .5));\n"
"    color.xyz = saturate(-sign(max(sin(uv.x * a), sin(uv.y * a))));\n"
"    cube.c0 = walls * float4(0, 0, 1 ,1);\n"
"    cube.c1 = walls * float4(0, 1, 0, 1);\n"
"    float top = saturate(-sign(max(sin(uv.x * a), sin(uv.y * a)))) * 12;\n"
"    cube.c2 = top;\n"
"    float floor = saturate(-sign((sin(uv.x * a * 4) * sin(uv.y * a * 4)))) * (1 - length(uv - .5)*2);\n"
"    cube.c3 = float4(.5, .5, .35, 1)*floor;\n"
"    cube.c4 = walls * float4(1, 0, 0, 1);\n"
"    cube.c5 = walls * float4(1, 0, 1, 1);\n"
"    return cube;\n"
"}\n"
;

const char* cubeMapViewer = 
"#include <../lib/constBuf.shader>\n"
"#include <../lib/io.shader>\n"
"\n"
"TextureCube env : register(t0);\n"
"SamplerState sam1 : register(s0);\n"
"\n"
"float4 PS(VS_OUTPUT input) : SV_Target\n"
"{\n"
"    float4 color = env.SampleLevel(sam1, float4(normalize(input.wpos.xyz),0), 0);\n"
"    return color;\n"
"}\n"
;

const char* genNormals = 
"#include <../lib/constBuf.shader>\n"
"#include <../lib/io.shader>\n"
"#include <../lib/constants.shader>\n"
"\n"
"Texture2D geo : register(t0);\n"
"SamplerState sam1 : register(s0);\n"
"\n"
"float4 PS(VS_OUTPUT input) : SV_Target\n"
"{\n"
"    uint sgX, sgY;\n"
"    geo.GetDimensions(sgX, sgY);\n"
"\n"
"    float a = 1;\n"
"    sgY *= a;\n"
"    sgX *= a;\n"
"    float2 uv = input.uv;\n"
"\n"
"    float4 pt0 = geo.Sample(sam1, uv + float2(-1. / sgX, 0));\n"
"    float4 pt1 = geo.Sample(sam1, uv + float2(1. / sgX, 0));\n"
"    float4 pt2 = geo.Sample(sam1, uv + float2(0, -1. / sgY));\n"
"    float4 pt3 = geo.Sample(sam1, uv + float2(0, 1. / sgY));\n"
"    float3 n = normalize(cross(pt0.xyz - pt1.xyz, pt2.xyz - pt3.xyz));\n"
"    \n"
"    float3 color = n;\n"
"    return float4(color, 1);\n"
"\n"
"}\n"
;

const char* letter_ps = 
"#include <../lib/constBuf.shader>\n"
"#include <../lib/io.shader>\n"
"\n"
"Texture2D tex : register(t0);\n"
"SamplerState s1 : register(s0);\n"
"\n"
"//[\n"
"cbuffer params : register(b1)\n"
"{\n"
"    float r, g, b, a;\n"
"    float bold;\n"
"}\n"
"//]\n"
"\n"
"\n"
"float4 PS(VS_OUTPUT_POS_UV input) : SV_Target\n"
"{\n"
"    float4 color = tex.SampleLevel(s1, input.uv, 0);\n"
"    color.a = dot(color.rgb, 1);\n"
"    color *= float4(r, g, b, a);\n"
"    return float4(color);\n"
"}\n"
;

const char* lineDrawer_ps = 
"#include <../lib/io.shader>\n"
"\n"
"//[\n"
"cbuffer params : register(b1)\n"
"{\n"
"    float r,g,b,a;\n"
"};\n"
"//]\n"
"\n"
"\n"
"\n"
"float4 PS(VS_OUTPUT_POSONLY input) : SV_Target\n"
"{\n"
"    return float4(r, g, b, a);\n"
"}\n"
;

const char* obj1 = 
"#include <../lib/constBuf.shader>\n"
"#include <../lib/io.shader>\n"
"#include <../lib/constants.shader>\n"
"\n"
"//[\n"
"cbuffer params : register(b1)\n"
"{\n"
"    float sx,sy,sz;\n"
"};\n"
"//]\n"
"\n"
"float3 sphere(float2 uv)\n"
"{\n"
"\n"
"    float2 a = uv * PI * 2;\n"
"       a.x *= -1;\n"
"    float3 pos = float3(sin(a.x), sin(a.y / 2), cos(a.x));\n"
"    pos.xz *= cos(a.y / 2);\n"
"       //pos = clamp(pos, -.75, .75);\n"
"    pos *= .45;\n"
"    float b = 64;\n"
"    pos /= 1 - pow(abs(sin(uv.x * b) + cos(uv.y * b/2)),8) * .00021;\n"
"    b = 64 + 11112 * sin(uv.x * 12) * cos(uv.y * 22);\n"
"    pos /= 1 - pow(abs(sin(uv.x * b) + cos(uv.y * b / 2)), 8) * .000021;\n"
"    \n"
"    return pos;\n"
"}\n"
"\n"
"float4 PS(VS_OUTPUT input) : SV_Target\n"
"{\n"
"    float2 uv = input.uv - .5;\n"
"    float3 pos = sphere(uv)*2;\n"
"    \n"
"    return float4(pos, 1.);\n"
"\n"
"}\n"
;

const char* simpleFx = 
"#include <../lib/constBuf.shader>\n"
"#include <../lib/io.shader>\n"
"\n"
"//[\n"
"cbuffer params : register(b1)\n"
"{\n"
"    float r, g, b;\n"
"};\n"
"//]\n"
"\n"
"float4 PS(VS_OUTPUT input) : SV_Target\n"
"{\n"
"    float pi = 3.141519;\n"
"    \n"
"    float c = 0;\n"
"    for (int i = 1; i < 3;i++)\n"
"    {\n"
"        float2 uv = 2 * ((input.uv) - .5)*pi;\n"
"        uv += float2(sin(time.x * .13 * sin(i*.4)), sin(time.x * .12 * sin(i*.5)));\n"
"        c += sin((atan2(uv.x, uv.y) * 12 - time.x * .3)) * (sin(1/length(uv * 2) + 5)) * saturate(1/pow(length(uv),3))*2;\n"
"    }\n"
"\n"
"    return float4(c*r, c*g, c*b, 1.);\n"
"\n"
"} \n"
;

const char* constants = 
"#define PI 3.1415926535897932384626433832795\n"
;

const char* constBuf = 
"//[\n"
"cbuffer global : register(b5)\n"
"{\n"
"    float4 gConst[32];\n"
"};\n"
"\n"
"cbuffer frame : register(b4)\n"
"{\n"
"    float4 time;\n"
"};\n"
"\n"
"cbuffer camera : register(b3)\n"
"{\n"
"    float4x4 world[2];\n"
"    float4x4 view[2];\n"
"    float4x4 proj[2];\n"
"};\n"
"\n"
"cbuffer drawMat : register(b2)\n"
"{\n"
"    float4x4 model;\n"
"};\n"
"//]\n"
;

const char* io = 
"struct VS_OUTPUT_POSONLY\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"};\n"
"\n"
"struct VS_OUTPUT_POS_UV\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float2 uv : TEXCOORD0;\n"
"    float2 sz : TEXCOORD1;\n"
"};\n"
"\n"
"\n"
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 vpos : POSITION0;\n"
"    float4 wpos : POSITION1;\n"
"    float2 uv : TEXCOORD0;\n"
"};\n"
"\n"
;

const char* noise = 
"float hash(float n)\n"
"{\n"
"    return frac(sin(n) * 43758.5453);\n"
"}\n"
"     \n"
"float noise(float3 x)\n"
"{\n"
"    float3 p = floor(x);\n"
"    float3 f = frac(x);\n"
"     \n"
"    f = f * f * (3.0 - 2.0 * f);\n"
"    float n = p.x + p.y * 57.0 + 113.0 * p.z;\n"
"     \n"
"    return lerp(lerp(lerp(hash(n + 0.0), hash(n + 1.0), f.x),\n"
"               lerp(hash(n + 57.0), hash(n + 58.0), f.x), f.y),\n"
"               lerp(lerp(hash(n + 113.0), hash(n + 114.0), f.x),\n"
"               lerp(hash(n + 170.0), hash(n + 171.0), f.x), f.y), f.z);\n"
"}\n"
;

const char* sinwave = 
"float wave(float3 x)\n"
"{\n"
"    return sin(x.x) * cos(x.y) / 5;\n"
"}\n"
;

const char* utils = 
"float4 getGrid(uint vID, float sep,float2 dim)\n"
"{\n"
"    uint index = vID / 6;\n"
"    float2 map[6] = { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1 };\n"
"    float2 uv = map[vID % 6];\n"
"    float2 grid = (float2((index % uint(dim.x)), index / uint(dim.x)) + (uv - .5) * sep + .5) / dim;\n"
"    return float4(grid, uv);\n"
"}\n"
;



};