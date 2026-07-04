#define PI 3.1415926535897932384626433832795

float4 getGrid(uint vID, float sep,int2 dim)
{
    uint index = vID / 6;
    float2 map[6] = { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1 };
    float2 uv = map[vID % 6];
    float2 grid = (float2((index % dim.x), index / dim.x) + (uv - .5) * sep + .5) / float2(dim.x,dim.y);
    return float4(grid, uv);
}

float4 getGridP(uint vID, float sep,int2 dim)
{
    uint index = vID / 6;
    float2 map[6] = { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1 };
    float2 uv = map[vID % 6];
    float2 grid = (float2((index % dim.x), index / dim.x)) / float2(dim.x,dim.y);
    return float4(grid, uv);
}

float4 getGridInst(uint vID, uint iID, int gX,int gY)
{
    float2 map[6] = { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1 };
    return float4(float2(iID % gX, floor(iID / gX))/float2(gX,gY), map[vID % 6]); 
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

float3 rotX(float3 pos,float a)
{
    float3x3 m =
    {
        1, 0,       0,
        0, cos(a), -sin(a),
        0, sin(a), cos(a)
    };
    pos = mul(pos, m);
    return pos;
}

float3 rotY(float3 pos, float a)
{
    float3x3 m =
    {
        cos(a), 0, sin(a),
        0, 1, 0,
        -sin(a), 0, cos(a)
    };
    pos = mul(pos, m);
    return pos;
}

float3 rotZ(float3 pos, float a)
{
    float3x3 m =
    {
        cos(a), -sin(a),0,
        sin(a), cos(a), 0,
        0, 0, 1
    };
    pos = mul(pos, m);
    return pos;
}

float hash( float n ) {
        return frac(sin(n)*43758.5453);
    }
     
float noise( float3 x ) {
    // The noise function returns a value in the range -.5f -> .5f
    float3 p = floor(x);
    float3 f = frac(x);
 
    f = f*f*(3.0-2.0*f);
    float n = p.x + p.y*57.0 + 113.0*p.z;
 
    float a= lerp(lerp(lerp( hash(n+0.0), hash(n+1.0),f.x),
           lerp( hash(n+57.0), hash(n+58.0),f.x),f.y),
           lerp(lerp( hash(n+113.0), hash(n+114.0),f.x),
           lerp( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
           return a-.5;
}

float noise_u( float3 x ) {

    float3 p = floor(x);
    float3 f = frac(x);
 
    f = f*f*(3.0-2.0*f);
    float n = p.x + p.y*57.0 + 113.0*p.z;
 
    float a= lerp(lerp(lerp( hash(n+0.0), hash(n+1.0),f.x),
           lerp( hash(n+57.0), hash(n+58.0),f.x),f.y),
           lerp(lerp( hash(n+113.0), hash(n+114.0),f.x),
           lerp( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
          
           return a;
}

float3 noise3(float3 p)
{
    //return float3(noise(p.x),noise(p.y),noise(p.z));
    return float3(noise(p.xyz),noise(p.yzx),noise(p.zxy));
}

float3 noise3_u(float3 p)
{
    //return float3(noise(p.x),noise(p.y),noise(p.z));
    return float3(noise(p.xyz),noise(p.yzx),noise(p.zxy))+.5;
}

struct pos_color
{
    float4 pos;
    float4 color;
    float sz;
};

float hash_s(int qid)
{
    return hash(qid)-.5;
}

float3 rot3(float3 pos, float3 angle)
{
    return rotZ(rotY(rotX(pos,angle.x),angle.y),angle.z);
}

float3 hash3(int qid)
{
    return float3(hash(qid*.27),hash(qid*.28),hash(qid*.29))-.5;
}

float smoothf(float x)
{
    return x + (x - (x * x * (3.0 - 2.0 * x)));

}

float3 tube(float2 uv)
{
    float2 a = uv * PI * 2;
    a.x *= -1;
    //float3 pos = float3(sin(a.x), (uv.y-.5)*2, cos(a.x));
    float3 pos = float3(sin(a.x), 0, cos(a.x))*.1;
    pos=rot3(pos,noise3(float3(pos.x,uv.y,pos.z)*112));
    pos.y+=(uv.y-.5)*2;

    return pos;
}


float3 shp(float2 uv)
{
    float2 a = uv * PI * 2;
    a.x *= -1;
    float3 pos = float3(sin(a.x), sin(a.y ), cos(a.x));
    pos.xz *= cos(a.y );

    return pos;
}

float3 quantize3(float3 x,float q)
{
    return floor(x*q)/q;
}

float quantize(float x,float q)
{
    return floor(x*q)/q;
}

float4 transform(float3 pos,float2 grid, float size)
{
    float2 uv = grid-.5;
    float2 scale = float2(proj[0]._m00,proj[0]._m11);
    float4 posT;
    posT = mul(float4(pos,1), view[0]);
        float4 pt = mul(posT, proj[0]);
        float2 sz = uv*.002*(posT.z/posT.w)*size;
        sz = uv*size*.1;
        posT.xy+=sz;
        posT = mul(posT, proj[0]);

        return posT;
}

float4 transform_unisize(float3 pos,float2 grid,float size)
{
    float2 uv = grid-.5;
    float2 scale = float2(proj[0]._m00,proj[0]._m11);
    float4 posT;
    posT = mul(float4(pos,1), view[0]);
        float4 pt = mul(posT, proj[0]);
        float2 sz = uv*.002*(posT.z/posT.w)*size;

        sz*=normalize(scale)*2;
        posT = mul(posT, proj[0]);
        posT.xy+=sz;

        return posT;
}

