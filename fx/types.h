//
char enumStringTable[255][255][255];
int enumCount[255];

int enumCounter = 0;

bool fillEnumTable(const char* name, const char* enumStr)
{
	int index = enumCounter;
	strcpy(enumStringTable[index][0], name);

	int counter = 1;
	int j = 0;
	for (unsigned int i = 0; i <= strlen(enumStr); i++)
	{
		if (*(enumStr + i) == ' ') continue;

		if (*(enumStr + i) == ',' || i == strlen(enumStr))
		{
			enumStringTable[index][counter][j] = 0;
			j = 0;
			counter++;
		}
		else
		{
			enumStringTable[index][counter][j++] = *(enumStr + i);
		}
	}

	enumCount[enumCounter] = counter - 1;

	enumCounter++;
	return true;
}

const float intToFloatDenom = 255.f;

typedef struct { float x; float y; float z; } vector3;
typedef struct { float x; float y; float z; float w; } vector4;

typedef struct { int x; int y; int z; } vector3i;
typedef struct { int x; int y; int z; int w; } vector4i;


typedef vector3i position;
typedef vector3i size;
typedef vector3i rotation;
typedef vector3i color;
typedef vector4i color4;
typedef vector4i rect;

#undef CreateTexture
#define CreateTexture(name,type,format,width,height,mip,depth) name,
enum class texture:int {
#include "projectFiles\texList.h"	
};

bool texturesToEnumType()
{
	strcpy(enumStringTable[enumCounter][0], "texture");

	int tc = 1;
	#undef CreateTexture
	#define CreateTexture(name,type,format,width,height,mip,depth) strcpy(enumStringTable[enumCounter][tc++],#name);
	#include "projectFiles\texList.h"

	enumCount[enumCounter] = tc - 1;
	enumCounter++;

	return true;
}

bool ta = texturesToEnumType();

#define enumType(name, ...) enum class name:int { __VA_ARGS__}; bool name##_b = fillEnumTable(#name, #__VA_ARGS__);

enumType(blendmode, off, on, alpha);
enumType(blendop, add, sub, revsub, min, max);
enumType(depthmode, off, on, readonly, writeonly);
enumType(filter, linear, point, minPoint_magLinear);
enumType(addr, clamp, wrap);
enumType(cullmode, off, front, back, wireframe);
enumType(topology, triList, lineList, lineStrip);
enumType(targetshader, vertex, pixel, both);

enumType(visibility, on, off, solo);

char* getEnumStr(char* name, int value)
{
	for (int i = 0; i < enumCounter; i++)
	{
		if (strcmp(name, enumStringTable[i][0]) == 0)
		{
			return enumStringTable[i][value + 1];
		}
	}
	return NULL;
}

int getEnumCount(char* name)
{
	for (int i = 0; i < enumCounter; i++)
	{
		if (strcmp(name, enumStringTable[i][0]) == 0)
		{
			return enumCount[i];
		}
	}

	return -1;
}
