struct { 

struct 
{
float tone;
float x1;
float x2;
} params;

void set () {
Shaders::SetVS(0);
memcpy((char*)ConstBuf::drawerV,&params,sizeof(params));
}

} meshOut;

struct { 

void set () {
Shaders::SetVS(1);
}

} quad;

struct { 

struct 
{
float r;
float g;
float b;
} params;

void set () {
Shaders::SetPS(0);
memcpy((char*)ConstBuf::drawerP,&params,sizeof(params));
}

} simpleFx;

struct { 

struct 
{
float mix;
} params;

struct 
{
int tex1;
int tex2;
} textures;

struct 
{
int sam1Filter;
int sam1AddressU;
int sam1AddressV;
} samplers;

void set () {
Shaders::SetPS(1);
memcpy((char*)ConstBuf::drawerP,&params,sizeof(params));
Textures::SetTexture(textures.tex1, 0, Textures::tAssignType::pixel); 
Textures::SetTexture(textures.tex2, 1, Textures::tAssignType::pixel); 
Sampler::Set(Sampler::to::pixel, 0, samplers.sam1Filter, samplers.sam1AddressU, samplers.sam1AddressV); 
}

} simpleTex;

