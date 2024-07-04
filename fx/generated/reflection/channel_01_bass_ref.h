#if REFLECTION

if (paramsAreLoaded) {
	if (!cmdParamDesc[cmdCounter].param[0].bypass) 	volume = (int)cmdParamDesc[cmdCounter].param[0].value[0];
	if (!cmdParamDesc[cmdCounter].param[1].bypass) 	pan = (int)cmdParamDesc[cmdCounter].param[1].value[0];
	if (!cmdParamDesc[cmdCounter].param[2].bypass)  ms = (visibility)cmdParamDesc[cmdCounter].param[2].value[0];
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
	cmdParamDesc[cmdCounter].param[2].value[0] = (int)ms;
	cmdParamDesc[cmdCounter].param[2].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[2].type, "visibility"); 
	strcpy(cmdParamDesc[cmdCounter].param[2].name, "ms"); 

		editor::paramEdit::setBypass();

}

AddToUI(__FUNCTION__);
cmdCounter++;

cmdLevel++;

#define channel_01_bass(volume, pan, ms) channel_01_bass( __FILE__, __LINE__ , volume, pan, ms)

#endif