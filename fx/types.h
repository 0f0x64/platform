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

typedef vector3 positionF;
typedef vector3 sizeF;
typedef vector3 rotationF;


typedef vector3i position;
typedef vector3i size;
typedef vector3i rotation;
typedef vector3i color;
typedef vector4i color4;
typedef vector4i rect;

typedef int timestamp;
typedef int duration_time;
typedef int duration_ticks;


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
enumType(keyType, set, slide);
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

int GetEnumValue(const char* type, const char* id)
{
	for (int i = 0; i < enumCounter; i++)
	{
		if (strcmp(type, enumStringTable[i][0]) == 0)
		{
			int j = 1; 
			while (enumStringTable[i][j][0] != 0)
			{
				if (strcmp(id, enumStringTable[i][j]) == 0)
				{
					return j - 1;
				}
				j++;
			}
			return INT_MAX;
		}
	}

	return INT_MAX;
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

bool isType(const char* t1, const char* t2)
{
	return (strcmp(t1, t2) == 0);
}

bool isTypeEnum(const char* t1)
{
	if (
		isType(t1, "texture") ||
		isType(t1, "topology") ||
		isType(t1, "blendmode") ||
		isType(t1, "blendop") ||
		isType(t1, "depthmode") ||
		isType(t1, "filter") ||
		isType(t1, "addr") ||
		isType(t1, "cullmode") ||
		isType(t1, "targetshader") ||
		isType(t1, "keyType") ||
		isType(t1, "visibility")
		)
	{
		return true;
	}

	return false;
}

int getTypeDim(const char* t1)
{
	if (
		isType(t1, "position") ||
		isType(t1, "size") ||
		isType(t1, "rotation") ||
		isType(t1, "color")
		)
	{
		return 3;
	}

	if (
		isType(t1, "color4") ||
		isType(t1, "rect")
		)
	{
		return 4;
	}

	return 1;
}

bool isNumber(const std::string& token)
{
	return std::regex_match(token, std::regex("(\\+|-)?[0-9]*(\\.?([0-9]+))$"));
}
