typedef void (*PVFN)(int i, float &x, float &y, float w, float lead, float sel);

struct ParamDesc {

	bool single_line;
	bool init_with_names;

	char funcName[255];
	char funcGroup[255];

	struct {
		char fileName[MAX_PATH];
		int line;
	} caller;

	PVFN uiDraw = NULL;
	PVFN uiClick = NULL;

	int pCount;

	struct {
		int value;
		char strValue[255];
		char type[255];
		int typeIndex;
		char name[255];
		bool bypass;
		int _min;
		int _max;
		int size;
		int offset;
	} param[255];

	int stackLevel = 0;

};

ParamDesc cmdParamDesc[1000];
ParamDesc cmdParamDescBack;

bool paramsAreLoaded = false;
int cmdLevel = 0;
int startCmd = 0;
int curCmdLevel = 0;

int currentCmd_backup = -1;
int currentCmd = -1;//comes from ui
int hilightedCmd = -1;

void AddToUI(const char* funcName)
{
	cmdParamDesc[cmdCounter].stackLevel = cmdLevel;

	int x = strlen(funcName)-1;;
	for (x; x > 0; x--) { if (funcName[x] == ':') { x++; break; } };

	strcpy(cmdParamDesc[cmdCounter].funcName, funcName+x);
}
