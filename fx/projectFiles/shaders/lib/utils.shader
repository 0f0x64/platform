float4 getGrid(uint vID, float sep,int2 dim)
{
    uint index = vID / 6;
    float2 map[6] = { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1 };
    float2 uv = map[vID % 6];
    float2 grid = (float2((index % dim.x), index / dim.x) + (uv - .5) * sep + .5) / float2(dim.x,dim.y);
    return float4(grid, uv);
}

float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}