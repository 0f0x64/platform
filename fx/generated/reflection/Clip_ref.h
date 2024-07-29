#if REFLECTION

if (paramsAreLoaded) {
	if (!cmdParamDesc[cmdCounter].param[0].bypass) 	pos = (int)cmdParamDesc[cmdCounter].param[0].value[0];
	if (!cmdParamDesc[cmdCounter].param[1].bypass) 	len = (int)cmdParamDesc[cmdCounter].param[1].value[0];
	if (!cmdParamDesc[cmdCounter].param[2].bypass) 	repeat = (int)cmdParamDesc[cmdCounter].param[2].value[0];
	if (!cmdParamDesc[cmdCounter].param[3].bypass) 	bpmScale = (int)cmdParamDesc[cmdCounter].param[3].value[0];
	if (!cmdParamDesc[cmdCounter].param[4].bypass) 	mode = (bool)cmdParamDesc[cmdCounter].param[4].value[0];
	if (!cmdParamDesc[cmdCounter].param[5].bypass) 	swing = (int)cmdParamDesc[cmdCounter].param[5].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 6;
	cmdParamDesc[cmdCounter].param[0].value[0] = pos;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "pos"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = len;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "len"); 
	cmdParamDesc[cmdCounter].param[2].value[0] = repeat;
	cmdParamDesc[cmdCounter].param[2].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[2].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[2].name, "repeat"); 
	cmdParamDesc[cmdCounter].param[3].value[0] = bpmScale;
	cmdParamDesc[cmdCounter].param[3].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[3].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[3].name, "bpmScale"); 
	cmdParamDesc[cmdCounter].param[4].value[0] = mode;
	cmdParamDesc[cmdCounter].param[4].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[4].type, "bool"); 
	strcpy(cmdParamDesc[cmdCounter].param[4].name, "mode"); 
	cmdParamDesc[cmdCounter].param[5].value[0] = swing;
	cmdParamDesc[cmdCounter].param[5].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[5].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[5].name, "swing"); 

		editor::paramEdit::setBypass();

		editor::paramEdit::setParamsAttr();

}

AddToUI(__FUNCTION__);
cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
cmdLevel++;

#define Clip(pos, len, repeat, bpmScale, mode, swing) Clip( __FILE__, __LINE__ , pos, len, repeat, bpmScale, mode, swing)

#else

#define Clip(pos, len, repeat, bpmScale, mode, swing) Clip( pos, len, repeat, bpmScale, mode, swing)

#endif

cmdCounter++;

