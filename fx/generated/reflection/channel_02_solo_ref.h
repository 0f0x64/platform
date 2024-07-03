#if REFLECTION

if (paramsAreLoaded) {
	volume = (int)cmdParamDesc[cmdCounter].param[0].value[0];
	pan = (int)cmdParamDesc[cmdCounter].param[1].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 3;
	cmdParamDesc[cmdCounter].param[0].value[0] = volume;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "volume"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = pan;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "pan"); 
	cmdParamDesc[cmdCounter].param[2].bypass = true;
	strcpy(cmdParamDesc[cmdCounter].param[2].type, "visibility"); 
	strcpy(cmdParamDesc[cmdCounter].param[2].name, "ms"); 
}

AddToUI(__FUNCTION__);
cmdCounter++;

cmdLevel++;

#define channel_02_solo(volume, pan, ms) channel_02_solo( __FILE__, __LINE__ , volume, pan, ms)

#endif