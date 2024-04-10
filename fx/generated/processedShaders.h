//automatically generated file: all used shaders as const char* strings

namespace shadersData {

const char* meshOut = "#include<../lib/constBuf.shader>\n"
"#include<../lib/io.shader>\n"
"cbuffer params:register(b3)\n"
"{\n"
"float4 tone;\n"
"}\n"
"VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 m[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=m[V];float4 e=float4(b/4,0,1);e.xyz=lerp(e.xyz,e.xzy,tone.x);e=mul(e,view[0]);e=mul(e,proj[0]);f.pos=e;f.uv=b/2.+.5;return f;}\n"
;

const char* quad = "#include<../lib/constBuf.shader>\n"
"#include<../lib/io.shader>\n"
"VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 B[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=B[V];f.pos=float4(b,0,1);f.uv=b/3.+.5;return f;}\n"
;

const char* simpleFx = "#include<../lib/constBuf.shader>\n"
"#include<../lib/io.shader>\n"
"cbuffer params:register(b4)\n"
"{\n"
"float4 p1;\n"
"};\n"
"float4 PS(VS_OUTPUT e):SV_Target{float s=3.141519,t=0;for(int x=1;x<3;x++){float2 f=2*(e.uv-.5)*s;f+=float2(sin(time.x*.13*sin(x*.4)),sin(time.x*.12*sin(x*.5)));t+=sin(atan2(f.x,f.y)*12-time.x*.3)*sin(1/length(f*2)+5)*saturate(1/pow(length(f),3))*2;}return float4(t,t,t,1.);}\n"
;

const char* simpleTex = "#include<../lib/constBuf.shader>\n"
"#include<../lib/io.shader>\n"
"Texture2D tex1:register(t0);SamplerState sam1:register(s0);cbuffer params:register(b3)\n"
"{\n"
"float4 tone;\n"
"}\n"
"float4 PS(VS_OUTPUT t):SV_Target{float4 P=float4(t.uv,sin(t.uv.x*12),1);float2 f=t.uv*5;f.y-=2;P=tex1.Sample(sam1,f);return P;}\n"
;

const char* constBuf = "cbuffer global:register(b0)\n"
"{\n"
"float4 gConst[32];\n"
"};\n"
"cbuffer frame:register(b1)\n"
"{\n"
"float4 time;\n"
"};\n"
"cbuffer camera:register(b2)\n"
"{\n"
"float4x4 world[2];\n"
"float4x4 view[2];\n"
"float4x4 proj[2];\n"
"};\n"
"cbuffer drawMat:register(b4)\n"
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


void CompileAll ()
{
Vertex (0, meshOut);
Vertex (1, quad);
Pixel (0, simpleFx);
Pixel (1, simpleTex);
};


};