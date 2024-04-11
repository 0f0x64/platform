//automatically generated file: all used shaders as const char* strings

namespace shadersData {

const char* meshOut = "#include<../lib/constBuf.shader>\n"
"#include<../lib/io.shader>\n"
"cbuffer params:register(b0)\n"
"{\n"
"float tone,x1,x2;\n"
"}\n"
"VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 m[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=m[V];float4 p=float4(b/4,0,1);p.xyz=lerp(p.xyz,p.xzy,tone);p=mul(p,view[0]);p=mul(p,proj[0]);f.pos=p;f.uv=b/2.+.5;return f;}\n"
;

const char* quad = "#include<../lib/constBuf.shader>\n"
"#include<../lib/io.shader>\n"
"VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 B[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=B[V];f.pos=float4(b,0,1);f.uv=b/3.+.5;return f;}\n"
;

const char* simpleFx = "#include<../lib/constBuf.shader>\n"
"#include<../lib/io.shader>\n"
"cbuffer params:register(b1)\n"
"{\n"
"float p1;float p2;\n"
"};\n"
"float4 PS(VS_OUTPUT e):SV_Target{float s=3.141519,f=0;for(int p=1;p<3;p++){float2 x=2*(e.uv-.5)*s;x+=float2(sin(time.x*.13*sin(p*.4)),sin(time.x*.12*sin(p*.5)));f+=sin(atan2(x.x,x.y)*12-time.x*.3)*sin(1/length(x*2)+5)*saturate(1/pow(length(x),3))*2;}return float4(f,f,f,1.);}\n"
;

const char* simpleTex = "#include<../lib/constBuf.shader>\n"
"#include<../lib/io.shader>\n"
"Texture2D albedo:register(t0),metalness:register(t1);SamplerState sam1:register(s0);cbuffer params:register(b1)\n"
"{\n"
"float tone;\n"
"float p2;\n"
"}\n"
"float4 PS(VS_OUTPUT s):SV_Target{float4 p=float4(s.uv,sin(s.uv.x*12),1);float2 f=s.uv*5;f.y-=2;p=albedo.Sample(sam1,f);return p;}\n"
;

const char* constBuf = "cbuffer global:register(b5)\n"
"{\n"
"float4 gConst[32];\n"
"};\n"
"cbuffer frame:register(b4)\n"
"{\n"
"float4 time;\n"
"};\n"
"cbuffer camera:register(b3)\n"
"{\n"
"float4x4 world[2];\n"
"float4x4 view[2];\n"
"float4x4 proj[2];\n"
"};\n"
"cbuffer drawMat:register(b2)\n"
"{\n"
"float4x4 model;\n"
"};\n"
;

const char* io = "struct VS_OUTPUT{float4 pos:SV_POSITION;float2 uv:TEXCOORD0;};\n"
;

const char* noise = "float hash(float h){return frac(sin(h)*43758.5453);}float noise(float3 h){float3 f=floor(h),x=frac(h);x=x*x*(3.-2.*x);float l=f.x+f.y*57.+113.*f.z;return lerp(lerp(lerp(hash(l),hash(l+1.),x.x),lerp(hash(l+57.),hash(l+58.),x.x),x.y),lerp(lerp(hash(l+113.),hash(l+114.),x.x),lerp(hash(l+170.),hash(l+171.),x.x),x.y),x.z);}\n"
;

const char* sinwave = "float wave(float3 f){return sin(f.x)*cos(f.y)/5;}\n"
;



};