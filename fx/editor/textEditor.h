namespace paramEdit {

void insertNumber(int p)
{
	if (cmdParamDesc[currentCmd].param[currentParam].bypass) return;
	auto sType = cmdParamDesc[currentCmd].param[currentParam].typeIndex;

	if (isTypeEnum(sType)) return;

	if (p == '0' && cursorPos == 0) return;
	if (p == VK_OEM_MINUS && cursorPos != 0) return;

	char vstr[_CVTBUFSIZE];
	char vstr2[_CVTBUFSIZE];

	_itoa(cmdParamDesc[currentCmd].param[currentParam].value, vstr, 10);

	strcpy(vstr2, vstr);
	vstr2[cursorPos] = p == VK_OEM_MINUS ? '-' : (unsigned char)p;
	vstr2[cursorPos + 1] = 0;
	strncat(vstr2 + cursorPos + 1, vstr + cursorPos, strlen(vstr) - cursorPos);

	cmdParamDesc[currentCmd].param[currentParam].value = atoi(vstr2);
	pLimits();
	cursorPos++;

}

void Delete()
{
	auto sType = cmdParamDesc[currentCmd].param[currentParam].typeIndex;
	if (isTypeEnum(sType)) return;

	char vstr[_CVTBUFSIZE];
	char vstr2[_CVTBUFSIZE];

	_itoa(cmdParamDesc[currentCmd].param[currentParam].value, vstr, 10);

	if (cursorPos == strlen(vstr))
	{
		return;
	}

	strcpy(vstr2, vstr);
	strncpy(vstr2 + cursorPos, vstr + cursorPos + 1, strlen(vstr) - cursorPos);

	cmdParamDesc[currentCmd].param[currentParam].value = atoi(vstr2);
	pLimits();

}

void BackSpace()
{
	auto sType = cmdParamDesc[currentCmd].param[currentParam].typeIndex;
	if (isTypeEnum(sType)) return;
	if (cursorPos == 0) return;

	char vstr[_CVTBUFSIZE];
	char vstr2[_CVTBUFSIZE];

	_itoa(cmdParamDesc[currentCmd].param[currentParam].value, vstr, 10);
	strcpy(vstr2, vstr);
	strncpy(vstr2 + max(cursorPos - 1, 0), vstr + cursorPos, max(strlen(vstr) - cursorPos + 1, 0));

	cmdParamDesc[currentCmd].param[currentParam].value = atoi(vstr2);
	pLimits();

	cursorPos--;
}

void Wheel(float delta)
{
	if (uiContext != uiContext_::stack) return;
	if (currentCmd == -1) return;
	if (currentParam == -1) return;

	if (cmdParamDesc[currentCmd].param[currentParam].bypass) return;
	auto sType = cmdParamDesc[currentCmd].param[currentParam].typeIndex;

	if (isTypeEnum(sType)) return;

	float scale = ui::lbDown ? 10.f : 1.f;

	cmdParamDesc[currentCmd].param[currentParam].value += (int)(sign(delta) * scale);
	pLimits();

}
}