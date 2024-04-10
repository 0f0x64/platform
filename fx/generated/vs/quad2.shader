#include<../lib/constBuf.shader>
#include<../lib/io.shader>
VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 m[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=m[V];float4 v=float4(b/4,0,1);v.xyz=lerp(v.xyz,v.xzy,tone.x);v=mul(v,view);v=mul(v,proj);f.pos=v;f.uv=b/2.+.5;return f;}