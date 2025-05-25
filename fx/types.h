#if EditMode

struct {
	char name[255];
	int _min;
	int _max;

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

bool fillTypeTable(const char* name, int _min_value, int _max_value, const char* enumStr = NULL)
{
	strcpy(typeDesc[typeCount].name, name);
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

#define createType(name,type, _min, _max) typedef type name; bool name##_r = fillTypeTable(#name, _min,_max);
#define enumType(name, ...) enum class name:int { __VA_ARGS__}; bool name##_t = fillTypeTable(#name, 0, 255, #__VA_ARGS__);

#else

#define createSimpleType(name, _min, _max) typedef int name;
#define enumType(name, ...) enum class name:int { __VA_ARGS__};

#endif

struct float2 { float x; float y; };
struct float3 { float x; float y; float z; };
struct float4 { float x; float y; float z; float w; };
struct int2 { int x; int y; };
struct int3 { int x; int y; int z; };
struct int4 { int x; int y; int z; int w; };

#define float4x4 XMMATRIX
#define t8 unsigned char
#define t8s signed char
#define t16 unsigned short
#define t16s signed short
#define t32 unsigned int
#define t32s signed int

const float intToFloatDenom = 255.f;

createType(timestamp, t32, 0, (int)DEMO_DURATION* SAMPLING_FREQ);
createType(duration_time, t32, 0, (int)DEMO_DURATION* SAMPLING_FREQ);
createType(volume, t8, 0, 100);
createType(panorama, t8s, -90, 90);

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









