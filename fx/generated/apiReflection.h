struct { 

void CalcCubemap( const char* srcFileName, int srcLine, texture target)
{

if (cmdParamDesc[cmdCounter].loaded) {
	target = (texture)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)target;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "texture"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "target"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Cubemap::CalcCubemap(target);
}

#define CalcCubemap(target) api.CalcCubemap( __FILE__, __LINE__ , target)

void ShowCubemap( const char* srcFileName, int srcLine, texture envTexture)
{

if (cmdParamDesc[cmdCounter].loaded) {
	envTexture = (texture)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)envTexture;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "texture"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "envTexture"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Cubemap::ShowCubemap(envTexture);
}

#define ShowCubemap(envTexture) api.ShowCubemap( __FILE__, __LINE__ , envTexture)

void ShowObject( const char* srcFileName, int srcLine, texture geometry, texture normals, int quality, position pos)
{

if (cmdParamDesc[cmdCounter].loaded) {
	geometry = (texture)cmdParamDesc[cmdCounter].params[0][0];
	normals = (texture)cmdParamDesc[cmdCounter].params[1][0];
	quality = (int)cmdParamDesc[cmdCounter].params[2][0];
	pos.x = cmdParamDesc[cmdCounter].params[3][0];
	pos.y = cmdParamDesc[cmdCounter].params[3][1];
	pos.z = cmdParamDesc[cmdCounter].params[3][2];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 4;
	cmdParamDesc[cmdCounter].params[0][0] = (float)geometry;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "texture"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "geometry"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)normals;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "texture"); 
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

#define ShowObject(geometry, normals, quality, pos_x, pos_y, pos_z) api.ShowObject( __FILE__, __LINE__ , geometry, normals, quality, position {pos_x, pos_y, pos_z })

void CalcObject( const char* srcFileName, int srcLine, texture targetGeo, texture targetNrml)
{

if (cmdParamDesc[cmdCounter].loaded) {
	targetGeo = (texture)cmdParamDesc[cmdCounter].params[0][0];
	targetNrml = (texture)cmdParamDesc[cmdCounter].params[1][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].params[0][0] = (float)targetGeo;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "texture"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "targetGeo"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)targetNrml;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "texture"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "targetNrml"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Object::CalcObject(targetGeo,targetNrml);
}

#define CalcObject(targetGeo, targetNrml) api.CalcObject( __FILE__, __LINE__ , targetGeo, targetNrml)

void Blending( const char* srcFileName, int srcLine, blendmode mode = blendmode::off, blendop operation = blendop::add)
{

if (cmdParamDesc[cmdCounter].loaded) {
	mode = (blendmode)cmdParamDesc[cmdCounter].params[0][0];
	operation = (blendop)cmdParamDesc[cmdCounter].params[1][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].params[0][0] = (float)mode;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "blendmode"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "mode"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)operation;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "blendop"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "operation"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Blend::Blending(mode,operation);
}

#define Blending(mode, operation) api.Blending( __FILE__, __LINE__ , mode, operation)

void Camera( const char* srcFileName, int srcLine, position eye, position at, position up, float angle)
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
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "position"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "eye"); 
	cmdParamDesc[cmdCounter].params[1][0] = at.x;
	cmdParamDesc[cmdCounter].params[1][1] = at.y;
	cmdParamDesc[cmdCounter].params[1][2] = at.z;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "position"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "at"); 
	cmdParamDesc[cmdCounter].params[2][0] = up.x;
	cmdParamDesc[cmdCounter].params[2][1] = up.y;
	cmdParamDesc[cmdCounter].params[2][2] = up.z;
	strcpy(cmdParamDesc[cmdCounter].paramType[2], "position"); 
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

#define Camera(eye_x, eye_y, eye_z, at_x, at_y, at_z, up_x, up_y, up_z, angle) api.Camera( __FILE__, __LINE__ , position {eye_x, eye_y, eye_z }, position {at_x, at_y, at_z }, position {up_x, up_y, up_z }, angle)

void Depth( const char* srcFileName, int srcLine, depthmode mode)
{

if (cmdParamDesc[cmdCounter].loaded) {
	mode = (depthmode)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)mode;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "depthmode"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "mode"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Depth::Depth(mode);
}

#define Depth(mode) api.Depth( __FILE__, __LINE__ , mode)

void Clear( const char* srcFileName, int srcLine, color4 color)
{

if (cmdParamDesc[cmdCounter].loaded) {
	color.x = cmdParamDesc[cmdCounter].params[0][0];
	color.y = cmdParamDesc[cmdCounter].params[0][1];
	color.z = cmdParamDesc[cmdCounter].params[0][2];
	color.w = cmdParamDesc[cmdCounter].params[0][3];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = color.x;
	cmdParamDesc[cmdCounter].params[0][1] = color.y;
	cmdParamDesc[cmdCounter].params[0][2] = color.z;
	cmdParamDesc[cmdCounter].params[0][3] = color.w;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "color4"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "color"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Draw::Clear(color);
}

#define Clear(color_x, color_y, color_z, color_w) api.Clear( __FILE__, __LINE__ , color4 {color_x, color_y, color_z, color_w })

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

void IA( const char* srcFileName, int srcLine, topology topoType)
{

if (cmdParamDesc[cmdCounter].loaded) {
	topoType = (topology)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)topoType;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "topology"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "topoType"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

InputAssembler::IA(topoType);
}

#define IA(topoType) api.IA( __FILE__, __LINE__ , topoType)

void Cull( const char* srcFileName, int srcLine, cullmode mode)
{

if (cmdParamDesc[cmdCounter].loaded) {
	mode = (cullmode)cmdParamDesc[cmdCounter].params[0][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = (float)mode;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "cullmode"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "mode"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Rasterizer::Cull(mode);
}

#define Cull(mode) api.Cull( __FILE__, __LINE__ , mode)

void Scissors( const char* srcFileName, int srcLine, rect r)
{

if (cmdParamDesc[cmdCounter].loaded) {
	r.x = cmdParamDesc[cmdCounter].params[0][0];
	r.y = cmdParamDesc[cmdCounter].params[0][1];
	r.z = cmdParamDesc[cmdCounter].params[0][2];
	r.w = cmdParamDesc[cmdCounter].params[0][3];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].params[0][0] = r.x;
	cmdParamDesc[cmdCounter].params[0][1] = r.y;
	cmdParamDesc[cmdCounter].params[0][2] = r.z;
	cmdParamDesc[cmdCounter].params[0][3] = r.w;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "rect"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "r"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Rasterizer::Scissors(r);
}

#define Scissors(r_x, r_y, r_z, r_w) api.Scissors( __FILE__, __LINE__ , rect {r_x, r_y, r_z, r_w })

void Sampler( const char* srcFileName, int srcLine, targetshader shader, int slot, filter filterType, addr addressU, addr addressV)
{

if (cmdParamDesc[cmdCounter].loaded) {
	shader = (targetshader)cmdParamDesc[cmdCounter].params[0][0];
	slot = (int)cmdParamDesc[cmdCounter].params[1][0];
	filterType = (filter)cmdParamDesc[cmdCounter].params[2][0];
	addressU = (addr)cmdParamDesc[cmdCounter].params[3][0];
	addressV = (addr)cmdParamDesc[cmdCounter].params[4][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 5;
	cmdParamDesc[cmdCounter].params[0][0] = (float)shader;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "targetshader"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "shader"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)slot;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "slot"); 
	cmdParamDesc[cmdCounter].params[2][0] = (float)filterType;
	strcpy(cmdParamDesc[cmdCounter].paramType[2], "filter"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[2], "filterType"); 
	cmdParamDesc[cmdCounter].params[3][0] = (float)addressU;
	strcpy(cmdParamDesc[cmdCounter].paramType[3], "addr"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[3], "addressU"); 
	cmdParamDesc[cmdCounter].params[4][0] = (float)addressV;
	strcpy(cmdParamDesc[cmdCounter].paramType[4], "addr"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[4], "addressV"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Sampler::Sampler(shader,slot,filterType,addressU,addressV);
}

#define Sampler(shader, slot, filterType, addressU, addressV) api.Sampler( __FILE__, __LINE__ , shader, slot, filterType, addressU, addressV)

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

void CopyColor( const char* srcFileName, int srcLine, texture dst, texture src)
{

if (cmdParamDesc[cmdCounter].loaded) {
	dst = (texture)cmdParamDesc[cmdCounter].params[0][0];
	src = (texture)cmdParamDesc[cmdCounter].params[1][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].params[0][0] = (float)dst;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "texture"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "dst"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)src;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "texture"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "src"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Textures::CopyColor(dst,src);
}

#define CopyColor(dst, src) api.CopyColor( __FILE__, __LINE__ , dst, src)

void CopyDepth( const char* srcFileName, int srcLine, texture dst, texture src)
{

if (cmdParamDesc[cmdCounter].loaded) {
	dst = (texture)cmdParamDesc[cmdCounter].params[0][0];
	src = (texture)cmdParamDesc[cmdCounter].params[1][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].params[0][0] = (float)dst;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "texture"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "dst"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)src;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "texture"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "src"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Textures::CopyDepth(dst,src);
}

#define CopyDepth(dst, src) api.CopyDepth( __FILE__, __LINE__ , dst, src)

void TextureToShader( const char* srcFileName, int srcLine, texture tex, int slot, targetshader tA = targetshader::both)
{

if (cmdParamDesc[cmdCounter].loaded) {
	tex = (texture)cmdParamDesc[cmdCounter].params[0][0];
	slot = (int)cmdParamDesc[cmdCounter].params[1][0];
	tA = (targetshader)cmdParamDesc[cmdCounter].params[2][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 3;
	cmdParamDesc[cmdCounter].params[0][0] = (float)tex;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "texture"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "tex"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)slot;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "slot"); 
	cmdParamDesc[cmdCounter].params[2][0] = (float)tA;
	strcpy(cmdParamDesc[cmdCounter].paramType[2], "targetshader"); 
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

void RenderTarget( const char* srcFileName, int srcLine, texture target, int level = 0)
{

if (cmdParamDesc[cmdCounter].loaded) {
	target = (texture)cmdParamDesc[cmdCounter].params[0][0];
	level = (int)cmdParamDesc[cmdCounter].params[1][0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].params[0][0] = (float)target;
	strcpy(cmdParamDesc[cmdCounter].paramType[0], "texture"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[0], "target"); 
	cmdParamDesc[cmdCounter].params[1][0] = (float)level;
	strcpy(cmdParamDesc[cmdCounter].paramType[1], "int"); 
	strcpy(cmdParamDesc[cmdCounter].paramName[1], "level"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Textures::RenderTarget(target,level);
}

#define RenderTarget(target, level) api.RenderTarget( __FILE__, __LINE__ , target, level)

} api;
