struct {

	char funcName[255];

	struct {
		char fileName[MAX_PATH];
		int line;
	} caller;

	int pCount;
	float params[256][4];
	char paramType[255][255];
	char paramName[255][255];

	bool loaded = false;

} cmdParamDesc[1000];

int cmdCounter = 0;//reset it in loop start point
int currentCmd = 0;//comes from ui

void AddToUI(const char* funcName)
{
	int x = strlen(funcName)-1;;
	for (x; x >= 0; x--) { if (funcName[x] == ':') { x++; break; } };

	strcpy(cmdParamDesc[cmdCounter].funcName, funcName+x);
}