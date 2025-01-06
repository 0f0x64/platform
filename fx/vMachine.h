/*#define MAX_PARAMS 255

struct {

	PVFN routine;

	int pCount;
	struct {
		char name[255];
		char pEnum[255][255];
	} param[MAX_PARAMS];


} cmdDesc[255];


struct {
	int funcCode;
	float param[MAX_PARAMS][4];
} stack[10000];

void execute()
{
	int i = 0;
	while (stack[i].funcCode != 0)
	{
		stack[i].funcCode();
		i++;
	}
}*/