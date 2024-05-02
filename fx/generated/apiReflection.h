struct { 

void CalcCubemap( const char* srcFileName, int srcLine, int texture)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 1;

if (cmdParamDesc.loaded) {
texture = cmdParamDesc.params[0];
} else {
cmdParamDesc.params[0] = texture;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Cubemap::CalcCubemap(texture);
}

void ShowCubemap( const char* srcFileName, int srcLine, unsigned int envTex)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 1;

if (cmdParamDesc.loaded) {
} else {
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Cubemap::ShowCubemap(envTex);
}

void ShowObject( const char* srcFileName, int srcLine, int geometry, int normals, int quality, float x, float y, float z)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 6;

if (cmdParamDesc.loaded) {
geometry = cmdParamDesc.params[0];
normals = cmdParamDesc.params[1];
quality = cmdParamDesc.params[2];
x = cmdParamDesc.params[3];
y = cmdParamDesc.params[4];
z = cmdParamDesc.params[5];
} else {
cmdParamDesc.params[0] = geometry;
cmdParamDesc.params[1] = normals;
cmdParamDesc.params[2] = quality;
cmdParamDesc.params[3] = x;
cmdParamDesc.params[4] = y;
cmdParamDesc.params[5] = z;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Object::ShowObject(geometry,normals,quality,x,y,z);
}

void CalcObject( const char* srcFileName, int srcLine, int targetGeoTexture, int targetNrmlTexture)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 2;

if (cmdParamDesc.loaded) {
targetGeoTexture = cmdParamDesc.params[0];
targetNrmlTexture = cmdParamDesc.params[1];
} else {
cmdParamDesc.params[0] = targetGeoTexture;
cmdParamDesc.params[1] = targetNrmlTexture;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Object::CalcObject(targetGeoTexture,targetNrmlTexture);
}

void Blending( const char* srcFileName, int srcLine, int m = blendmode::off, int blend = blendop::add)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 2;

if (cmdParamDesc.loaded) {
m = cmdParamDesc.params[0];
blend = cmdParamDesc.params[1];
} else {
cmdParamDesc.params[0] = m;
cmdParamDesc.params[1] = blend;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Blend::Blending(m,blend);
}

void Camera( const char* srcFileName, int srcLine, camData* cam)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 1;

if (cmdParamDesc.loaded) {
} else {
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Camera::Camera(cam);
}

void ConstToVertex( const char* srcFileName, int srcLine, int i)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 1;

if (cmdParamDesc.loaded) {
i = cmdParamDesc.params[0];
} else {
cmdParamDesc.params[0] = i;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

ConstBuf::ConstToVertex(i);
}

void ConstToPixel( const char* srcFileName, int srcLine, int i)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 1;

if (cmdParamDesc.loaded) {
i = cmdParamDesc.params[0];
} else {
cmdParamDesc.params[0] = i;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

ConstBuf::ConstToPixel(i);
}

void Depth( const char* srcFileName, int srcLine, int m)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 1;

if (cmdParamDesc.loaded) {
m = cmdParamDesc.params[0];
} else {
cmdParamDesc.params[0] = m;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Depth::Depth(m);
}

void Clear( const char* srcFileName, int srcLine, float r, float g, float b, float a)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 4;

if (cmdParamDesc.loaded) {
r = cmdParamDesc.params[0];
g = cmdParamDesc.params[1];
b = cmdParamDesc.params[2];
a = cmdParamDesc.params[3];
} else {
cmdParamDesc.params[0] = r;
cmdParamDesc.params[1] = g;
cmdParamDesc.params[2] = b;
cmdParamDesc.params[3] = a;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Draw::Clear(r,g,b,a);
}

void ClearDepth( const char* srcFileName, int srcLine)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 0;
};
AddToUI(__FUNCTION__);
cmdCounter++;

Draw::ClearDepth();
}

void NullDrawer( const char* srcFileName, int srcLine, int quadCount, int instances = 1)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 2;

if (cmdParamDesc.loaded) {
quadCount = cmdParamDesc.params[0];
instances = cmdParamDesc.params[1];
} else {
cmdParamDesc.params[0] = quadCount;
cmdParamDesc.params[1] = instances;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Draw::NullDrawer(quadCount,instances);
}

void IA( const char* srcFileName, int srcLine, int topology)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 1;

if (cmdParamDesc.loaded) {
topology = cmdParamDesc.params[0];
} else {
cmdParamDesc.params[0] = topology;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

InputAssembler::IA(topology);
}

void Cull( const char* srcFileName, int srcLine, int i)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 1;

if (cmdParamDesc.loaded) {
i = cmdParamDesc.params[0];
} else {
cmdParamDesc.params[0] = i;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Rasterizer::Cull(i);
}

void Scissors( const char* srcFileName, int srcLine, float left, float top, float right, float bottom)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 4;

if (cmdParamDesc.loaded) {
left = cmdParamDesc.params[0];
top = cmdParamDesc.params[1];
right = cmdParamDesc.params[2];
bottom = cmdParamDesc.params[3];
} else {
cmdParamDesc.params[0] = left;
cmdParamDesc.params[1] = top;
cmdParamDesc.params[2] = right;
cmdParamDesc.params[3] = bottom;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Rasterizer::Scissors(left,top,right,bottom);
}

void Sampler( const char* srcFileName, int srcLine, targetShader shader, int slot, int filter, int addressU, int addressV)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 5;

if (cmdParamDesc.loaded) {
slot = cmdParamDesc.params[1];
filter = cmdParamDesc.params[2];
addressU = cmdParamDesc.params[3];
addressV = cmdParamDesc.params[4];
} else {
cmdParamDesc.params[1] = slot;
cmdParamDesc.params[2] = filter;
cmdParamDesc.params[3] = addressU;
cmdParamDesc.params[4] = addressV;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Sampler::Sampler(shader,slot,filter,addressU,addressV);
}

void SamplerComp( const char* srcFileName, int srcLine, int slot)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 1;

if (cmdParamDesc.loaded) {
slot = cmdParamDesc.params[0];
} else {
cmdParamDesc.params[0] = slot;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Sampler::SamplerComp(slot);
}

void vShader( const char* srcFileName, int srcLine, int n)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 1;

if (cmdParamDesc.loaded) {
n = cmdParamDesc.params[0];
} else {
cmdParamDesc.params[0] = n;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Shaders::vShader(n);
}

void pShader( const char* srcFileName, int srcLine, int n)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 1;

if (cmdParamDesc.loaded) {
n = cmdParamDesc.params[0];
} else {
cmdParamDesc.params[0] = n;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Shaders::pShader(n);
}

void CopyColor( const char* srcFileName, int srcLine, int dst, int src)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 2;

if (cmdParamDesc.loaded) {
dst = cmdParamDesc.params[0];
src = cmdParamDesc.params[1];
} else {
cmdParamDesc.params[0] = dst;
cmdParamDesc.params[1] = src;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Textures::CopyColor(dst,src);
}

void CopyDepth( const char* srcFileName, int srcLine, int dst, int src)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 2;

if (cmdParamDesc.loaded) {
dst = cmdParamDesc.params[0];
src = cmdParamDesc.params[1];
} else {
cmdParamDesc.params[0] = dst;
cmdParamDesc.params[1] = src;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Textures::CopyDepth(dst,src);
}

void TextureToShader( const char* srcFileName, int srcLine, int tex, int slot, targetShader tA = targetShader::both)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 3;

if (cmdParamDesc.loaded) {
tex = cmdParamDesc.params[0];
slot = cmdParamDesc.params[1];
} else {
cmdParamDesc.params[0] = tex;
cmdParamDesc.params[1] = slot;
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Textures::TextureToShader(tex,slot,tA);
}

void CreateMipMap( const char* srcFileName, int srcLine)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 0;
};
AddToUI(__FUNCTION__);
cmdCounter++;

Textures::CreateMipMap();
}

void RenderTarget( const char* srcFileName, int srcLine, byte texId = mainRTIndex, byte level = 0)
{
if (currentCmd ==  cmdCounter) {
strcpy(cmdParamDesc.caller.fileName,srcFileName);
cmdParamDesc.caller.line = srcLine;
cmdParamDesc.pCount = 2;

if (cmdParamDesc.loaded) {
} else {
cmdParamDesc.loaded = true;
}
};
AddToUI(__FUNCTION__);
cmdCounter++;

Textures::RenderTarget(texId,level);
}

} api;
