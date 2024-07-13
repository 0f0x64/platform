#if REFLECTION

if (paramsAreLoaded) {
	if (!cmdParamDesc[cmdCounter].param[0].bypass)  i = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
	if (!cmdParamDesc[cmdCounter].param[1].bypass) 	level = (int)cmdParamDesc[cmdCounter].param[1].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)i;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "i"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = level;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "level"); 

		editor::paramEdit::setBypass();

		editor::paramEdit::setParamsAttr();

}

AddToUI(__FUNCTION__);
cmdCounter++;

cmdLevel++;

#define SetRT(i, level) SetRT( __FILE__, __LINE__ , i, level)

#endif