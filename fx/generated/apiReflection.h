namespace api { 

void CalcCubemap( const char* srcFileName, int srcLine, texture target)
{

if (cmdParamDesc[cmdCounter].loaded) {
	target = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)target;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "target"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Cubemap::CalcCubemap(target);
}

void ShowCubemap( const char* srcFileName, int srcLine, texture envTexture)
{

if (cmdParamDesc[cmdCounter].loaded) {
	envTexture = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)envTexture;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "envTexture"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Cubemap::ShowCubemap(envTexture);
}

void ShowObject( const char* srcFileName, int srcLine, texture geometry, texture normals, unsigned int quality, position pos)
{

if (cmdParamDesc[cmdCounter].loaded) {
	geometry = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
	normals = (texture)cmdParamDesc[cmdCounter].param[1].value[0];
	quality = (unsigned int)cmdParamDesc[cmdCounter].param[2].value[0];
	pos.x = cmdParamDesc[cmdCounter].param[3].value[0];
	pos.y = cmdParamDesc[cmdCounter].param[3].value[1];
	pos.z = cmdParamDesc[cmdCounter].param[3].value[2];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 4;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)geometry;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "geometry"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = (int)normals;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "normals"); 
	cmdParamDesc[cmdCounter].param[2].value[0] = quality;
	cmdParamDesc[cmdCounter].param[2].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[2].type, "unsigned int"); 
	strcpy(cmdParamDesc[cmdCounter].param[2].name, "quality"); 
	cmdParamDesc[cmdCounter].param[3].value[0] = pos.x;
	cmdParamDesc[cmdCounter].param[3].value[1] = pos.y;
	cmdParamDesc[cmdCounter].param[3].value[2] = pos.z;
	cmdParamDesc[cmdCounter].param[3].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[3].type, "position"); 
	strcpy(cmdParamDesc[cmdCounter].param[3].name, "pos"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Object::ShowObject(geometry,normals,quality,pos);
}

void CalcObject( const char* srcFileName, int srcLine, texture targetGeo, texture targetNrml)
{

if (cmdParamDesc[cmdCounter].loaded) {
	targetGeo = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
	targetNrml = (texture)cmdParamDesc[cmdCounter].param[1].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)targetGeo;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "targetGeo"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = (int)targetNrml;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "targetNrml"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Object::CalcObject(targetGeo,targetNrml);
}

void Blending( const char* srcFileName, int srcLine, blendmode mode = blendmode::off, blendop operation = blendop::add)
{

if (cmdParamDesc[cmdCounter].loaded) {
	mode = (blendmode)cmdParamDesc[cmdCounter].param[0].value[0];
	operation = (blendop)cmdParamDesc[cmdCounter].param[1].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)mode;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "blendmode"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "mode"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = (int)operation;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "blendop"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "operation"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Blend::Blending(mode,operation);
}

void Camera( const char* srcFileName, int srcLine, position eye, position at, position up, unsigned int angle)
{

if (cmdParamDesc[cmdCounter].loaded) {
	eye.x = cmdParamDesc[cmdCounter].param[0].value[0];
	eye.y = cmdParamDesc[cmdCounter].param[0].value[1];
	eye.z = cmdParamDesc[cmdCounter].param[0].value[2];
	at.x = cmdParamDesc[cmdCounter].param[1].value[0];
	at.y = cmdParamDesc[cmdCounter].param[1].value[1];
	at.z = cmdParamDesc[cmdCounter].param[1].value[2];
	up.x = cmdParamDesc[cmdCounter].param[2].value[0];
	up.y = cmdParamDesc[cmdCounter].param[2].value[1];
	up.z = cmdParamDesc[cmdCounter].param[2].value[2];
	angle = (unsigned int)cmdParamDesc[cmdCounter].param[3].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 4;
	cmdParamDesc[cmdCounter].param[0].value[0] = eye.x;
	cmdParamDesc[cmdCounter].param[0].value[1] = eye.y;
	cmdParamDesc[cmdCounter].param[0].value[2] = eye.z;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "position"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "eye"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = at.x;
	cmdParamDesc[cmdCounter].param[1].value[1] = at.y;
	cmdParamDesc[cmdCounter].param[1].value[2] = at.z;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "position"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "at"); 
	cmdParamDesc[cmdCounter].param[2].value[0] = up.x;
	cmdParamDesc[cmdCounter].param[2].value[1] = up.y;
	cmdParamDesc[cmdCounter].param[2].value[2] = up.z;
	cmdParamDesc[cmdCounter].param[2].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[2].type, "position"); 
	strcpy(cmdParamDesc[cmdCounter].param[2].name, "up"); 
	cmdParamDesc[cmdCounter].param[3].value[0] = angle;
	cmdParamDesc[cmdCounter].param[3].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[3].type, "unsigned int"); 
	strcpy(cmdParamDesc[cmdCounter].param[3].name, "angle"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Camera::Camera(eye,at,up,angle);
}

void Depth( const char* srcFileName, int srcLine, depthmode mode)
{

if (cmdParamDesc[cmdCounter].loaded) {
	mode = (depthmode)cmdParamDesc[cmdCounter].param[0].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)mode;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "depthmode"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "mode"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Depth::Depth(mode);
}

void Clear( const char* srcFileName, int srcLine, color4 color)
{

if (cmdParamDesc[cmdCounter].loaded) {
	color.x = cmdParamDesc[cmdCounter].param[0].value[0];
	color.y = cmdParamDesc[cmdCounter].param[0].value[1];
	color.z = cmdParamDesc[cmdCounter].param[0].value[2];
	color.w = cmdParamDesc[cmdCounter].param[0].value[3];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = color.x;
	cmdParamDesc[cmdCounter].param[0].value[1] = color.y;
	cmdParamDesc[cmdCounter].param[0].value[2] = color.z;
	cmdParamDesc[cmdCounter].param[0].value[3] = color.w;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "color4"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "color"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Draw::Clear(color);
}

void ClearDepth( const char* srcFileName, int srcLine)
{

AddToUI(__FUNCTION__);
cmdCounter++;

Draw::ClearDepth();
}

void NullDrawer( const char* srcFileName, int srcLine, int quadCount, unsigned int instances = 1)
{

if (cmdParamDesc[cmdCounter].loaded) {
	quadCount = (int)cmdParamDesc[cmdCounter].param[0].value[0];
	instances = (unsigned int)cmdParamDesc[cmdCounter].param[1].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].param[0].value[0] = quadCount;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "quadCount"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = instances;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "unsigned int"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "instances"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Draw::NullDrawer(quadCount,instances);
}

void IA( const char* srcFileName, int srcLine, topology topoType)
{

if (cmdParamDesc[cmdCounter].loaded) {
	topoType = (topology)cmdParamDesc[cmdCounter].param[0].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)topoType;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "topology"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "topoType"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

InputAssembler::IA(topoType);
}

void Cull( const char* srcFileName, int srcLine, cullmode mode)
{

if (cmdParamDesc[cmdCounter].loaded) {
	mode = (cullmode)cmdParamDesc[cmdCounter].param[0].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)mode;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "cullmode"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "mode"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Rasterizer::Cull(mode);
}

void Scissors( const char* srcFileName, int srcLine, rect r)
{

if (cmdParamDesc[cmdCounter].loaded) {
	r.x = cmdParamDesc[cmdCounter].param[0].value[0];
	r.y = cmdParamDesc[cmdCounter].param[0].value[1];
	r.z = cmdParamDesc[cmdCounter].param[0].value[2];
	r.w = cmdParamDesc[cmdCounter].param[0].value[3];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = r.x;
	cmdParamDesc[cmdCounter].param[0].value[1] = r.y;
	cmdParamDesc[cmdCounter].param[0].value[2] = r.z;
	cmdParamDesc[cmdCounter].param[0].value[3] = r.w;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "rect"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "r"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Rasterizer::Scissors(r);
}

void Sampler( const char* srcFileName, int srcLine, targetshader shader, unsigned int slot, filter filterType, addr addressU, addr addressV)
{

if (cmdParamDesc[cmdCounter].loaded) {
	shader = (targetshader)cmdParamDesc[cmdCounter].param[0].value[0];
	slot = (unsigned int)cmdParamDesc[cmdCounter].param[1].value[0];
	filterType = (filter)cmdParamDesc[cmdCounter].param[2].value[0];
	addressU = (addr)cmdParamDesc[cmdCounter].param[3].value[0];
	addressV = (addr)cmdParamDesc[cmdCounter].param[4].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 5;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)shader;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "targetshader"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "shader"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = slot;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "unsigned int"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "slot"); 
	cmdParamDesc[cmdCounter].param[2].value[0] = (int)filterType;
	cmdParamDesc[cmdCounter].param[2].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[2].type, "filter"); 
	strcpy(cmdParamDesc[cmdCounter].param[2].name, "filterType"); 
	cmdParamDesc[cmdCounter].param[3].value[0] = (int)addressU;
	cmdParamDesc[cmdCounter].param[3].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[3].type, "addr"); 
	strcpy(cmdParamDesc[cmdCounter].param[3].name, "addressU"); 
	cmdParamDesc[cmdCounter].param[4].value[0] = (int)addressV;
	cmdParamDesc[cmdCounter].param[4].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[4].type, "addr"); 
	strcpy(cmdParamDesc[cmdCounter].param[4].name, "addressV"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Sampler::Sampler(shader,slot,filterType,addressU,addressV);
}

void SamplerComp( const char* srcFileName, int srcLine, unsigned int slot)
{

if (cmdParamDesc[cmdCounter].loaded) {
	slot = (unsigned int)cmdParamDesc[cmdCounter].param[0].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = slot;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "unsigned int"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "slot"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Sampler::SamplerComp(slot);
}

void vShader( const char* srcFileName, int srcLine, unsigned int n)
{

if (cmdParamDesc[cmdCounter].loaded) {
	n = (unsigned int)cmdParamDesc[cmdCounter].param[0].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = n;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "unsigned int"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "n"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Shaders::vShader(n);
}

void pShader( const char* srcFileName, int srcLine, unsigned int n)
{

if (cmdParamDesc[cmdCounter].loaded) {
	n = (unsigned int)cmdParamDesc[cmdCounter].param[0].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = n;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "unsigned int"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "n"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Shaders::pShader(n);
}

void CopyColor( const char* srcFileName, int srcLine, texture dst, texture src)
{

if (cmdParamDesc[cmdCounter].loaded) {
	dst = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
	src = (texture)cmdParamDesc[cmdCounter].param[1].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)dst;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "dst"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = (int)src;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "src"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Textures::CopyColor(dst,src);
}

void CopyDepth( const char* srcFileName, int srcLine, texture dst, texture src)
{

if (cmdParamDesc[cmdCounter].loaded) {
	dst = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
	src = (texture)cmdParamDesc[cmdCounter].param[1].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)dst;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "dst"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = (int)src;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "src"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Textures::CopyDepth(dst,src);
}

void TextureToShader( const char* srcFileName, int srcLine, texture tex, unsigned int slot, targetshader tA = targetshader::both)
{

if (cmdParamDesc[cmdCounter].loaded) {
	tex = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
	slot = (unsigned int)cmdParamDesc[cmdCounter].param[1].value[0];
	tA = (targetshader)cmdParamDesc[cmdCounter].param[2].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 3;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)tex;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "tex"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = slot;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "unsigned int"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "slot"); 
	cmdParamDesc[cmdCounter].param[2].value[0] = (int)tA;
	cmdParamDesc[cmdCounter].param[2].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[2].type, "targetshader"); 
	strcpy(cmdParamDesc[cmdCounter].param[2].name, "tA"); 
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

void RenderTarget( const char* srcFileName, int srcLine, texture target, unsigned int level = 0)
{

if (cmdParamDesc[cmdCounter].loaded) {
	target = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
	level = (unsigned int)cmdParamDesc[cmdCounter].param[1].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)target;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "target"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = level;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "unsigned int"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "level"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

Textures::RenderTarget(target,level);
}

void oscillator( const char* srcFileName, int srcLine, int a, int b)
{

if (cmdParamDesc[cmdCounter].loaded) {
	a = (int)cmdParamDesc[cmdCounter].param[0].value[0];
	b = (int)cmdParamDesc[cmdCounter].param[1].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].param[0].value[0] = a;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "a"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = b;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "b"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

tracker::oscillator(a,b);
}

void eq( const char* srcFileName, int srcLine, int a)
{

if (cmdParamDesc[cmdCounter].loaded) {
	a = (int)cmdParamDesc[cmdCounter].param[0].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = a;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "a"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

tracker::eq(a);
}

void channel_01_bass( const char* srcFileName, int srcLine, int volume, int pan, visibility ms)
{

if (cmdParamDesc[cmdCounter].loaded) {
	volume = (int)cmdParamDesc[cmdCounter].param[0].value[0];
	pan = (int)cmdParamDesc[cmdCounter].param[1].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 3;
	cmdParamDesc[cmdCounter].param[0].value[0] = volume;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "volume"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = pan;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "pan"); 
	cmdParamDesc[cmdCounter].param[2].bypass = true;
	strcpy(cmdParamDesc[cmdCounter].param[2].type, "visibility"); 
	strcpy(cmdParamDesc[cmdCounter].param[2].name, "ms"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

tracker::channel_01_bass(volume,pan,ms);
}

void channel_02_solo( const char* srcFileName, int srcLine, int volume,int pan,visibility ms)
{

if (cmdParamDesc[cmdCounter].loaded) {
	volume = (int)cmdParamDesc[cmdCounter].param[0].value[0];
	pan = (int)cmdParamDesc[cmdCounter].param[1].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 3;
	cmdParamDesc[cmdCounter].param[0].value[0] = volume;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "volume"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = pan;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "pan"); 
	cmdParamDesc[cmdCounter].param[2].bypass = true;
	strcpy(cmdParamDesc[cmdCounter].param[2].type, "visibility"); 
	strcpy(cmdParamDesc[cmdCounter].param[2].name, "ms"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

tracker::channel_02_solo(volume,pan,ms);
}

void mix( const char* srcFileName, int srcLine, int level)
{

if (cmdParamDesc[cmdCounter].loaded) {
	level = (int)cmdParamDesc[cmdCounter].param[0].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = level;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "level"); 
	cmdParamDesc[cmdCounter].loaded = true; 
}

AddToUI(__FUNCTION__);
cmdCounter++;

tracker::mix(level);
}

void playTrack( const char* srcFileName, int srcLine)
{

AddToUI(__FUNCTION__);
cmdCounter++;

tracker::playTrack();
}

};
