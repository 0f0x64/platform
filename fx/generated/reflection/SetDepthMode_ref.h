#if REFLECTION

if (paramsAreLoaded) {
	if (!cmdParamDesc[cmdCounter].param[0].bypass)  mode = (depthmode)cmdParamDesc[cmdCounter].param[0].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)mode;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "depthmode"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "mode"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
}

AddToUI(__FUNCTION__);
cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
cmdLevel++;

#define SetDepthMode(mode) SetDepthMode( __FILE__, __LINE__ , mode)

#else

#define SetDepthMode(mode) SetDepthMode( mode)

#endif

cmdCounter++;

