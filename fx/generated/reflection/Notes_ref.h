#if REFLECTION

if (paramsAreLoaded) {
	count = cmdParamDesc[cmdCounter].param[0].value[0];
	cmdParamDesc[cmdCounter].pCount = count+1;
	for (int i = 1; i <= count; i++)
	{
		params [i]= (unsigned char) cmdParamDesc[cmdCounter].param[i].value[0];
	}
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].param[0].value[0] = count ;
	cmdParamDesc[cmdCounter].pCount = count+1;
	for (int i = 0; i < 256; i++)
	{
		cmdParamDesc[cmdCounter].param[i].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[i].type , "int");
 		cmdParamDesc[cmdCounter].param[i].typeIndex = -1;
 		cmdParamDesc[cmdCounter].param[i]._min = 0;
		cmdParamDesc[cmdCounter].param[i]._max = 255;
		cmdParamDesc[cmdCounter].param[i]._dim = 1;
	}
	for (int i = 1; i <= count; i++)
	{
		cmdParamDesc[cmdCounter].param[i].value[0]= (int)params[i];
	}
}

AddToUI(__FUNCTION__);
cmdLevel++;

#define Notes(...) Notes( __FILE__, __LINE__ , __VA_ARGS__)

#else

#define Notes(...) Notes( __VA_ARGS__)

#endif

cmdCounter++;

