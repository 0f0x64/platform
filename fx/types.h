#if EditMode

struct {
	char name[255];
	int _min;
	int _max;
	int _dim;

	char enumString[255][255];
	int enumCount = 0;

} typeDesc[255];

int typeCount = 0;

char tmpConv[255];

char* getStrValue(int index, int value)
{
	if (index < 0)//type is unknown
	{
		_itoa(value, tmpConv, 10);
		return tmpConv;
	}

	auto ec = typeDesc[index].enumCount;

	if (value >= 0 && value < ec)
	{
		return typeDesc[index].enumString[value];
	}

	_itoa(value, tmpConv, 10);
	return tmpConv;

}

int GetEnumValue(int typeIndex, const char* id)
{
	if (typeIndex < 0) return NULL;

	for (int i = 0; i < typeDesc[typeIndex].enumCount; i++)
	{
		if (strcmp(id, typeDesc[typeIndex].enumString[i]) == 0)
		{
			return i;
		}
	}
	return -1;
}

int getEnumCount(int typeIndex)
{
	if (typeIndex < 0) return -1;

	return typeDesc[typeIndex].enumCount;
}

bool isType(const char* t1, const char* t2)
{
	return (strcmp(t1, t2) == 0);
}

bool isTypeEnum(int typeIndex)
{
	if (typeIndex < 0) return false;

	return typeDesc[typeIndex].enumCount != 0 ? true : false;
}

int getTypeIndex(const char* t1)
{
	for (int i = 0; i < typeCount; i++)
	{
		if (strcmp(typeDesc[i].name, t1) == 0)
		{
			return i;
		}
	}

	return -1;
}

int getTypeDim(int typeIndex)
{
	if (typeIndex < 0) return 1;
	return typeDesc[typeIndex]._dim;
}

int getTypeMin(int typeIndex)
{
	if (typeIndex < 0) return INT_MIN;
	return typeDesc[typeIndex]._min;
}

int getTypeMax(int typeIndex)
{
	if (typeIndex < 0) return INT_MAX;
	return typeDesc[typeIndex]._max;
}

bool isNumber(const std::string& token)
{
	return std::regex_match(token, std::regex("(\\+|-)?[0-9]*(\\.?([0-9]+))$"));
}

bool fillTypeTable(const char* name, int _min_value, int _max_value, int _dim, const char* enumStr = NULL)
{
	strcpy(typeDesc[typeCount].name, name);
	typeDesc[typeCount]._dim = _dim;
	typeDesc[typeCount]._min = _min_value;
	typeDesc[typeCount]._max = _max_value;

	if (enumStr)
	{
		int counter = 0;
		int j = 0;
		for (unsigned int i = 0; i <= strlen(enumStr); i++)
		{
			if (*(enumStr + i) == ' ') continue;

			if (*(enumStr + i) == ',' || i == strlen(enumStr))
			{
				j = 0;
				counter++;
			}
			else
			{
				typeDesc[typeCount].enumString[counter][j++] = *(enumStr + i);
			}
		}

		typeDesc[typeCount].enumCount = counter;
	}

	typeCount++;

	return true;
}

bool texturesToEnumType()
{
	strcpy(typeDesc[typeCount].name, "texture");
	typeDesc[typeCount]._dim = 1;
	typeDesc[typeCount]._min = 0;
	typeDesc[typeCount]._max = 0;
	typeDesc[typeCount].enumCount = 0;

	int tc = 1;
#undef CreateTexture
#define CreateTexture(name,type,format,width,height,mip,depth) { \
	typeDesc[typeCount]._max++; \
	strcpy(typeDesc[typeCount].enumString[typeDesc[typeCount].enumCount++],#name); }

#include "projectFiles\texList.h"

	typeCount++;

	return true;
}

bool ta = texturesToEnumType();

#define createType(name, _min, _max, _dim, ...) typedef struct {  __VA_ARGS__ } name; bool name##_r = fillTypeTable(#name, _min,_max,_dim);
#define createSimpleType(name, _min, _max) typedef int name; bool name##_r = fillTypeTable(#name, _min,_max,1);
#define enumType(name, ...) enum class name:int { __VA_ARGS__}; bool name##_t = fillTypeTable(#name, 0, INT_MAX, 1, #__VA_ARGS__);

#else

#define createType(name, _min, _max, _dim, ...) typedef struct {  __VA_ARGS__ } name;
#define createSimpleType(name, _min, _max) typedef int name;
#define enumType(name, ...) enum class name:int { __VA_ARGS__};

#endif

const float intToFloatDenom = 255.f;

createType(vector3, 0, 0, 3, float x; float y; float z;);
createType(vector4, 0, 0, 4, float x; float y; float z; float w;);
createType(vector3i, 0, 0, 3, int x; int y; int z;);
createType(vector4i, 0, 0, 4, int x; int y; int z; int w;);

//internal 
typedef vector3 positionF;
typedef vector3 sizeF;
typedef vector3 rotationF;
//----

createType(position, INT_MIN, INT_MAX, 3, int x; int y; int z;);
createType(size, INT_MIN, INT_MAX, 3, int x; int y; int z;);
createType(rotation, INT_MIN, INT_MAX, 3, int x; int y; int z;);
createType(color, 0, 255, 3, int x; int y; int z;);
createType(color4, 0, 255, 4, int x; int y; int z; int w;);
createType(rect, INT_MIN, INT_MAX, 4, int x; int y; int z; int w;);
createSimpleType(timestamp, 0, (int)DEMO_DURATION* SAMPLING_FREQ);
createSimpleType(duration_time, 0, (int)DEMO_DURATION* SAMPLING_FREQ);
createSimpleType(volume, 0, 100);
createSimpleType(panorama, -90, 90);

enumType(blendmode, off, on, alpha);
enumType(blendop, add, sub, revsub, min, max);
enumType(depthmode, off, on, readonly, writeonly);
enumType(filter, linear, point, minPoint_magLinear);
enumType(addr, clamp, wrap);
enumType(cullmode, off, front, back, wireframe);
enumType(topology, triList, lineList, lineStrip);
enumType(targetshader, vertex, pixel, both);
enumType(keyType, set, slide);
enumType(visibility, on, off, solo);
enumType(camAxis, local, global);
enumType(sliderType, follow, pan, slide);
enumType(switcher, off, on);
enumType(overdub, off, on);
enumType(layers, pitch, variation, retrigger, slide, send0, send1, send2, send3);

#undef CreateTexture
#define CreateTexture(name,type,format,width,height,mip,depth) name,
enum class texture:int {
#include "projectFiles\texList.h"	
};

unsigned char va_params[255];

#define VA_READ \
va_list args;\
va_start(args, count);\
va_params[0] = count;\
for (int i = 1; i <= count; i++)\
{\
	va_params[i] = va_arg(args, unsigned char);\
};







