#if REFLECTION

if (paramsAreLoaded) {
	if (!cmdParamDesc[cmdCounter].param[0].bypass)  i = (cullmode)cmdParamDesc[cmdCounter].param[0].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)i;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "cullmode"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "i"); 

		editor::paramEdit::setBypass();

		editor::paramEdit::setParamsAttr();

}

AddToUI(__FUNCTION__);
cmdCounter++;

cmdLevel++;

#define SetCull(i) SetCull( __FILE__, __LINE__ , i)

#endif