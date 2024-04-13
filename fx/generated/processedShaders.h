//automatically generated file: all used shaders as const char* strings

namespace shadersData {

const char* objViewer = "#include<../lib/constBuf.shader>\n"
"#include<../lib/io.shader>\n"
"#include<../lib/constants.shader>\n"
"#include<../lib/utils.shader>\n"
"Texture2D positions:register(t0),normals:register(t0);SamplerState sam1:register(s0);cbuffer params:register(b0)\n"
"{\n"
"float gX,gY;\n"
"}\n"
"float3 rotY(float3 f,float s){float3x3 g;g[0]=float3(cos(s),0,sin(s));g[1]=float3(0,1,0);g[2]=float3(-sin(s),0,cos(s));return mul(f,g);}VS_OUTPUT VS(uint s:SV_VertexID){VS_OUTPUT g=(VS_OUTPUT)0;float4 f=getGrid(s,1,float2(gX,gY));float2 p=f.xy;float4 V=positions.SampleLevel(sam1,p,0);g.wpos=float4(V.xyz,0);g.vpos=mul(float4(V.xyz,1),view[0]);g.pos=mul(float4(V.xyz,1),mul(view[0],proj[0]));g.uv=f.xy;return g;}\n"
;

const char* quad = "#include<../lib/constBuf.shader>\n"
"#include<../lib/io.shader>\n"
"VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 B[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=B[V];f.pos=float4(b,0,1);f.uv=float2(1,-1)*b/2.+.5;return f;}\n"
;

const char* simpleCube = "#include<../lib/constBuf.shader>\n"
"#include<../lib/io.shader>\n"
"#include<../lib/constants.shader>\n"
"#include<../lib/utils.shader>\n"
"cbuffer params:register(b0)\n"
"{\n"
"float gX,gY;\n"
"}\n"
"float3 rotY(float3 V,float f){float3x3 g;g[0]=float3(cos(f),0,sin(f));g[1]=float3(0,1,0);g[2]=float3(-sin(f),0,cos(f));return mul(V,g);}VS_OUTPUT VS(uint f:SV_VertexID){VS_OUTPUT g=(VS_OUTPUT)0;float4 s=getGrid(f,1,float2(gX,gY));float2 V=s.xy-.5,c=V*PI*2;float3 r=float3(sin(c.x),sin(c.y/2),cos(c.x));r.xz*=cos(c.y/2);r=rotY(r,45*PI/180.);r=clamp(r,-.5,.5)*5;g.wpos=float4(r,0);g.pos=mul(float4(r,1),mul(view[0],proj[0]));g.uv=s.xy;return g;}\n"
;

const char* basic = "#include<../lib/constBuf.shader>\n"
"#include<../lib/io.shader>\n"
"TextureCube env:register(t0);Texture2D normals:register(t1);TextureCube albedo:register(t2);SamplerState sam1:register(s0);float4 PS(VS_OUTPUT n):SV_Target{float3 e=normals.SampleLevel(sam1,n.uv,1).xyz,f=n.vpos.xyz;f=mul(view[0],float4(f,1));f=normalize(f);float3 P=reflect(f,e);return env.SampleLevel(sam1,P,0);}\n"
;

const char* cubemapCreator = "#include<../lib/constBuf.shader>\n"
"#include<../lib/io.shader>\n"
"#include<../lib/constants.shader>\n"
"cbuffer params:register(b1)\n"
"{\n"
"float p;\n"
"}\n"
"struct PS_OUTPUT{float4 c0:SV_Target0;float4 c1:SV_Target1;float4 c2:SV_Target2;float4 c3:SV_Target3;float4 c4:SV_Target4;float4 c5:SV_Target5;};PS_OUTPUT PS(VS_OUTPUT c){float2 P=c.uv;float4 f=float4(P,0,1);PS_OUTPUT p;f.xyz=saturate(sin(P.x*32)*sin(P.y*32)*100).xxx;p.c0=f*float4(0,0,1,1);p.c1=f*float4(0,1,0,1);p.c2=f*float4(0,1,1,1);p.c3=f*float4(1,0,0,1);p.c4=f*float4(1,0,1,1);p.c5=f*float4(1,1,0,1);return p;}\n"
;

const char* cubeMapViewer = "#include<../lib/constBuf.shader>\n"
"#include<../lib/io.shader>\n"
"TextureCube env:register(t0);SamplerState sam1:register(s0);float4 PS(VS_OUTPUT s):SV_Target{return env.SampleLevel(sam1,float4(normalize(s.wpos.xyz),0),0);}\n"
;

const char* genNormals = "#include<../lib/constBuf.shader>\n"
"#include<../lib/io.shader>\n"
"#include<../lib/constants.shader>\n"
"Texture2D geo:register(t0);SamplerState sam1:register(s0);float4 PS(VS_OUTPUT o):SV_Target{uint f,l;geo.GetDimensions(f,l);float g=1;l*=g;f*=g;float2 s=o.uv;s.y=1-s.y;float4 P=geo.Sample(sam1,s+float2(-1./f,0)),y=geo.Sample(sam1,s+float2(1./f,0)),B=geo.Sample(sam1,s+float2(0,-1./l)),D=geo.Sample(sam1,s+float2(0,1./l));float3 n=normalize(cross(P.xyz-y.xyz,B.xyz-D.xyz)),V=n;return float4(V,1);}\n"
;

const char* obj1 = "#include<../lib/constBuf.shader>\n"
"#include<../lib/io.shader>\n"
"#include<../lib/constants.shader>\n"
"cbuffer params:register(b1)\n"
"{\n"
"float sx,sy,sz;\n"
"};\n"
"float3 sphere(float2 s){float2 f=s*PI*2;float3 r=float3(sin(f.x),sin(f.y/2),cos(f.x));r.xz*=cos(f.y/2);r=clamp(r,-.5,.5);r*=.45;return r;}float4 PS(VS_OUTPUT f):SV_Target{float2 r=f.uv-.5;float3 s=sphere(r)*2;return float4(s,1.);}\n"
;

const char* simpleFx = "#include<../lib/constBuf.shader>\n"
"#include<../lib/io.shader>\n"
"cbuffer params:register(b1)\n"
"{\n"
"float r,g,b;\n"
"};\n"
"float4 PS(VS_OUTPUT e):SV_Target{float s=3.141519,x=0;for(int f=1;f<3;f++){float2 b=2*(e.uv-.5)*s;b+=float2(sin(time.x*.13*sin(f*.4)),sin(time.x*.12*sin(f*.5)));x+=sin(atan2(b.x,b.y)*12-time.x*.3)*sin(1/length(b*2)+5)*saturate(1/pow(length(b),3))*2;}return float4(x*r,x*g,x*b,1.);}\n"
;

const char* constants = "#define PI 3.1415926535897932384626433832795\n"
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

const char* io = "struct VS_OUTPUT{float4 pos:SV_POSITION;float4 vpos:POSITION0;float4 wpos:POSITION1;float2 uv:TEXCOORD0;};\n"
;

const char* noise = "float hash(float h){return frac(sin(h)*43758.5453);}float noise(float3 h){float3 f=floor(h),x=frac(h);x=x*x*(3.-2.*x);float l=f.x+f.y*57.+113.*f.z;return lerp(lerp(lerp(hash(l),hash(l+1.),x.x),lerp(hash(l+57.),hash(l+58.),x.x),x.y),lerp(lerp(hash(l+113.),hash(l+114.),x.x),lerp(hash(l+170.),hash(l+171.),x.x),x.y),x.z);}\n"
;

const char* sinwave = "float wave(float3 f){return sin(f.x)*cos(f.y)/5;}\n"
;

const char* utils = "float4 getGrid(uint f,float g,float2 u){uint G=f/6;float2 L[6]={0,0,1,0,1,1,0,0,1,1,0,1},x=L[f%6],J=(float2(G%uint(u.x),G/uint(u.x))+(x-.5)*g+.5)/u;return float4(J,x);}\n"
;



};