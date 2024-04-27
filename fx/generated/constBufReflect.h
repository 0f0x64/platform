namespace vs{

struct { 

void set () {
Shaders::SetVS(0);
}

} box;

}

namespace vs{

struct { 

struct 
{
float width;
float height;
} params;

void set () {
Shaders::SetVS(1);
memcpy((char*)ConstBuf::drawerV,&params,sizeof(params));
}

} letter;

}

namespace vs{

struct { 

void set () {
Shaders::SetVS(2);
}

} lineDrawer;

}

namespace vs{

struct { 

void set () {
Shaders::SetVS(3);
}

} lineDrawer3d;

}

namespace vs{

struct { 

struct 
{
float gX;
float gY;
} params;

struct 
{
int positions;
int normals;
} textures;

struct 
{
int sam1Filter;
int sam1AddressU;
int sam1AddressV;
} samplers;

void set () {
Shaders::SetVS(4);
memcpy((char*)ConstBuf::drawerV,&params,sizeof(params));
Textures::SetTexture(textures.positions, 0, Textures::tAssignType::vertex); 
Textures::SetTexture(textures.normals, 1, Textures::tAssignType::vertex); 
Sampler::Set(Sampler::to::vertex, 0, samplers.sam1Filter, samplers.sam1AddressU, samplers.sam1AddressV); 
}

} objViewer;

}

namespace vs{

struct { 

void set () {
Shaders::SetVS(5);
}

} quad;

}

namespace vs{

struct { 

struct 
{
float gX;
float gY;
} params;

void set () {
Shaders::SetVS(6);
memcpy((char*)ConstBuf::drawerV,&params,sizeof(params));
}

} simpleCube;

}

namespace ps{

struct { 

struct 
{
float r;
float g;
float b;
float a;
} params;

struct 
{
int env;
int normals;
int albedo;
} textures;

struct 
{
int sam1Filter;
int sam1AddressU;
int sam1AddressV;
} samplers;

void set () {
Shaders::SetPS(0);
memcpy((char*)ConstBuf::drawerP,&params,sizeof(params));
Textures::SetTexture(textures.env, 0, Textures::tAssignType::pixel); 
Textures::SetTexture(textures.normals, 1, Textures::tAssignType::pixel); 
Textures::SetTexture(textures.albedo, 2, Textures::tAssignType::pixel); 
Sampler::Set(Sampler::to::pixel, 0, samplers.sam1Filter, samplers.sam1AddressU, samplers.sam1AddressV); 
}

} basic;

}

namespace ps{

struct { 

struct 
{
float aspect;
float rad;
float r;
float g;
float b;
float a;
float soft;
float edge;
float outlineBrightness;
} params;

void set () {
Shaders::SetPS(1);
memcpy((char*)ConstBuf::drawerP,&params,sizeof(params));
}

} box_ps;

}

namespace ps{

struct { 

struct 
{
float p;
} params;

void set () {
Shaders::SetPS(2);
memcpy((char*)ConstBuf::drawerP,&params,sizeof(params));
}

} cubemapCreator;

}

namespace ps{

struct { 

struct 
{
int env;
} textures;

struct 
{
int sam1Filter;
int sam1AddressU;
int sam1AddressV;
} samplers;

void set () {
Shaders::SetPS(3);
Textures::SetTexture(textures.env, 0, Textures::tAssignType::pixel); 
Sampler::Set(Sampler::to::pixel, 0, samplers.sam1Filter, samplers.sam1AddressU, samplers.sam1AddressV); 
}

} cubeMapViewer;

}

namespace ps{

struct { 

struct 
{
int geo;
} textures;

struct 
{
int sam1Filter;
int sam1AddressU;
int sam1AddressV;
} samplers;

void set () {
Shaders::SetPS(4);
Textures::SetTexture(textures.geo, 0, Textures::tAssignType::pixel); 
Sampler::Set(Sampler::to::pixel, 0, samplers.sam1Filter, samplers.sam1AddressU, samplers.sam1AddressV); 
}

} genNormals;

}

namespace ps{

struct { 

struct 
{
float r;
float g;
float b;
float a;
float bold;
} params;

struct 
{
int tex;
} textures;

struct 
{
int s1Filter;
int s1AddressU;
int s1AddressV;
} samplers;

void set () {
Shaders::SetPS(5);
memcpy((char*)ConstBuf::drawerP,&params,sizeof(params));
Textures::SetTexture(textures.tex, 0, Textures::tAssignType::pixel); 
Sampler::Set(Sampler::to::pixel, 0, samplers.s1Filter, samplers.s1AddressU, samplers.s1AddressV); 
}

} letter_ps;

}

namespace ps{

struct { 

struct 
{
float r;
float g;
float b;
float a;
} params;

void set () {
Shaders::SetPS(6);
memcpy((char*)ConstBuf::drawerP,&params,sizeof(params));
}

} lineDrawer_ps;

}

namespace ps{

struct { 

struct 
{
float sx;
float sy;
float sz;
} params;

void set () {
Shaders::SetPS(7);
memcpy((char*)ConstBuf::drawerP,&params,sizeof(params));
}

} obj1;

}

namespace ps{

struct { 

struct 
{
float r;
float g;
float b;
} params;

void set () {
Shaders::SetPS(8);
memcpy((char*)ConstBuf::drawerP,&params,sizeof(params));
}

} simpleFx;

}

