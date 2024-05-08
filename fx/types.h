//

typedef struct { float x; float y; float z; } position_;
typedef struct { float x; float y; float z; } size_;
typedef struct { float x; float y; float z; } rotation_;
typedef struct { float r; float g; float b; } color_;
typedef struct { float r; float g; float b; float a; } color4_;
typedef struct { float x; float y; float x1; float y1; } rect_;

typedef int texture_;
typedef int topology_;
typedef int blendmode_;
typedef int blendop_;
typedef int depthmode_;
typedef int filter_;
typedef int addr_;
typedef int cullmode_;
typedef int targetshader_;

#define vEnum(name, ...) namespace name { enum { __VA_ARGS__}; }; const char* name##_str = #__VA_ARGS__;

vEnum(blendmode, off, on, alpha);
vEnum(blendop, add, sub, revsub, min, max);
vEnum(depthmode, off, on, readonly, writeonly);
vEnum(filter, linear, point, minPoint_magLinear);
vEnum(addr, clamp, wrap);
vEnum(cullmode, off, front, back, wireframe);
vEnum(topology, triList, lineList, lineStrip);
vEnum(targetshader, vertex, pixel, both);



void getStr(const char* mStr, char* result, int enumToGet)
{
	char rStr[128];

	int i = 0; int commaCounter = 0;
	int offset = 0;

	for (unsigned int i = 0; i <= strlen(mStr); i++)
	{
		if (*(mStr + i) == ',' || i == strlen(mStr))
		{
			commaCounter++;

			if (commaCounter == enumToGet + 1)
			{
				while (*(mStr + offset) == ' ') offset++;
				int j = i - 1;
				while (*(mStr + j) == ' ') j--;

				//todo::check bounds
				memcpy(rStr, (char*)(mStr + offset), j - offset + 1);
				rStr[j - offset + 1] = 0;
				strcpy(result, rStr);
				break;
			}
			offset = i + 1;
		}
	}
}

#define getEnumStr(m, s, n) getStr(m##_str, s,n)