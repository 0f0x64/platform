#if REFLECTION

if (paramsAreLoaded) {
	c.x = cmdParamDesc[cmdCounter].param[0].value[0];
	c.y = cmdParamDesc[cmdCounter].param[0].value[1];
	c.z = cmdParamDesc[cmdCounter].param[0].value[2];
	c.w = cmdParamDesc[cmdCounter].param[0].value[3];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = c.x;
	cmdParamDesc[cmdCounter].param[0].value[1] = c.y;
	cmdParamDesc[cmdCounter].param[0].value[2] = c.z;
	cmdParamDesc[cmdCounter].param[0].value[3] = c.w;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "color4"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "c"); 
}

AddToUI(__FUNCTION__);
cmdCounter++;

cmdLevel++;

#define ClearRT(c_x, c_y, c_z, c_w) ClearRT( __FILE__, __LINE__ , color4 {c_x, c_y, c_z, c_w })

#endif