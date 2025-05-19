//[
cbuffer camera : register(b1)
{
    float4x4 world[2];
    float4x4 view[2];
    float4x4 proj[2];
};

cbuffer frame : register(b2)
{
    float4 time;
    float4 aspect;
};

cbuffer global : register(b3)
{
    float4 gConst[32];
};
//]