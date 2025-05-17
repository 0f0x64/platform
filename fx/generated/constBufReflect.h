namespace vs{

struct { 

struct 
{
float4 pos_size[256];
} params;

void set () {
context->UpdateSubresource(dx11::Shaders::VS[0].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[0].params);
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
float4 _pos_size[256];
} params;

void set () {
context->UpdateSubresource(dx11::Shaders::VS[1].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[1].params);
Shaders::vShader(1);
}

} letter;

}

namespace vs{

struct { 

struct 
{
float4 _pos [4000];
} params;

void set () {
context->UpdateSubresource(dx11::Shaders::VS[2].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[2].params);
Shaders::vShader(2);
}

} lineDrawer;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
float4 _pos [4000];
} params;

void set () {
context->UpdateSubresource(dx11::Shaders::VS[3].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[3].params);
Shaders::vShader(3);
}

} lineDrawer3d;

}

namespace vs{

struct { 

struct 
{
int gX;
int gY;
float4x4 model;
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
context->UpdateSubresource(dx11::Shaders::VS[4].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[4].params);
Shaders::vShader(4);
Textures::TextureToShader((texture)textures.positions, 0, targetshader::vertex); 
Textures::TextureToShader((texture)textures.normals, 1, targetshader::vertex); 
Sampler::Sampler(targetshader::vertex, 0, samplers.sam1Filter, samplers.sam1AddressU, samplers.sam1AddressV); 
}

} objViewer;

}

namespace vs{

struct { 

struct {} params;

void set () {
Shaders::vShader(5);
}

} quad;

}

namespace vs{

struct { 

struct 
{
int gX;
int gY;
} params;

void set () {
context->UpdateSubresource(dx11::Shaders::VS[6].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[6].params);
Shaders::vShader(6);
}

} simpleCube;

}

namespace ps{

struct { 

struct 
{
float hilight;
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
context->UpdateSubresource(dx11::Shaders::PS[0].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[0].params);
Shaders::pShader(0);
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
float _aspect;
float rad;
float r;
float g;
float b;
float a;
float soft;
float edge;
float outlineBrightness;
float progress;
float signed_progress;
float slider_type;
} params;

void set () {
context->UpdateSubresource(dx11::Shaders::PS[1].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[1].params);
Shaders::pShader(1);
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
context->UpdateSubresource(dx11::Shaders::PS[2].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[2].params);
Shaders::pShader(2);
}

} cubemapCreator;

}

namespace ps{

struct { 

struct {} params;

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

struct {} params;

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
context->UpdateSubresource(dx11::Shaders::PS[5].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[5].params);
Shaders::pShader(5);
Textures::TextureToShader((texture)textures.tex, 0, targetshader::pixel); 
Sampler::Sampler(targetshader::pixel, 0, samplers.s1Filter, samplers.s1AddressU, samplers.s1AddressV); 
}

} letter_ps;

}

namespace ps{

struct { 

struct 
{
float4 color;
} params;

void set () {
context->UpdateSubresource(dx11::Shaders::PS[6].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[6].params);
Shaders::pShader(6);
}

} lineDrawerUV_ps;

}

namespace ps{

struct { 

struct 
{
float4 color;
} params;

void set () {
context->UpdateSubresource(dx11::Shaders::PS[7].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[7].params);
Shaders::pShader(7);
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
context->UpdateSubresource(dx11::Shaders::PS[8].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[8].params);
Shaders::pShader(8);
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
context->UpdateSubresource(dx11::Shaders::PS[9].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[9].params);
Shaders::pShader(9);
}

} simpleFx;

}

