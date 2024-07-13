#if REFLECTION

if (paramsAreLoaded) {
	if (!cmdParamDesc[cmdCounter].param[0].bypass)  geometry = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
	if (!cmdParamDesc[cmdCounter].param[1].bypass)  normals = (texture)cmdParamDesc[cmdCounter].param[1].value[0];
	if (!cmdParamDesc[cmdCounter].param[2].bypass) 	quality = (unsigned int)cmdParamDesc[cmdCounter].param[2].value[0];
	if (!cmdParamDesc[cmdCounter].param[3].bypass) 
		{
			pos.x = cmdParamDesc[cmdCounter].param[3].value[0];
			pos.y = cmdParamDesc[cmdCounter].param[3].value[1];
			pos.z = cmdParamDesc[cmdCounter].param[3].value[2];
		}
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 4;
	cmdParamDesc[cmdCounter].param[0].value[0] = (int)geometry;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "geometry"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = (int)normals;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "texture"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "normals"); 
	cmdParamDesc[cmdCounter].param[2].value[0] = quality;
	cmdParamDesc[cmdCounter].param[2].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[2].type, "unsigned int"); 
	strcpy(cmdParamDesc[cmdCounter].param[2].name, "quality"); 
	cmdParamDesc[cmdCounter].param[3].value[0] = pos.x;
	cmdParamDesc[cmdCounter].param[3].value[1] = pos.y;
	cmdParamDesc[cmdCounter].param[3].value[2] = pos.z;
	cmdParamDesc[cmdCounter].param[3].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[3].type, "position"); 
	strcpy(cmdParamDesc[cmdCounter].param[3].name, "pos"); 

		editor::paramEdit::setBypass();

		editor::paramEdit::setParamsAttr();

}

AddToUI(__FUNCTION__);
cmdCounter++;

cmdLevel++;

#define ShowObject(geometry, normals, quality, pos_x, pos_y, pos_z) ShowObject( __FILE__, __LINE__ , geometry, normals, quality, position {pos_x, pos_y, pos_z })

#endif