#include<../lib/constBuf.shader>
#include<../lib/io.shader>
VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float2 B[6]={-1,-1,1,-1,-1,1,1,-1,1,1,-1,1},b=B[V];f.pos=float4(b,0,1);f.uv=b/2.+.5;return f;}