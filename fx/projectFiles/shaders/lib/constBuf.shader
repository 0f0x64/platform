//[
cbuffer global : register(b0)
{
    float4 gConst[32];
};

cbuffer frame : register(b1)
{
    float4 time;
};

cbuffer camera : register(b2)
{
    float4x4 world[2];
    float4x4 view[2];
    float4x4 proj[2];
};

cbuffer drawMat : register(b5)
{
    float4x4 model;
};
//]