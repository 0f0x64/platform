float4 getGrid(uint f,float g,float2 u){uint G=f/6;float2 L[6]={0,0,1,0,1,1,0,0,1,1,0,1},x=L[f%6],J=(float2(G%uint(u.x),G/uint(u.x))+(x-.5)*g+.5)/u;return float4(J,x);}