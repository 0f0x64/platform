#if REFLECTION

if (paramsAreLoaded) {
	if (!cmdParamDesc[cmdCounter].param[0].bypass) 	camTime = (timestamp)cmdParamDesc[cmdCounter].param[0].value[0];
	if (!cmdParamDesc[cmdCounter].param[1].bypass)  camType = (keyType)cmdParamDesc[cmdCounter].param[1].value[0];
	if (!cmdParamDesc[cmdCounter].param[2].bypass) 
		{
			eye.x = cmdParamDesc[cmdCounter].param[2].value[0];
			eye.y = cmdParamDesc[cmdCounter].param[2].value[1];
			eye.z = cmdParamDesc[cmdCounter].param[2].value[2];
		}
	if (!cmdParamDesc[cmdCounter].param[3].bypass) 
		{
			at.x = cmdParamDesc[cmdCounter].param[3].value[0];
			at.y = cmdParamDesc[cmdCounter].param[3].value[1];
			at.z = cmdParamDesc[cmdCounter].param[3].value[2];
		}
	if (!cmdParamDesc[cmdCounter].param[4].bypass) 
		{
			up.x = cmdParamDesc[cmdCounter].param[4].value[0];
			up.y = cmdParamDesc[cmdCounter].param[4].value[1];
			up.z = cmdParamDesc[cmdCounter].param[4].value[2];
		}
	if (!cmdParamDesc[cmdCounter].param[5].bypass) 	angle = (int)cmdParamDesc[cmdCounter].param[5].value[0];
	if (!cmdParamDesc[cmdCounter].param[6].bypass)  sType = (sliderType)cmdParamDesc[cmdCounter].param[6].value[0];
	if (!cmdParamDesc[cmdCounter].param[7].bypass) 
		{
			slide.x = cmdParamDesc[cmdCounter].param[7].value[0];
			slide.y = cmdParamDesc[cmdCounter].param[7].value[1];
			slide.z = cmdParamDesc[cmdCounter].param[7].value[2];
		}
	if (!cmdParamDesc[cmdCounter].param[8].bypass)  axisType = (camAxis)cmdParamDesc[cmdCounter].param[8].value[0];
	if (!cmdParamDesc[cmdCounter].param[9].bypass) 
		{
			fly.x = cmdParamDesc[cmdCounter].param[9].value[0];
			fly.y = cmdParamDesc[cmdCounter].param[9].value[1];
			fly.z = cmdParamDesc[cmdCounter].param[9].value[2];
		}
	if (!cmdParamDesc[cmdCounter].param[10].bypass) 	jitter = (int)cmdParamDesc[cmdCounter].param[10].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 11;
	cmdParamDesc[cmdCounter].param[0].value[0] = camTime;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "timestamp"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "camTime"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = (int)camType;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "keyType"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "camType"); 
	cmdParamDesc[cmdCounter].param[2].value[0] = eye.x;
	cmdParamDesc[cmdCounter].param[2].value[1] = eye.y;
	cmdParamDesc[cmdCounter].param[2].value[2] = eye.z;
	cmdParamDesc[cmdCounter].param[2].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[2].type, "position"); 
	strcpy(cmdParamDesc[cmdCounter].param[2].name, "eye"); 
	cmdParamDesc[cmdCounter].param[3].value[0] = at.x;
	cmdParamDesc[cmdCounter].param[3].value[1] = at.y;
	cmdParamDesc[cmdCounter].param[3].value[2] = at.z;
	cmdParamDesc[cmdCounter].param[3].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[3].type, "position"); 
	strcpy(cmdParamDesc[cmdCounter].param[3].name, "at"); 
	cmdParamDesc[cmdCounter].param[4].value[0] = up.x;
	cmdParamDesc[cmdCounter].param[4].value[1] = up.y;
	cmdParamDesc[cmdCounter].param[4].value[2] = up.z;
	cmdParamDesc[cmdCounter].param[4].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[4].type, "position"); 
	strcpy(cmdParamDesc[cmdCounter].param[4].name, "up"); 
	cmdParamDesc[cmdCounter].param[5].value[0] = angle;
	cmdParamDesc[cmdCounter].param[5].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[5].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[5].name, "angle"); 
	cmdParamDesc[cmdCounter].param[6].value[0] = (int)sType;
	cmdParamDesc[cmdCounter].param[6].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[6].type, "sliderType"); 
	strcpy(cmdParamDesc[cmdCounter].param[6].name, "sType"); 
	cmdParamDesc[cmdCounter].param[7].value[0] = slide.x;
	cmdParamDesc[cmdCounter].param[7].value[1] = slide.y;
	cmdParamDesc[cmdCounter].param[7].value[2] = slide.z;
	cmdParamDesc[cmdCounter].param[7].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[7].type, "position"); 
	strcpy(cmdParamDesc[cmdCounter].param[7].name, "slide"); 
	cmdParamDesc[cmdCounter].param[8].value[0] = (int)axisType;
	cmdParamDesc[cmdCounter].param[8].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[8].type, "camAxis"); 
	strcpy(cmdParamDesc[cmdCounter].param[8].name, "axisType"); 
	cmdParamDesc[cmdCounter].param[9].value[0] = fly.x;
	cmdParamDesc[cmdCounter].param[9].value[1] = fly.y;
	cmdParamDesc[cmdCounter].param[9].value[2] = fly.z;
	cmdParamDesc[cmdCounter].param[9].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[9].type, "rotation"); 
	strcpy(cmdParamDesc[cmdCounter].param[9].name, "fly"); 
	cmdParamDesc[cmdCounter].param[10].value[0] = jitter;
	cmdParamDesc[cmdCounter].param[10].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[10].type, "int"); 
	strcpy(cmdParamDesc[cmdCounter].param[10].name, "jitter"); 

		editor::paramEdit::setBypass();

		editor::paramEdit::setParamsAttr();

}

AddToUI(__FUNCTION__);
cmdCounter++;

cmdLevel++;

#define setCamKey(camTime, camType, eye_x, eye_y, eye_z, at_x, at_y, at_z, up_x, up_y, up_z, angle, sType, slide_x, slide_y, slide_z, axisType, fly_x, fly_y, fly_z, jitter) setCamKey( __FILE__, __LINE__ , camTime, camType, position {eye_x, eye_y, eye_z }, position {at_x, at_y, at_z }, position {up_x, up_y, up_z }, angle, sType, position {slide_x, slide_y, slide_z }, axisType, rotation {fly_x, fly_y, fly_z }, jitter)

#endif