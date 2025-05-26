#define float4x4 XMMATRIX

#if EditMode

struct {
	char name[255];
	int _min;
	int _max;

	char enumString[255][255];
	int enumCount = 0;
	int size;

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

int getTypeSize(int typeIndex)
{
	if (typeIndex < 0) return 4;
	return typeDesc[typeIndex].size;
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

bool fillTypeTable(int size, const char* name, int _min_value, int _max_value, const char* enumStr = NULL)
{
	strcpy(typeDesc[typeCount].name, name);
	typeDesc[typeCount]._min = _min_value;
	typeDesc[typeCount]._max = _max_value;
	typeDesc[typeCount].size =size;

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
	typeDesc[typeCount].size = 1;

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

#define createType32s(name, _min, _max) typedef int name; bool name##_r = fillTypeTable(sizeof(int), #name, _min,_max);
#define createType32u(name, _min, _max) typedef unsigned int name; bool name##_r = fillTypeTable(sizeof(unsigned int), #name, _min,_max);
#define createType16s(name, _min, _max) typedef short name; bool name##_r = fillTypeTable(sizeof(short), #name, _min,_max);
#define createType16u(name, _min, _max) typedef unsigned short name; bool name##_r = fillTypeTable(sizeof(unsigned short), #name, _min,_max);
#define createType8s(name, _min, _max) typedef char name; bool name##_r = fillTypeTable(sizeof(char), #name, _min,_max);
#define createType8u(name, _min, _max) typedef unsigned char name; bool name##_r = fillTypeTable(sizeof(unsigned char), #name, _min,_max);

#define createTypeEnum(name, ...) enum class name:unsigned char { __VA_ARGS__}; bool name##_t = fillTypeTable(sizeof(int), #name, 0, 255, #__VA_ARGS__);

#else

#define createType32s (name, _min, _max) typedef int name;
#define createType32u (name, _min, _max) typedef unsigned int name;
#define createType16s (name, _min, _max) typedef short name;
#define createType16u (name, _min, _max) typedef unsigned short name;
#define createType8s (name, _min, _max) typedef char name;
#define createType8u (name, _min, _max) typedef unsigned char name;


#define createTypeEnum(name, ...) enum class name:unsigned char { __VA_ARGS__};

#endif

struct float2 { float x; float y; };
struct float3 { float x; float y; float z; };
struct float4 { float x; float y; float z; float w; };
struct int2 { int x; int y; };
struct int3 { int x; int y; int z; };
struct int4 { int x; int y; int z; int w; };


const float intToFloatDenom = 255.f;

createType32u(int32u, 0, UINT32_MAX);
createType32s(int32s, INT32_MIN, INT32_MAX);
createType16u(int16u, 0, UINT16_MAX);
createType16s(int16s, INT16_MIN, INT16_MAX);
createType8u(int8u, 0, 255);
createType8s(int8s, INT8_MIN, INT8_MAX);

createType32s(timestamp, 0, (int)DEMO_DURATION* SAMPLING_FREQ);
createType32s(duration_time, 0, (int)DEMO_DURATION* SAMPLING_FREQ);
createType8u(volume, 0, 100);
createType8s(panorama, -90, 90);

createTypeEnum(blendmode, off, on, alpha);
createTypeEnum(blendop, add, sub, revsub, min, max);
createTypeEnum(depthmode, off, on, readonly, writeonly);
createTypeEnum(filter, linear, point, minPoint_magLinear);
createTypeEnum(addr, clamp, wrap);
createTypeEnum(cullmode, off, front, back, wireframe);
createTypeEnum(topology, triList, lineList, lineStrip);
createTypeEnum(targetshader, vertex, pixel, both);
createTypeEnum(keyType, set, slide);
createTypeEnum(visibility, on, off, solo);
createTypeEnum(camAxis, local, global);
createTypeEnum(sliderType, follow, pan, slide);

createTypeEnum(switcher, off, on);
createTypeEnum(overdub, off, on);
createTypeEnum(layers, pitch, variation, retrigger, slide, send0, send1, send2, send3);

#undef CreateTexture
#define CreateTexture(name,type,format,width,height,mip,depth) name,
enum class texture:unsigned char {
#include "projectFiles\texList.h"	
};









