#if REFLECTION

if (paramsAreLoaded) {
	i = (depthmode)cmdParamDesc[cmdCounter].param[0].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)i;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "depthmode"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "i"); 
}

AddToUI(__FUNCTION__);
cmdCounter++;

cmdLevel++;

#define SetDepthMode(i) SetDepthMode( __FILE__, __LINE__ , i)

#endif