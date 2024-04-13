float4 getGrid(uint vID, float sep,float2 dim)
{
    uint index = vID / 6;
    float2 map[6] = { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1 };
    float2 uv = map[vID % 6];
    float2 grid = (float2((index % uint(dim.x)), index / uint(dim.x)) + (uv - .5) * sep + .5) / dim;
    return float4(grid, uv);
}