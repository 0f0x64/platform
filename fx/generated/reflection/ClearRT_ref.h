#if REFLECTION

if (paramsAreLoaded) {
	if (!cmdParamDesc[cmdCounter].param[0].bypass) 	color.x = cmdParamDesc[cmdCounter].param[0].value[0];
	color.y = cmdParamDesc[cmdCounter].param[0].value[1];
	color.z = cmdParamDesc[cmdCounter].param[0].value[2];
	color.w = cmdParamDesc[cmdCounter].param[0].value[3];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = color.x;
	cmdParamDesc[cmdCounter].param[0].value[1] = color.y;
	cmdParamDesc[cmdCounter].param[0].value[2] = color.z;
	cmdParamDesc[cmdCounter].param[0].value[3] = color.w;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "color4"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "color"); 

		editor::paramEdit::setBypass();

		editor::paramEdit::setParamsAttr();

}

AddToUI(__FUNCTION__);
cmdCounter++;

cmdLevel++;

#define ClearRT(color_x, color_y, color_z, color_w) ClearRT( __FILE__, __LINE__ , color4 {color_x, color_y, color_z, color_w })

#endif