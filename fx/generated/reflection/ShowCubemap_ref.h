#if REFLECTION

if (paramsAreLoaded) {
	if (!cmdParamDesc[cmdCounter].param[0].bypass)  envTexture = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)envTexture;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "envTexture"); 

		editor::paramEdit::setBypass();

		editor::paramEdit::setParamsAttr();

}

AddToUI(__FUNCTION__);
cmdCounter++;

cmdLevel++;

#define ShowCubemap(envTexture) ShowCubemap( __FILE__, __LINE__ , envTexture)

#endif