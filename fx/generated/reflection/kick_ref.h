#if REFLECTION

if (paramsAreLoaded) {
	if (!cmdParamDesc[cmdCounter].param[0].bypass) 	vol = (volume)cmdParamDesc[cmdCounter].param[0].value[0];
	if (!cmdParamDesc[cmdCounter].param[1].bypass) 	pan = (panorama)cmdParamDesc[cmdCounter].param[1].value[0];
	if (!cmdParamDesc[cmdCounter].param[2].bypass) 	send = (volume)cmdParamDesc[cmdCounter].param[2].value[0];
	if (!cmdParamDesc[cmdCounter].param[3].bypass)  solo = (switcher)cmdParamDesc[cmdCounter].param[3].value[0];
	if (!cmdParamDesc[cmdCounter].param[4].bypass)  mute = (switcher)cmdParamDesc[cmdCounter].param[4].value[0];
} else {
	strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
	cmdParamDesc[cmdCounter].caller.line = srcLine;
	cmdParamDesc[cmdCounter].pCount = 5;
	cmdParamDesc[cmdCounter].param[0].value[0] = vol;
	cmdParamDesc[cmdCounter].param[0].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[0].type, "volume"); 
	strcpy(cmdParamDesc[cmdCounter].param[0].name, "vol"); 
	cmdParamDesc[cmdCounter].param[1].value[0] = pan;
	cmdParamDesc[cmdCounter].param[1].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[1].type, "panorama"); 
	strcpy(cmdParamDesc[cmdCounter].param[1].name, "pan"); 
	cmdParamDesc[cmdCounter].param[2].value[0] = send;
	cmdParamDesc[cmdCounter].param[2].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[2].type, "volume"); 
	strcpy(cmdParamDesc[cmdCounter].param[2].name, "send"); 
	cmdParamDesc[cmdCounter].param[3].value[0] = (int)solo;
	cmdParamDesc[cmdCounter].param[3].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[3].type, "switcher"); 
	strcpy(cmdParamDesc[cmdCounter].param[3].name, "solo"); 
	cmdParamDesc[cmdCounter].param[4].value[0] = (int)mute;
	cmdParamDesc[cmdCounter].param[4].bypass = false;
	strcpy(cmdParamDesc[cmdCounter].param[4].type, "switcher"); 
	strcpy(cmdParamDesc[cmdCounter].param[4].name, "mute"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
}

AddToUI(__FUNCTION__);
cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
cmdLevel++;

#define kick(vol, pan, send, solo, mute) kick( __FILE__, __LINE__ , vol, pan, send, solo, mute)

#else

#define kick(vol, pan, send, solo, mute) kick( vol, pan, send, solo, mute)

#endif

cmdCounter++;

