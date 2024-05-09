namespace vs{

struct { 

void set () {
Shaders::vShader(0);
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
Shaders::vShader(1);
memcpy((char*)ConstBuf::drawerV,&params,sizeof(params));
}

} letter;

}

namespace vs{

struct { 

void set () {
Shaders::vShader(2);
}

} lineDrawer;

}

namespace vs{

struct { 

void set () {
Shaders::vShader(3);
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
texture positions;
texture normals;
} textures;

struct 
{
filter sam1Filter;
addr sam1AddressU;
addr sam1AddressV;
} samplers;

void set () {
Shaders::vShader(4);
memcpy((char*)ConstBuf::drawerV,&params,sizeof(params));
Textures::TextureToShader((texture)textures.positions, 0, targetshader::vertex); 
Textures::TextureToShader((texture)textures.normals, 1, targetshader::vertex); 
Sampler::Sampler(targetshader::vertex, 0, samplers.sam1Filter, samplers.sam1AddressU, samplers.sam1AddressV); 
}

} objViewer;

}

namespace vs{

struct { 

void set () {
Shaders::vShader(5);
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
Shaders::vShader(6);
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
texture env;
texture normals;
texture albedo;
} textures;

struct 
{
filter sam1Filter;
addr sam1AddressU;
addr sam1AddressV;
} samplers;

void set () {
Shaders::pShader(0);
memcpy((char*)ConstBuf::drawerP,&params,sizeof(params));
Textures::TextureToShader((texture)textures.env, 0, targetshader::pixel); 
Textures::TextureToShader((texture)textures.normals, 1, targetshader::pixel); 
Textures::TextureToShader((texture)textures.albedo, 2, targetshader::pixel); 
Sampler::Sampler(targetshader::pixel, 0, samplers.sam1Filter, samplers.sam1AddressU, samplers.sam1AddressV); 
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
Shaders::pShader(1);
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
Shaders::pShader(2);
memcpy((char*)ConstBuf::drawerP,&params,sizeof(params));
}

} cubemapCreator;

}

namespace ps{

struct { 

struct 
{
texture env;
} textures;

struct 
{
filter sam1Filter;
addr sam1AddressU;
addr sam1AddressV;
} samplers;

void set () {
Shaders::pShader(3);
Textures::TextureToShader((texture)textures.env, 0, targetshader::pixel); 
Sampler::Sampler(targetshader::pixel, 0, samplers.sam1Filter, samplers.sam1AddressU, samplers.sam1AddressV); 
}

} cubeMapViewer;

}

namespace ps{

struct { 

struct 
{
texture geo;
} textures;

struct 
{
filter sam1Filter;
addr sam1AddressU;
addr sam1AddressV;
} samplers;

void set () {
Shaders::pShader(4);
Textures::TextureToShader((texture)textures.geo, 0, targetshader::pixel); 
Sampler::Sampler(targetshader::pixel, 0, samplers.sam1Filter, samplers.sam1AddressU, samplers.sam1AddressV); 
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
texture tex;
} textures;

struct 
{
filter s1Filter;
addr s1AddressU;
addr s1AddressV;
} samplers;

void set () {
Shaders::pShader(5);
memcpy((char*)ConstBuf::drawerP,&params,sizeof(params));
Textures::TextureToShader((texture)textures.tex, 0, targetshader::pixel); 
Sampler::Sampler(targetshader::pixel, 0, samplers.s1Filter, samplers.s1AddressU, samplers.s1AddressV); 
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
Shaders::pShader(6);
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
Shaders::pShader(7);
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
Shaders::pShader(8);
memcpy((char*)ConstBuf::drawerP,&params,sizeof(params));
}

} simpleFx;

}

