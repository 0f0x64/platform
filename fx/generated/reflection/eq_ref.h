#if REFLECTION

if (paramsAreLoaded) {
	if (!cmdParamDesc[cmdCounter].param[0].bypass) 	a = (int)cmdParamDesc[cmdCounter].param[0].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = a;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "a"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
}

AddToUI(__FUNCTION__);
cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
cmdLevel++;

#define eq(a) eq( __FILE__, __LINE__ , a)

#else

#define eq(a) eq( a)

#endif

cmdCounter++;

