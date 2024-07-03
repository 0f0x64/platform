#if REFLECTION

if (paramsAreLoaded) {
	r.x = cmdParamDesc[cmdCounter].param[0].value[0];
	r.y = cmdParamDesc[cmdCounter].param[0].value[1];
	r.z = cmdParamDesc[cmdCounter].param[0].value[2];
	r.w = cmdParamDesc[cmdCounter].param[0].value[3];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = r.x;
	cmdParamDesc[cmdCounter].param[0].value[1] = r.y;
	cmdParamDesc[cmdCounter].param[0].value[2] = r.z;
	cmdParamDesc[cmdCounter].param[0].value[3] = r.w;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "rect"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "r"); 
}

AddToUI(__FUNCTION__);
cmdCounter++;

cmdLevel++;

#define SetScissors(r_x, r_y, r_z, r_w) SetScissors( __FILE__, __LINE__ , rect {r_x, r_y, r_z, r_w })

#endif