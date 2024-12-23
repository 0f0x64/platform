#if REFLECTION

if (paramsAreLoaded) {
	if (!cmdParamDesc[cmdCounter].param[0].bypass) 	bbox.x = cmdParamDesc[cmdCounter].param[0].value[0];
	bbox.y = cmdParamDesc[cmdCounter].param[0].value[1];
	bbox.z = cmdParamDesc[cmdCounter].param[0].value[2];
	bbox.w = cmdParamDesc[cmdCounter].param[0].value[3];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 1;
	cmdParamDesc[cmdCounter].param[0].value[0] = bbox.x;
	cmdParamDesc[cmdCounter].param[0].value[1] = bbox.y;
	cmdParamDesc[cmdCounter].param[0].value[2] = bbox.z;
	cmdParamDesc[cmdCounter].param[0].value[3] = bbox.w;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "rect"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "bbox"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
}

AddToUI(__FUNCTION__);
cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
cmdLevel++;

#define SetScissors(bbox_x, bbox_y, bbox_z, bbox_w) SetScissors( __FILE__, __LINE__ , rect {bbox_x, bbox_y, bbox_z, bbox_w })

#else

#define SetScissors(bbox_x, bbox_y, bbox_z, bbox_w) SetScissors( rect {bbox_x, bbox_y, bbox_z, bbox_w })

#endif

cmdCounter++;

