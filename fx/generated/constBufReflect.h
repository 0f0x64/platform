namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
float4 base_color;
} params;

void set () {
Shaders::vShader(0);
context->UpdateSubresource(dx11::Shaders::VS[0].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[0].params);
}

} blob;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
float4 base_color;
} params;

void set () {
Shaders::vShader(1);
context->UpdateSubresource(dx11::Shaders::VS[1].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[1].params);
}

} capriStar;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
} params;

void set () {
Shaders::vShader(2);
context->UpdateSubresource(dx11::Shaders::VS[2].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[2].params);
}

} fish;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
float4 base_color;
} params;

void set () {
Shaders::vShader(3);
context->UpdateSubresource(dx11::Shaders::VS[3].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[3].params);
}

} galaxy;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
float4 base_color;
} params;

void set () {
Shaders::vShader(4);
context->UpdateSubresource(dx11::Shaders::VS[4].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[4].params);
}

} galaxy_2;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
float4 base_color;
float4 modelPos;
float4 triCount;
float4 brightness;
float4 tickness;
float4 zoom;
float onLineOfs;
float jumpCharge;
} params;

void set () {
Shaders::vShader(5);
context->UpdateSubresource(dx11::Shaders::VS[5].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[5].params);
}

} girl;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
float4 base_color;
} params;

void set () {
Shaders::vShader(6);
context->UpdateSubresource(dx11::Shaders::VS[6].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[6].params);
}

} insideNebula;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
float4 base_color;
} params;

void set () {
Shaders::vShader(7);
context->UpdateSubresource(dx11::Shaders::VS[7].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[7].params);
}

} islands;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
float4 base_color;
} params;

void set () {
Shaders::vShader(8);
context->UpdateSubresource(dx11::Shaders::VS[8].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[8].params);
}

} leo;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
} params;

void set () {
Shaders::vShader(9);
context->UpdateSubresource(dx11::Shaders::VS[9].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[9].params);
}

} libra_sph;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
float4 heroPosition;
float4 base_color;
float4 basePoint[3500];
int particlesCount;
int basePointsCount;
} params;

void set () {
Shaders::vShader(10);
context->UpdateSubresource(dx11::Shaders::VS[10].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[10].params);
}

} maze;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
float4 base_color;
} params;

void set () {
Shaders::vShader(11);
context->UpdateSubresource(dx11::Shaders::VS[11].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[11].params);
}

} Nebula2;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
} params;

void set () {
Shaders::vShader(12);
context->UpdateSubresource(dx11::Shaders::VS[12].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[12].params);
}

} neitron_star;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
float glow_p;
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
Shaders::vShader(13);
context->UpdateSubresource(dx11::Shaders::VS[13].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[13].params);
Textures::TextureToShader((texture)textures.positions, 0, targetshader::vertex); 
Textures::TextureToShader((texture)textures.normals, 1, targetshader::vertex); 
Sampler::Sampler(targetshader::vertex, 0, samplers.sam1Filter, samplers.sam1AddressU, samplers.sam1AddressV); 
}

} objViewer;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
float4 base_color;
} params;

void set () {
Shaders::vShader(14);
context->UpdateSubresource(dx11::Shaders::VS[14].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[14].params);
}

} pearl;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
} params;

void set () {
Shaders::vShader(15);
context->UpdateSubresource(dx11::Shaders::VS[15].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[15].params);
}

} pillars;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
} params;

void set () {
Shaders::vShader(16);
context->UpdateSubresource(dx11::Shaders::VS[16].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[16].params);
}

} pillarsHand;

}

namespace vs{

struct { 

struct {} params;

void set () {
Shaders::vShader(17);
}

} quad;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
float4 base_color;
} params;

void set () {
Shaders::vShader(18);
context->UpdateSubresource(dx11::Shaders::VS[18].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[18].params);
}

} rocks;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
float4 base_color;
} params;

void set () {
Shaders::vShader(19);
context->UpdateSubresource(dx11::Shaders::VS[19].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[19].params);
}

} scorpBall;

}

namespace vs{

struct { 

struct 
{
int gX;
int gY;
} params;

void set () {
Shaders::vShader(20);
context->UpdateSubresource(dx11::Shaders::VS[20].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[20].params);
}

} simpleCube;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
} params;

void set () {
Shaders::vShader(21);
context->UpdateSubresource(dx11::Shaders::VS[21].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[21].params);
}

} space;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
float4 base_color;
float4 mesh[40];
} params;

void set () {
Shaders::vShader(22);
context->UpdateSubresource(dx11::Shaders::VS[22].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[22].params);
}

} Tau;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
float4 base_color;
} params;

void set () {
Shaders::vShader(23);
context->UpdateSubresource(dx11::Shaders::VS[23].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[23].params);
}

} transporter;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
} params;

void set () {
Shaders::vShader(24);
context->UpdateSubresource(dx11::Shaders::VS[24].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[24].params);
}

} tree;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
float4 base_color;
float4 eye;
float4 forward;
float4 up;
float4 right;
} params;

void set () {
Shaders::vShader(25);
context->UpdateSubresource(dx11::Shaders::VS[25].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[25].params);
}

} Virgo;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
int gX;
int gY;
int mode;
int skipper;
float4 base_color;
} params;

void set () {
Shaders::vShader(26);
context->UpdateSubresource(dx11::Shaders::VS[26].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[26].params);
}

} waterfall;

}

namespace vs{

struct { 

struct 
{
float4 pos_size[256];
} params;

void set () {
Shaders::vShader(27);
context->UpdateSubresource(dx11::Shaders::VS[27].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[27].params);
}

} box;

}

namespace vs{

struct { 

struct 
{
float width;
float height;
float2 padding;
float4 pos_size[256];
} params;

void set () {
Shaders::vShader(28);
context->UpdateSubresource(dx11::Shaders::VS[28].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[28].params);
}

} letter;

}

namespace vs{

struct { 

struct 
{
float4 position [4000];
} params;

void set () {
Shaders::vShader(29);
context->UpdateSubresource(dx11::Shaders::VS[29].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[29].params);
}

} lineDrawer;

}

namespace vs{

struct { 

struct 
{
float4x4 model;
float4 position [4000];
} params;

void set () {
Shaders::vShader(30);
context->UpdateSubresource(dx11::Shaders::VS[30].params, 0, NULL, &params, 0, 0);
context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[30].params);
}

} lineDrawer3d;

}

namespace ps{

struct { 

struct 
{
float hilight;
} params;

void set () {
Shaders::pShader(0);
context->UpdateSubresource(dx11::Shaders::PS[0].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[0].params);
}

} basic;

}

namespace ps{

struct { 

struct 
{
float hilight;
} params;

void set () {
Shaders::pShader(1);
context->UpdateSubresource(dx11::Shaders::PS[1].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[1].params);
}

} basicLow;

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
Shaders::pShader(2);
context->UpdateSubresource(dx11::Shaders::PS[2].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[2].params);
}

} cat;

}

namespace ps{

struct { 

struct {} params;

void set () {
Shaders::pShader(3);
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
Shaders::pShader(4);
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
Shaders::pShader(5);
Textures::TextureToShader((texture)textures.geo, 0, targetshader::pixel); 
Sampler::Sampler(targetshader::pixel, 0, samplers.sam1Filter, samplers.sam1AddressU, samplers.sam1AddressV); 
}

} genNormals;

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
Shaders::pShader(6);
context->UpdateSubresource(dx11::Shaders::PS[6].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[6].params);
}

} obj1;

}

namespace ps{

struct { 

struct 
{
float hilight;
} params;

struct 
{
texture screen;
texture screenMid;
texture screenLow;
} textures;

struct 
{
filter sam1Filter;
addr sam1AddressU;
addr sam1AddressV;
} samplers;

void set () {
Shaders::pShader(7);
context->UpdateSubresource(dx11::Shaders::PS[7].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[7].params);
Textures::TextureToShader((texture)textures.screen, 0, targetshader::pixel); 
Textures::TextureToShader((texture)textures.screenMid, 1, targetshader::pixel); 
Textures::TextureToShader((texture)textures.screenLow, 2, targetshader::pixel); 
Sampler::Sampler(targetshader::pixel, 0, samplers.sam1Filter, samplers.sam1AddressU, samplers.sam1AddressV); 
}

} output;

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
Shaders::pShader(8);
context->UpdateSubresource(dx11::Shaders::PS[8].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[8].params);
}

} box_ps;

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
Shaders::pShader(9);
context->UpdateSubresource(dx11::Shaders::PS[9].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[9].params);
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
Shaders::pShader(10);
context->UpdateSubresource(dx11::Shaders::PS[10].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[10].params);
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
Shaders::pShader(11);
context->UpdateSubresource(dx11::Shaders::PS[11].params, 0, NULL, &params, 0, 0);
context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[11].params);
}

} lineDrawer_ps;

}

