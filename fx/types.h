//
char enumStringTable[255][255][255];

int enumCounter = 0;

bool fillEnumTable(const char* name, const char* enumStr)
{
	int index = enumCounter;
	strcpy(enumStringTable[index][0], name);
	strcat(enumStringTable[index][0], "_");

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

	enumCounter++;
	return true;
}

typedef struct { float x; float y; float z; } vector3;
typedef struct { float x; float y; float z; float w; } vector4;

typedef vector3 position_;
typedef vector3 size_;
typedef vector3 rotation_;
typedef vector3 color_;
typedef vector4 color4_;
typedef vector4 rect_;

typedef int texture_;

#define enumType(name, ...) namespace name { enum { __VA_ARGS__}; }; typedef int name##_; bool name##_b = fillEnumTable(#name, #__VA_ARGS__);

enumType(blendmode, off, on, alpha);
enumType(blendop, add, sub, revsub, min, max);
enumType(depthmode, off, on, readonly, writeonly);
enumType(filter, linear, point, minPoint_magLinear);
enumType(addr, clamp, wrap);
enumType(cullmode, off, front, back, wireframe);
enumType(topology, triList, lineList, lineStrip);
enumType(targetshader, vertex, pixel, both);


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
