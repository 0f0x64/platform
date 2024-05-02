struct {
	struct {
		char fileName[MAX_PATH];
		int line;
	} caller;

	int pCount;
	float params[256];

	bool loaded = false;
} cmdParamDesc;

int cmdCounter = 0;//reset it in loop start point
int currentCmd = 0;//comes from ui

int cFunc = 0;

struct {
	char funcName[255];
}callList[1000];

void AddToUI(const char* funcName)
{
	strcpy(callList[cFunc].funcName, funcName);
	cFunc++;
}