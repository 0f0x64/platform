struct {

	char funcName[255];

	struct {
		char fileName[MAX_PATH];
		int line;
	} caller;

	int pCount;

	struct {
		int value[4];
		char type[255];
		char name[255];
		bool bypass;
	} param[255];

	bool loaded = false;

} cmdParamDesc[1000];

int cmdCounter = 0;//reset it in loop start point
int currentCmd = -1;//comes from ui
int hilightedCmd = -1;

void AddToUI(const char* funcName)
{
	int x = strlen(funcName)-1;;
	for (x; x >= 0; x--) { if (funcName[x] == ':') { x++; break; } };

	strcpy(cmdParamDesc[cmdCounter].funcName, funcName+x);
}