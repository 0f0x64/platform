struct { 

void CalcCubemap( const char* srcFileName, int srcLine, int texture)
{

if (cmdParamDesc[cmdCounter].loaded) {
	texture = (int)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)texture;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "texture"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Cubemap::CalcCubemap(texture);
}

void ShowCubemap( const char* srcFileName, int srcLine, unsigned int envTex)
{

if (cmdParamDesc[cmdCounter].loaded) {
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "unsigned int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "envTex"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Cubemap::ShowCubemap(envTex);
}

void ShowObject( const char* srcFileName, int srcLine, int geometry, int normals, int quality, position pos)
{

if (cmdParamDesc[cmdCounter].loaded) {
	geometry = (int)cmdParamDesc[cmdCounter].params[0][0];
	normals = (int)cmdParamDesc[cmdCounter].params[1][0];
	quality = (int)cmdParamDesc[cmdCounter].params[2][0];
	pos.x = cmdParamDesc[cmdCounter].params[3][0];
	pos.y = cmdParamDesc[cmdCounter].params[3][1];
	pos.z = cmdParamDesc[cmdCounter].params[3][2];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 4;
	cmdParamDesc[cmdCounter].params[0][0] = (float)geometry;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "geometry"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)normals;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "normals"); 
	cmdParamDesc[cmdCounter].params[2][0] = (float)quality;
	strcpy(cmdParamDesc[cmdCounter].paramType[2], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[2], "quality"); 
	cmdParamDesc[cmdCounter].params[3][0] = pos.x;
	cmdParamDesc[cmdCounter].params[3][1] = pos.y;
	cmdParamDesc[cmdCounter].params[3][2] = pos.z;
	strcpy(cmdParamDesc[cmdCounter].paramType[3], "position"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[3], "pos"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Object::ShowObject(geometry,normals,quality,pos);
}

void CalcObject( const char* srcFileName, int srcLine, int targetGeoTexture, int targetNrmlTexture)
{

if (cmdParamDesc[cmdCounter].loaded) {
	targetGeoTexture = (int)cmdParamDesc[cmdCounter].params[0][0];
	targetNrmlTexture = (int)cmdParamDesc[cmdCounter].params[1][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].params[0][0] = (float)targetGeoTexture;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "targetGeoTexture"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)targetNrmlTexture;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "targetNrmlTexture"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Object::CalcObject(targetGeoTexture,targetNrmlTexture);
}

void Blending( const char* srcFileName, int srcLine, int m = blendmode::off, int blend = blendop::add)
{

if (cmdParamDesc[cmdCounter].loaded) {
	m = (int)cmdParamDesc[cmdCounter].params[0][0];
	blend = (int)cmdParamDesc[cmdCounter].params[1][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].params[0][0] = (float)m;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "m"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)blend;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "blend"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Blend::Blending(m,blend);
}

void Camera( const char* srcFileName, int srcLine, camData* cam)
{

if (cmdParamDesc[cmdCounter].loaded) {
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "camData*"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "cam"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Camera::Camera(cam);
}

void ConstToVertex( const char* srcFileName, int srcLine, int i)
{

if (cmdParamDesc[cmdCounter].loaded) {
	i = (int)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)i;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "i"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

ConstBuf::ConstToVertex(i);
}

void ConstToPixel( const char* srcFileName, int srcLine, int i)
{

if (cmdParamDesc[cmdCounter].loaded) {
	i = (int)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)i;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "i"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

ConstBuf::ConstToPixel(i);
}

void Depth( const char* srcFileName, int srcLine, int m)
{

if (cmdParamDesc[cmdCounter].loaded) {
	m = (int)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)m;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "m"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Depth::Depth(m);
}

void Clear( const char* srcFileName, int srcLine, float r, float g, float b, float a)
{

if (cmdParamDesc[cmdCounter].loaded) {
	r = (float)cmdParamDesc[cmdCounter].params[0][0];
	g = (float)cmdParamDesc[cmdCounter].params[1][0];
	b = (float)cmdParamDesc[cmdCounter].params[2][0];
	a = (float)cmdParamDesc[cmdCounter].params[3][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 4;
	cmdParamDesc[cmdCounter].params[0][0] = (float)r;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "float"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "r"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)g;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "float"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "g"); 
	cmdParamDesc[cmdCounter].params[2][0] = (float)b;
	strcpy(cmdParamDesc[cmdCounter].paramType[2], "float"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[2], "b"); 
	cmdParamDesc[cmdCounter].params[3][0] = (float)a;
	strcpy(cmdParamDesc[cmdCounter].paramType[3], "float"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[3], "a"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Draw::Clear(r,g,b,a);
}

void ClearDepth( const char* srcFileName, int srcLine)
{

AddToUI(__FUNCTION__);
cmdCounter++;

Draw::ClearDepth();
}

void NullDrawer( const char* srcFileName, int srcLine, int quadCount, int instances = 1)
{

if (cmdParamDesc[cmdCounter].loaded) {
	quadCount = (int)cmdParamDesc[cmdCounter].params[0][0];
	instances = (int)cmdParamDesc[cmdCounter].params[1][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].params[0][0] = (float)quadCount;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "quadCount"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)instances;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "instances"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Draw::NullDrawer(quadCount,instances);
}

void IA( const char* srcFileName, int srcLine, int topology)
{

if (cmdParamDesc[cmdCounter].loaded) {
	topology = (int)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)topology;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "topology"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

InputAssembler::IA(topology);
}

void Cull( const char* srcFileName, int srcLine, int i)
{

if (cmdParamDesc[cmdCounter].loaded) {
	i = (int)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)i;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "i"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Rasterizer::Cull(i);
}

void Scissors( const char* srcFileName, int srcLine, float left, float top, float right, float bottom)
{

if (cmdParamDesc[cmdCounter].loaded) {
	left = (float)cmdParamDesc[cmdCounter].params[0][0];
	top = (float)cmdParamDesc[cmdCounter].params[1][0];
	right = (float)cmdParamDesc[cmdCounter].params[2][0];
	bottom = (float)cmdParamDesc[cmdCounter].params[3][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 4;
	cmdParamDesc[cmdCounter].params[0][0] = (float)left;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "float"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "left"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)top;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "float"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "top"); 
	cmdParamDesc[cmdCounter].params[2][0] = (float)right;
	strcpy(cmdParamDesc[cmdCounter].paramType[2], "float"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[2], "right"); 
	cmdParamDesc[cmdCounter].params[3][0] = (float)bottom;
	strcpy(cmdParamDesc[cmdCounter].paramType[3], "float"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[3], "bottom"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Rasterizer::Scissors(left,top,right,bottom);
}

void Sampler( const char* srcFileName, int srcLine, targetShader shader, int slot, int filter, int addressU, int addressV)
{

if (cmdParamDesc[cmdCounter].loaded) {
	slot = (int)cmdParamDesc[cmdCounter].params[1][0];
	filter = (int)cmdParamDesc[cmdCounter].params[2][0];
	addressU = (int)cmdParamDesc[cmdCounter].params[3][0];
	addressV = (int)cmdParamDesc[cmdCounter].params[4][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 5;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "targetShader"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "shader"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)slot;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "slot"); 
	cmdParamDesc[cmdCounter].params[2][0] = (float)filter;
	strcpy(cmdParamDesc[cmdCounter].paramType[2], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[2], "filter"); 
	cmdParamDesc[cmdCounter].params[3][0] = (float)addressU;
	strcpy(cmdParamDesc[cmdCounter].paramType[3], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[3], "addressU"); 
	cmdParamDesc[cmdCounter].params[4][0] = (float)addressV;
	strcpy(cmdParamDesc[cmdCounter].paramType[4], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[4], "addressV"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Sampler::Sampler(shader,slot,filter,addressU,addressV);
}

void SamplerComp( const char* srcFileName, int srcLine, int slot)
{

if (cmdParamDesc[cmdCounter].loaded) {
	slot = (int)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)slot;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "slot"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Sampler::SamplerComp(slot);
}

void vShader( const char* srcFileName, int srcLine, int n)
{

if (cmdParamDesc[cmdCounter].loaded) {
	n = (int)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)n;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "n"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Shaders::vShader(n);
}

void pShader( const char* srcFileName, int srcLine, int n)
{

if (cmdParamDesc[cmdCounter].loaded) {
	n = (int)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)n;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "n"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Shaders::pShader(n);
}

void CopyColor( const char* srcFileName, int srcLine, int dst, int src)
{

if (cmdParamDesc[cmdCounter].loaded) {
	dst = (int)cmdParamDesc[cmdCounter].params[0][0];
	src = (int)cmdParamDesc[cmdCounter].params[1][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].params[0][0] = (float)dst;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "dst"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)src;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "src"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Textures::CopyColor(dst,src);
}

void CopyDepth( const char* srcFileName, int srcLine, int dst, int src)
{

if (cmdParamDesc[cmdCounter].loaded) {
	dst = (int)cmdParamDesc[cmdCounter].params[0][0];
	src = (int)cmdParamDesc[cmdCounter].params[1][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].params[0][0] = (float)dst;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "dst"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)src;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "src"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Textures::CopyDepth(dst,src);
}

void TextureToShader( const char* srcFileName, int srcLine, int tex, int slot, targetShader tA = targetShader::both)
{

if (cmdParamDesc[cmdCounter].loaded) {
	tex = (int)cmdParamDesc[cmdCounter].params[0][0];
	slot = (int)cmdParamDesc[cmdCounter].params[1][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 3;
	cmdParamDesc[cmdCounter].params[0][0] = (float)tex;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "tex"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)slot;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "slot"); 
	strcpy(cmdParamDesc[cmdCounter].paramType[2], "targetShader"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[2], "tA"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Textures::TextureToShader(tex,slot,tA);
}

void CreateMipMap( const char* srcFileName, int srcLine)
{

AddToUI(__FUNCTION__);
cmdCounter++;

Textures::CreateMipMap();
}

void RenderTarget( const char* srcFileName, int srcLine, byte texId = mainRTIndex, byte level = 0)
{

if (cmdParamDesc[cmdCounter].loaded) {
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "byte"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "texId"); 
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "byte"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "level"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Textures::RenderTarget(texId,level);
}

} api;
