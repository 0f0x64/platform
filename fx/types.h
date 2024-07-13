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

struct {
	char name[255];
	int _min;
	int _max;
	int _dim;
} typeDesc[255];

int typeCount = 0;

bool fillTypeTable(const char* name, int _min_value, int _max_value, int _dim)
{
	strcpy(typeDesc[typeCount].name, name);
	typeDesc[typeCount]._dim = _dim;
	typeDesc[typeCount]._min = _min_value;
	typeDesc[typeCount]._max = _max_value;
	typeCount++;
	return true;
}

#define createType(name, _min, _max, _dim, ...) typedef struct {  __VA_ARGS__ } name; bool name##_r = fillTypeTable(#name, _min,_max,_dim);
#define createSimpleType(name, _min, _max) typedef int name; bool name##_r = fillTypeTable(#name, _min,_max,1);

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
createSimpleType(timestamp, 0, DEMO_DURATION* SAMPLING_FREQ);
createSimpleType(duration_time, 0, DEMO_DURATION* SAMPLING_FREQ);


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

char tmpConv[255];

int getEnumTableIndex(char* name)
{
	for (int i = 0; i < enumCounter; i++)
	{
		if (strcmp(name, enumStringTable[i][0]) == 0)
		{
			return i;
		}
	}

	return -1;

}

char* getStrValue(char* name, int value)
{
	for (int i = 0; i < enumCounter; i++)
	{
		if (strcmp(name, enumStringTable[i][0]) == 0)
		{
			return enumStringTable[i][value + 1];
		}
	}

	_itoa(value, tmpConv, 10);

	return tmpConv;
	
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

bool isTypeEnum(const char* name)
{
	for (int i = 0; i < enumCounter; i++)
	{
		if (strcmp(name, enumStringTable[i][0]) == 0)
		{
			return true;
		}
	}

	return false;
}

int getTypeDim(const char* t1)
{
	for (int i = 0; i < typeCount; i++)
	{
	if (strcmp(typeDesc[i].name,t1) == 0)
		{
		return typeDesc[i]._dim;
		}
	}

	return 1;
}

bool isNumber(const std::string& token)
{
	return std::regex_match(token, std::regex("(\\+|-)?[0-9]*(\\.?([0-9]+))$"));
}
