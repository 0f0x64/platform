#if REFLECTION

if (paramsAreLoaded) {
	targetGeo = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
	targetNrml = (texture)cmdParamDesc[cmdCounter].param[1].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 2;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)targetGeo;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "targetGeo"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = (int)targetNrml;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "targetNrml"); 
}

AddToUI(__FUNCTION__);
cmdCounter++;

cmdLevel++;

#define CalcObject(targetGeo, targetNrml) CalcObject( __FILE__, __LINE__ , targetGeo, targetNrml)

#endif