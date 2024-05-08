struct { 

void CalcCubemap( const char* srcFileName, int srcLine, texture_ target)
{

if (cmdParamDesc[cmdCounter].loaded) {
	target = (texture_)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)target;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "texture_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "target"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Cubemap::CalcCubemap(target);
}

#define CalcCubemap(target) api.CalcCubemap( __FILE__, __LINE__ , target)

void ShowCubemap( const char* srcFileName, int srcLine, texture_ envTex)
{

if (cmdParamDesc[cmdCounter].loaded) {
	envTex = (texture_)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)envTex;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "texture_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "envTex"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Cubemap::ShowCubemap(envTex);
}

#define ShowCubemap(envTex) api.ShowCubemap( __FILE__, __LINE__ , envTex)

void ShowObject( const char* srcFileName, int srcLine, texture_ geometry, texture_ normals, int quality, position_ pos)
{

if (cmdParamDesc[cmdCounter].loaded) {
	geometry = (texture_)cmdParamDesc[cmdCounter].params[0][0];
	normals = (texture_)cmdParamDesc[cmdCounter].params[1][0];
	quality = (int)cmdParamDesc[cmdCounter].params[2][0];
	pos.x = cmdParamDesc[cmdCounter].params[3][0];
	pos.y = cmdParamDesc[cmdCounter].params[3][1];
	pos.z = cmdParamDesc[cmdCounter].params[3][2];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 4;
	cmdParamDesc[cmdCounter].params[0][0] = (float)geometry;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "texture_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "geometry"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)normals;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "texture_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "normals"); 
	cmdParamDesc[cmdCounter].params[2][0] = (float)quality;
	strcpy(cmdParamDesc[cmdCounter].paramType[2], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[2], "quality"); 
	cmdParamDesc[cmdCounter].params[3][0] = pos.x;
	cmdParamDesc[cmdCounter].params[3][1] = pos.y;
	cmdParamDesc[cmdCounter].params[3][2] = pos.z;
	strcpy(cmdParamDesc[cmdCounter].paramType[3], "position_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[3], "pos"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Object::ShowObject(geometry,normals,quality,pos);
}

#define ShowObject(geometry, normals, quality, pos_x, pos_y, pos_z) api.ShowObject( __FILE__, __LINE__ , geometry, normals, quality, position_ {pos_x, pos_y, pos_z })

void CalcObject( const char* srcFileName, int srcLine, texture_ targetGeoTexture, texture_ targetNrmlTexture)
{

if (cmdParamDesc[cmdCounter].loaded) {
	targetGeoTexture = (texture_)cmdParamDesc[cmdCounter].params[0][0];
	targetNrmlTexture = (texture_)cmdParamDesc[cmdCounter].params[1][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].params[0][0] = (float)targetGeoTexture;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "texture_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "targetGeoTexture"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)targetNrmlTexture;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "texture_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "targetNrmlTexture"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Object::CalcObject(targetGeoTexture,targetNrmlTexture);
}

#define CalcObject(targetGeoTexture, targetNrmlTexture) api.CalcObject( __FILE__, __LINE__ , targetGeoTexture, targetNrmlTexture)

void Blending( const char* srcFileName, int srcLine, blendmode_ m = blendmode::off, blendop_ blend = blendop::add)
{

if (cmdParamDesc[cmdCounter].loaded) {
	m = (blendmode_)cmdParamDesc[cmdCounter].params[0][0];
	blend = (blendop_)cmdParamDesc[cmdCounter].params[1][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].params[0][0] = (float)m;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "blendmode_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "m"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)blend;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "blendop_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "blend"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Blend::Blending(m,blend);
}

#define Blending(m, blend) api.Blending( __FILE__, __LINE__ , m, blend)

void Camera( const char* srcFileName, int srcLine, position_ eye, position_ at, position_ up, float angle)
{

if (cmdParamDesc[cmdCounter].loaded) {
	eye.x = cmdParamDesc[cmdCounter].params[0][0];
	eye.y = cmdParamDesc[cmdCounter].params[0][1];
	eye.z = cmdParamDesc[cmdCounter].params[0][2];
	at.x = cmdParamDesc[cmdCounter].params[1][0];
	at.y = cmdParamDesc[cmdCounter].params[1][1];
	at.z = cmdParamDesc[cmdCounter].params[1][2];
	up.x = cmdParamDesc[cmdCounter].params[2][0];
	up.y = cmdParamDesc[cmdCounter].params[2][1];
	up.z = cmdParamDesc[cmdCounter].params[2][2];
	angle = (float)cmdParamDesc[cmdCounter].params[3][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 4;
	cmdParamDesc[cmdCounter].params[0][0] = eye.x;
	cmdParamDesc[cmdCounter].params[0][1] = eye.y;
	cmdParamDesc[cmdCounter].params[0][2] = eye.z;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "position_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "eye"); 
	cmdParamDesc[cmdCounter].params[1][0] = at.x;
	cmdParamDesc[cmdCounter].params[1][1] = at.y;
	cmdParamDesc[cmdCounter].params[1][2] = at.z;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "position_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "at"); 
	cmdParamDesc[cmdCounter].params[2][0] = up.x;
	cmdParamDesc[cmdCounter].params[2][1] = up.y;
	cmdParamDesc[cmdCounter].params[2][2] = up.z;
	strcpy(cmdParamDesc[cmdCounter].paramType[2], "position_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[2], "up"); 
	cmdParamDesc[cmdCounter].params[3][0] = (float)angle;
	strcpy(cmdParamDesc[cmdCounter].paramType[3], "float"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[3], "angle"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Camera::Camera(eye,at,up,angle);
}

#define Camera(eye_x, eye_y, eye_z, at_x, at_y, at_z, up_x, up_y, up_z, angle) api.Camera( __FILE__, __LINE__ , position_ {eye_x, eye_y, eye_z }, position_ {at_x, at_y, at_z }, position_ {up_x, up_y, up_z }, angle)

void Depth( const char* srcFileName, int srcLine, depthmode_ m)
{

if (cmdParamDesc[cmdCounter].loaded) {
	m = (depthmode_)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)m;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "depthmode_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "m"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Depth::Depth(m);
}

#define Depth(m) api.Depth( __FILE__, __LINE__ , m)

void Clear( const char* srcFileName, int srcLine, color4_ c)
{

if (cmdParamDesc[cmdCounter].loaded) {
	c.r = cmdParamDesc[cmdCounter].params[0][0];
	c.g = cmdParamDesc[cmdCounter].params[0][1];
	c.b = cmdParamDesc[cmdCounter].params[0][2];
	c.a = cmdParamDesc[cmdCounter].params[0][3];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = c.r;
	cmdParamDesc[cmdCounter].params[0][1] = c.g;
	cmdParamDesc[cmdCounter].params[0][2] = c.b;
	cmdParamDesc[cmdCounter].params[0][3] = c.a;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "color4_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "c"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Draw::Clear(c);
}

#define Clear(c_r, c_g, c_b, c_a) api.Clear( __FILE__, __LINE__ , color4_ {c_r, c_g, c_b, c_a })

void ClearDepth( const char* srcFileName, int srcLine)
{

AddToUI(__FUNCTION__);
cmdCounter++;

Draw::ClearDepth();
}

#define ClearDepth() api.ClearDepth( __FILE__, __LINE__ )

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

#define NullDrawer(quadCount, instances) api.NullDrawer( __FILE__, __LINE__ , quadCount, instances)

void IA( const char* srcFileName, int srcLine, topology_ t)
{

if (cmdParamDesc[cmdCounter].loaded) {
	t = (topology_)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)t;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "topology_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "t"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

InputAssembler::IA(t);
}

#define IA(t) api.IA( __FILE__, __LINE__ , t)

void Cull( const char* srcFileName, int srcLine, cullmode_ i)
{

if (cmdParamDesc[cmdCounter].loaded) {
	i = (cullmode_)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)i;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "cullmode_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "i"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Rasterizer::Cull(i);
}

#define Cull(i) api.Cull( __FILE__, __LINE__ , i)

void Scissors( const char* srcFileName, int srcLine, rect_ r)
{

if (cmdParamDesc[cmdCounter].loaded) {
	r.x = cmdParamDesc[cmdCounter].params[0][0];
	r.y = cmdParamDesc[cmdCounter].params[0][1];
	r.x1 = cmdParamDesc[cmdCounter].params[0][2];
	r.y1 = cmdParamDesc[cmdCounter].params[0][3];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = r.x;
	cmdParamDesc[cmdCounter].params[0][1] = r.y;
	cmdParamDesc[cmdCounter].params[0][2] = r.x1;
	cmdParamDesc[cmdCounter].params[0][3] = r.y1;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "rect_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "r"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Rasterizer::Scissors(r);
}

#define Scissors(r_x, r_y, r_x1r_y1) api.Scissors( __FILE__, __LINE__ , rect_ {r_x, r_y, r_x1, r_y1 })

void Sampler( const char* srcFileName, int srcLine, targetshader_ shader, int slot, filter_ filter, addr_ addressU, addr_ addressV)
{

if (cmdParamDesc[cmdCounter].loaded) {
	shader = (targetshader_)cmdParamDesc[cmdCounter].params[0][0];
	slot = (int)cmdParamDesc[cmdCounter].params[1][0];
	filter = (filter_)cmdParamDesc[cmdCounter].params[2][0];
	addressU = (addr_)cmdParamDesc[cmdCounter].params[3][0];
	addressV = (addr_)cmdParamDesc[cmdCounter].params[4][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 5;
	cmdParamDesc[cmdCounter].params[0][0] = (float)shader;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "targetshader_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "shader"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)slot;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "slot"); 
	cmdParamDesc[cmdCounter].params[2][0] = (float)filter;
	strcpy(cmdParamDesc[cmdCounter].paramType[2], "filter_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[2], "filter"); 
	cmdParamDesc[cmdCounter].params[3][0] = (float)addressU;
	strcpy(cmdParamDesc[cmdCounter].paramType[3], "addr_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[3], "addressU"); 
	cmdParamDesc[cmdCounter].params[4][0] = (float)addressV;
	strcpy(cmdParamDesc[cmdCounter].paramType[4], "addr_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[4], "addressV"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Sampler::Sampler(shader,slot,filter,addressU,addressV);
}

#define Sampler(shader, slot, filter, addressU, addressV) api.Sampler( __FILE__, __LINE__ , shader, slot, filter, addressU, addressV)

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

#define SamplerComp(slot) api.SamplerComp( __FILE__, __LINE__ , slot)

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

#define vShader(n) api.vShader( __FILE__, __LINE__ , n)

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

#define pShader(n) api.pShader( __FILE__, __LINE__ , n)

void CopyColor( const char* srcFileName, int srcLine, texture_ dst, texture_ src)
{

if (cmdParamDesc[cmdCounter].loaded) {
	dst = (texture_)cmdParamDesc[cmdCounter].params[0][0];
	src = (texture_)cmdParamDesc[cmdCounter].params[1][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].params[0][0] = (float)dst;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "texture_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "dst"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)src;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "texture_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "src"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Textures::CopyColor(dst,src);
}

#define CopyColor(dst, src) api.CopyColor( __FILE__, __LINE__ , dst, src)

void CopyDepth( const char* srcFileName, int srcLine, texture_ dst, texture_ src)
{

if (cmdParamDesc[cmdCounter].loaded) {
	dst = (texture_)cmdParamDesc[cmdCounter].params[0][0];
	src = (texture_)cmdParamDesc[cmdCounter].params[1][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].params[0][0] = (float)dst;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "texture_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "dst"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)src;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "texture_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "src"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Textures::CopyDepth(dst,src);
}

#define CopyDepth(dst, src) api.CopyDepth( __FILE__, __LINE__ , dst, src)

void TextureToShader( const char* srcFileName, int srcLine, texture_ tex, int slot, targetshader_ tA = targetshader::both)
{

if (cmdParamDesc[cmdCounter].loaded) {
	tex = (texture_)cmdParamDesc[cmdCounter].params[0][0];
	slot = (int)cmdParamDesc[cmdCounter].params[1][0];
	tA = (targetshader_)cmdParamDesc[cmdCounter].params[2][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 3;
	cmdParamDesc[cmdCounter].params[0][0] = (float)tex;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "texture_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "tex"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)slot;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "slot"); 
	cmdParamDesc[cmdCounter].params[2][0] = (float)tA;
	strcpy(cmdParamDesc[cmdCounter].paramType[2], "targetshader_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[2], "tA"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Textures::TextureToShader(tex,slot,tA);
}

#define TextureToShader(tex, slot, tA) api.TextureToShader( __FILE__, __LINE__ , tex, slot, tA)

void CreateMipMap( const char* srcFileName, int srcLine)
{

AddToUI(__FUNCTION__);
cmdCounter++;

Textures::CreateMipMap();
}

#define CreateMipMap() api.CreateMipMap( __FILE__, __LINE__ )

void RenderTarget( const char* srcFileName, int srcLine, texture_ texId = mainRTIndex, int level = 0)
{

if (cmdParamDesc[cmdCounter].loaded) {
	texId = (texture_)cmdParamDesc[cmdCounter].params[0][0];
	level = (int)cmdParamDesc[cmdCounter].params[1][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].params[0][0] = (float)texId;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "texture_"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "texId"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)level;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "level"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Textures::RenderTarget(texId,level);
}

#define RenderTarget(texId, level) api.RenderTarget( __FILE__, __LINE__ , texId, level)

} api;
