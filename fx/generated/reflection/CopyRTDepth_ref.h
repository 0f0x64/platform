#if REFLECTION

if (paramsAreLoaded) {
	if (!cmdParamDesc[cmdCounter].param[0].bypass)  dst = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
	if (!cmdParamDesc[cmdCounter].param[1].bypass)  src = (texture)cmdParamDesc[cmdCounter].param[1].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)dst;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "dst"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = (int)src;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "src"); 

		editor::paramEdit::setBypass();

}

AddToUI(__FUNCTION__);
cmdCounter++;

cmdLevel++;

#define CopyRTDepth(dst, src) CopyRTDepth( __FILE__, __LINE__ , dst, src)

#endif