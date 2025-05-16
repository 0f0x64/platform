namespace vs{

struct { 

struct {} params;

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
context->UpdateSubresource(dx11::Shaders::VS[1].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[1].params);
memcpy((char*)ConstBuf::drawerV,&params,sizeof(params));
}

} letter;

}

namespace vs{

struct { 

struct {} params;

void set () {
Shaders::vShader(2);
}

} lineDrawer;

}

namespace vs{

struct { 

struct {} params;

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
context->UpdateSubresource(dx11::Shaders::VS[4].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[4].params);
memcpy((char*)ConstBuf::drawerV,&params,sizeof(params));
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
float gX;
float gY;
} params;

void set () {
Shaders::vShader(6);
context->UpdateSubresource(dx11::Shaders::VS[6].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[6].params);
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
context->UpdateSubresource(dx11::Shaders::PS[0].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(1, 1, &dx11::Shaders::VS[0].params);
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
Shaders::pShader(1);
context->UpdateSubresource(dx11::Shaders::PS[1].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(1, 1, &dx11::Shaders::VS[1].params);
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
context->UpdateSubresource(dx11::Shaders::PS[2].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(1, 1, &dx11::Shaders::VS[2].params);
memcpy((char*)ConstBuf::drawerP,&params,sizeof(params));
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
Shaders::pShader(5);
context->UpdateSubresource(dx11::Shaders::PS[5].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(1, 1, &dx11::Shaders::VS[5].params);
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
context->UpdateSubresource(dx11::Shaders::PS[6].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(1, 1, &dx11::Shaders::VS[6].params);
memcpy((char*)ConstBuf::drawerP,&params,sizeof(params));
}

} lineDrawerUV_ps;

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
Shaders::pShader(7);
context->UpdateSubresource(dx11::Shaders::PS[7].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(1, 1, &dx11::Shaders::VS[7].params);
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
Shaders::pShader(8);
context->UpdateSubresource(dx11::Shaders::PS[8].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(1, 1, &dx11::Shaders::VS[8].params);
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
Shaders::pShader(9);
context->UpdateSubresource(dx11::Shaders::PS[9].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(1, 1, &dx11::Shaders::VS[9].params);
memcpy((char*)ConstBuf::drawerP,&params,sizeof(params));
}

} simpleFx;

}

