#if REFLECTION

if (paramsAreLoaded) {
	mode = (blendmode)cmdParamDesc[cmdCounter].param[0].value[0];
	op = (blendop)cmdParamDesc[cmdCounter].param[1].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)mode;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "blendmode"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "mode"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = (int)op;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "blendop"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "op"); 
}

AddToUI(__FUNCTION__);
cmdCounter++;

cmdLevel++;

#define SetBlendMode(mode, op) SetBlendMode( __FILE__, __LINE__ , mode, op)

#endif