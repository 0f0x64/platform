#if REFLECTION

if (paramsAreLoaded) {
	if (!cmdParamDesc[cmdCounter].param[0].bypass) 
		{
			eye.x = cmdParamDesc[cmdCounter].param[0].value[0];
			eye.y = cmdParamDesc[cmdCounter].param[0].value[1];
			eye.z = cmdParamDesc[cmdCounter].param[0].value[2];
		}
	if (!cmdParamDesc[cmdCounter].param[1].bypass) 
		{
			at.x = cmdParamDesc[cmdCounter].param[1].value[0];
			at.y = cmdParamDesc[cmdCounter].param[1].value[1];
			at.z = cmdParamDesc[cmdCounter].param[1].value[2];
		}
	if (!cmdParamDesc[cmdCounter].param[2].bypass) 
		{
			up.x = cmdParamDesc[cmdCounter].param[2].value[0];
			up.y = cmdParamDesc[cmdCounter].param[2].value[1];
			up.z = cmdParamDesc[cmdCounter].param[2].value[2];
		}
	if (!cmdParamDesc[cmdCounter].param[3].bypass) 	angle = (unsigned int)cmdParamDesc[cmdCounter].param[3].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 4;
	cmdParamDesc[cmdCounter].param[0].value[0] = eye.x;
	cmdParamDesc[cmdCounter].param[0].value[1] = eye.y;
	cmdParamDesc[cmdCounter].param[0].value[2] = eye.z;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "position"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "eye"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = at.x;
	cmdParamDesc[cmdCounter].param[1].value[1] = at.y;
	cmdParamDesc[cmdCounter].param[1].value[2] = at.z;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "position"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "at"); 
	cmdParamDesc[cmdCounter].param[2].value[0] = up.x;
	cmdParamDesc[cmdCounter].param[2].value[1] = up.y;
	cmdParamDesc[cmdCounter].param[2].value[2] = up.z;
	cmdParamDesc[cmdCounter].param[2].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[2].type, "position"); 
	strcpy(cmdParamDesc[cmdCounter].param[2].name, "up"); 
	cmdParamDesc[cmdCounter].param[3].value[0] = angle;
	cmdParamDesc[cmdCounter].param[3].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[3].type, "unsigned int"); 
	strcpy(cmdParamDesc[cmdCounter].param[3].name, "angle"); 

		editor::paramEdit::setBypass();

}

AddToUI(__FUNCTION__);
cmdCounter++;

cmdLevel++;

#define SetCamera(eye_x, eye_y, eye_z, at_x, at_y, at_z, up_x, up_y, up_z, angle) SetCamera( __FILE__, __LINE__ , position {eye_x, eye_y, eye_z }, position {at_x, at_y, at_z }, position {up_x, up_y, up_z }, angle)

#endif