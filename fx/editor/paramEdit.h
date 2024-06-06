namespace paramEdit {

	int floatConvertDigits = 5;

	float yPos=0.05;
	float yPosLast;

	int storedParam[4] = { 0,0,0,0 };
	int currentParam = -1;
	int subParam = -1;

	bool action = false;
	bool clickOnEmptyPlace;

	bool editContext = false;

	int cursorPos = 0;
	float cursorX = 0;
	float cursorY = 0;
	bool showCursor = false;

	void rbDown()
	{
		yPosLast = yPos;
	}

	bool saveMe = false;

	void lbDown()
	{
	//	storedParam = cmdParamDesc[currentCmd].params[currentParam][0];
	}

	bool isMouseOver(float x,float y, float w, float h)
	{
		if (editor::ui::mousePos.x > x && editor::ui::mousePos.x < x + w &&
			editor::ui::mousePos.y > y &&
			editor::ui::mousePos.y < y + h)
		{
			return true;
		}

		return false;
	}

	float top;
	float bottom;
	float lead;
	float insideX;
	float insideY;
	float x;
	float y;

	void showLabels()
	{
		for (int i = 0; i < cmdCounter; i++)
		{
			float sel = currentCmd == i ? 1.f : 0.f;
			ui::style::text::r = lerp(1.f, .0f, sel);
			ui::style::text::g = lerp(1.f, .0f, sel);
			ui::style::text::b = lerp(1.f, .0f, sel);

			ui::Text::Draw(cmdParamDesc[i].funcName, x + insideX, y + insideY);
			y += lead;
		}
	}

	bool isType(const char* t1, const char* t2 )
	{
		return (strcmp(t1, t2) == 0);
	}

	bool isTypeEnum(const char* t1)
	{
		if (
			isType(t1, "texture") ||
			isType(t1, "topology") || 
			isType(t1, "blendmode") || 
			isType(t1, "blendop") || 
			isType(t1, "depthmode") ||
			isType(t1, "filter") || 
			isType(t1, "addr") ||
			isType(t1, "cullmode") ||
			isType(t1, "targetshader")
			) 
		{
			return true;
		}

		return false;
	}

	int getTypeDim(const char* t1)
	{
		if (
			isType(t1, "position") ||
			isType(t1, "size") ||
			isType(t1, "rotation") ||
			isType(t1, "color")
			)
		{
			return 3;
		} 

		if (
			isType(t1, "color4") ||
			isType(t1, "rect") 
			)
		{
			return 4;
		}

		return 1;
	}

	void showBoxes()
	{
		ui::Box::Setup();

		for (int i = 0; i < cmdCounter; i++)
		{
			auto w = ui::Text::getTextLen(cmdParamDesc[i].funcName, ui::style::text::width) + insideX * 2.f;

			if (isMouseOver(x, y, w, ui::style::box::height) && editContext)
			{
				ui::style::box::outlineBrightness = 1.f;

				clickOnEmptyPlace = false;

				if (ui::lbDown && i != currentCmd) {
					currentCmd = i;
					currentParam = -1;
					subParam = -1;

					if (ui::dblClk)
					{
						for (int j = 0; j < cmdParamDesc[currentCmd].pCount; j++)
						{
							if (isType(cmdParamDesc[currentCmd].param[j].type, "position"))
							{
								auto x = cmdParamDesc[currentCmd].param[j].value[0];
								auto y = cmdParamDesc[currentCmd].param[j].value[1];
								auto z = cmdParamDesc[currentCmd].param[j].value[2];
								ViewCam::TransCam(-x/ intToFloatDenom, -y/intToFloatDenom, -z/ intToFloatDenom);
							}
						}
						ui::dblClk = false;
					}

				}
			}
			else
			{
				ui::style::box::outlineBrightness = 0.1f;
			}

			float sel = currentCmd == i ? 1.f : 0.f;
			ui::style::box::r = lerp(0.2f, .8f, sel);
			ui::style::box::g = lerp(0.2f, .8f, sel);
			ui::style::box::b = lerp(0.2f, .8f, sel);

			ui::Box::Draw(x, y, w, ui::style::text::height * .8f);
			y += lead;
		}
	}

	void insertNumber(int p)
	{
		auto sType = cmdParamDesc[currentCmd].param[currentParam].type;
		if (isTypeEnum(sType)) return;

		if (p == '0' && cursorPos == 0) return;
		if (p == VK_OEM_MINUS && cursorPos != 0) return;

		char vstr[_CVTBUFSIZE];
		vstr[0] = 0;
		char vstr2[_CVTBUFSIZE];
		vstr2[0] = 0;

		_itoa(cmdParamDesc[currentCmd].param[currentParam].value[max(subParam, 0)], vstr, 10);

		strcpy(vstr2, vstr);
		vstr2[cursorPos] = p == VK_OEM_MINUS ? '-' : (unsigned char)p;
		vstr2[cursorPos+1] = 0;
		strncat(vstr2 + cursorPos + 1, vstr + cursorPos, strlen(vstr) - cursorPos);

		cmdParamDesc[currentCmd].param[currentParam].value[max(subParam, 0)] = atoi(vstr2);

		cursorPos++;

	}

	void Delete()
	{
		auto sType = cmdParamDesc[currentCmd].param[currentParam].type;
		if (isTypeEnum(sType)) return;

		char vstr[_CVTBUFSIZE];
		vstr[0] = 0;
		char vstr2[_CVTBUFSIZE];
		vstr2[0] = 0;

		_itoa(cmdParamDesc[currentCmd].param[currentParam].value[max(subParam, 0)], vstr, 10);

		if (cursorPos == strlen(vstr))
		{
			return;
		}

		strcpy(vstr2, vstr);
		strncpy(vstr2 + cursorPos, vstr + cursorPos + 1, strlen(vstr) - cursorPos);

		cmdParamDesc[currentCmd].param[currentParam].value[max(subParam, 0)] = atoi(vstr2);

	}

	void BackSpace()
	{
		auto sType = cmdParamDesc[currentCmd].param[currentParam].type;
		if (isTypeEnum(sType)) return;
		if (cursorPos == 0) return;

		char vstr[_CVTBUFSIZE];
		vstr[0] = 0;
		char vstr2[_CVTBUFSIZE];
		vstr2[0] = 0;

		_itoa(cmdParamDesc[currentCmd].param[currentParam].value[max(subParam, 0)], vstr, 10);

		strcpy(vstr2, vstr);
		strncpy(vstr2 + max(cursorPos - 1, 0), vstr + cursorPos, max(strlen(vstr) - cursorPos + 1,0));

		cmdParamDesc[currentCmd].param[currentParam].value[max(subParam, 0)] = atoi(vstr2);

		cursorPos--;

	}

	void Wheel(float delta)
	{
		if (!editContext) return;
		if (currentCmd == -1) return;
		if (currentParam == -1) return;

		auto sType = cmdParamDesc[currentCmd].param[currentParam].type;

		if (isTypeEnum(sType)) return;

		float scale = ui::lbDown ? 10 : 1;

		cmdParamDesc[currentCmd].param[currentParam].value[max(subParam,0)] += sign(delta) * scale;

	}

	//create c++ call string and save it into source file
	void Save()
	{
		char str[1024];
		
		strcpy(str, cmdParamDesc[currentCmd].funcName);
		strcat(str, "(");

		for (int i = 0; i < cmdParamDesc[currentCmd].pCount; i++)
		{
			auto sType = cmdParamDesc[currentCmd].param[i].type;

			if (isType(sType, "int") || isType(sType, "signed int") || isType(sType, "unsigned int"))
			{
				char pStr[32];
				_itoa((int)cmdParamDesc[currentCmd].param[i].value[0], pStr, 10);
				strcat(str, pStr);
			}

			if (isTypeEnum(sType))
			{
				strcat(str, cmdParamDesc[currentCmd].param[i].type);
				str[strlen(str) - 1] = 0;
				strcat(str, "::");
				strcat(str, getEnumStr(sType, (int)cmdParamDesc[currentCmd].param[i].value[0]));
			}

			if (isType(sType, "position") ||
				isType(sType, "size") ||
				isType(sType, "rotation") || 
				isType(sType, "color") )
			{
				char pStr[32];
				for (int x = 0; x < 3; x++)
				{
					_itoa(cmdParamDesc[currentCmd].param[i].value[x], pStr, 10);
					strcat(str, pStr);
					if (x!=2) strcat(str, ", ");
				}
			}

			if (isType(sType, "color4") ||
				isType(sType, "rect") )
			{
				char pStr[32];
				for (int x = 0; x < 4; x++)
				{
					_itoa(cmdParamDesc[currentCmd].param[i].value[x], pStr, 10);
					strcat(str, pStr);
					if (x != 3) strcat(str, ", ");
				}
			}

			if (i!= cmdParamDesc[currentCmd].pCount -1) strcat(str, ", ");
		}

		strcat(str, ");");

		//
	}

	float valueDrawOffset = .1f;
	float enumDrawOffset = .2f;

	float enumPos = 0;

	void showParamBoxes()
	{
		ui::Box::Setup();

		float _x = x + valueDrawOffset - insideX;
		float __x = x + enumDrawOffset - insideX;

		float y = yPos + float(currentCmd) * lead;
		for (int i = 0; i < cmdParamDesc[currentCmd].pCount; i++)
		{

			if (cmdParamDesc[currentCmd].param[i].bypass) continue;

			auto w = .05f;
			auto sType = cmdParamDesc[currentCmd].param[i].type;
			int count = getTypeDim(sType);

			if (editContext)
			{

				if (count == 1)
				{
					ui::style::box::outlineBrightness = 0.1f;
					if (isMouseOver(_x, y, w, ui::style::box::height))
					{
						ui::style::box::outlineBrightness = 1.f;
						clickOnEmptyPlace = false;

						if (ui::lbDown && !action)
						{
							action = true;
							if (currentParam == i && isTypeEnum(sType))
							{
								currentParam = -1;
							}
							else
							{
								currentParam = i;
								cursorPos = 0;
								storedParam[0] = cmdParamDesc[currentCmd].param[currentParam].value[0];
								subParam = -1;
							}				
						}
					}
				}
				else
				{
					for (int p = 0; p < count; p++)
					{
						ui::style::box::outlineBrightness = 0.1f;

						if (isMouseOver(_x , y+ float(p)*lead, w, ui::style::box::height))
						{
							ui::style::box::outlineBrightness = 1.f;
							clickOnEmptyPlace = false;

							if (ui::lbDown && !action)
							{
								action = true;
								currentParam = i;
								subParam = p;
								cursorPos = 0;

								storedParam[p] = cmdParamDesc[currentCmd].param[currentParam].value[p];
							}
						}
					}

				}
			}

			float sel = currentParam == i ? 1.f : 0.f;
			ui::style::box::r = lerp(0.2f, .8f, sel);
			ui::style::box::g = lerp(0.2f, .8f, sel);
			ui::style::box::b = lerp(0.2f, .8f, sel);

			for (int p = 0; p < count; p++)
			{
				float sel = currentParam == i ? 1.f : 0.f;
				sel = sel * (float)(max(subParam,0) == p);
				
				bool mo = isMouseOver(_x , y+float(p)*lead, w, ui::style::box::height);
				
				ui::style::box::outlineBrightness = lerp(0.1f,1.f,mo);

				ui::style::box::r = lerp(0.2f, .8f, sel);
				ui::style::box::g = lerp(0.2f, .8f, sel);
				ui::style::box::b = lerp(0.2f, .8f, sel);


				ui::Box::Draw(_x, y+ float(p)*lead, w, ui::style::text::height * .8f);
			}
			
			if (sel && isTypeEnum(sType))
			{
				auto val = cmdParamDesc[currentCmd].param[i].value[0];
				
				for (int e = 0; e < getEnumCount(sType); e++)
				{
					float sel = e == val ? 1.f : 0.f;
					ui::style::box::r = .2f;
					ui::style::box::g = .2f;
					ui::style::box::b = .2f;

					auto over = isMouseOver(__x, y+e*lead, w, ui::style::box::height);

					ui::style::box::outlineBrightness = over ? 1.f : 0.1f;

					ui::Box::Draw(__x, y + e*lead, w, ui::style::text::height * .8f);

					if (over) { 
						clickOnEmptyPlace = false; 
					}

					if (ui::lbDown && (!action) && editContext)
					{
						
						if (over)
						{
							action = true;
							currentParam = -1;

							cmdParamDesc[currentCmd].param[i].value[0] = e;
						}
					}
				}
			}

			y += lead* count;
		}

		if (!isTypeEnum(cmdParamDesc[currentCmd].param[currentParam].type))
		{
			if (editContext && currentParam >= 0)
			{
				if (ui::mbDown)
				{
					cmdParamDesc[currentCmd].param[currentParam].value[max(subParam, 0)] = storedParam[max(subParam, 0)] - ui::mouseDelta.y * height;
				}
				else
				{
					storedParam[max(subParam, 0)] = cmdParamDesc[currentCmd].param[currentParam].value[max(subParam, 0)];
				}
			}
		}
	}

	void showParams()
	{
		float inCurPos = 0;

		showParamBoxes();

		ui::Text::Setup();

		showCursor = false;
		auto bw = .05f;

		float y = yPos + float(currentCmd) * lead;

		for (int i = 0; i < cmdParamDesc[currentCmd].pCount; i++)
		{
			if (cmdParamDesc[currentCmd].param[i].bypass) continue;

			char vstr[_CVTBUFSIZE];
			vstr[0] = 0;
			auto w = ui::Text::getTextLen(cmdParamDesc[currentCmd].param[i].name, ui::style::text::width);


			float sel = currentParam == i ? 1.f : 0.f;
			ui::style::text::r = lerp(.8f, 1.f, sel);
			ui::style::text::g = lerp(.8f, 1.f, sel);
			ui::style::text::b = lerp(.8f, 1.f, sel);

			ui::Text::Draw(cmdParamDesc[currentCmd].param[i].name, x+insideX, y + insideY);

			auto sType = cmdParamDesc[currentCmd].param[i].type;

			if (isType(sType, "int") || isType(sType, "signed int") || isType(sType, "unsigned int"))
			{
				_itoa(cmdParamDesc[currentCmd].param[i].value[0], vstr, 10);
			}
			
			int count = getTypeDim(sType);

			if (count > 1)
			{
				char val[_CVTBUFSIZE];
				for (int k = 0; k < count; k++)
				{
					float sel = currentParam == i ? 1.f : 0.f;
					sel = sel * (float)(subParam == k);
					ui::style::text::r = lerp(.8, .2, sel);
					ui::style::text::g = lerp(.8, .2, sel);
					ui::style::text::b = lerp(.8, .2, sel);

					_itoa(cmdParamDesc[currentCmd].param[i].value[k], val, 10);
					strcpy(vstr, val);
					ui::Text::Draw(vstr, x + valueDrawOffset, y + float(k) * lead + insideY);

					if (sel) {
						inCurPos = ui::Text::getTextLen(vstr, ui::style::text::width, cursorPos);
						cursorPos = clamp(cursorPos, 0, (int)strlen(vstr));
					}
				}
			}


			if (isTypeEnum(sType))
			{
				strcpy(vstr, getEnumStr(sType, (int)cmdParamDesc[currentCmd].param[i].value[0]));
			}

			ui::style::text::r = lerp(.8, .2, sel);
			ui::style::text::g = lerp(.8, .2, sel);
			ui::style::text::b = lerp(.8, .2, sel);

			if (count == 1)
			{
				ui::Text::Draw(vstr, x + valueDrawOffset, y + insideY);
				if (i == currentParam)
				{
					inCurPos = ui::Text::getTextLen(vstr, ui::style::text::width, cursorPos);
					cursorPos = clamp(cursorPos, 0, (int)strlen(vstr));
				}
			}

			if (sel && isTypeEnum(sType))
			{
				ui::style::text::r = .8f;
				ui::style::text::g = .8f;
				ui::style::text::b = .8f;

				for (int e = 0; e < getEnumCount(sType); e++)
				{
					strcpy(vstr, getEnumStr(sType, e));
					ui::Text::Draw(vstr, x + enumDrawOffset, yPos + float(currentCmd + i + e) * lead + insideY);
				}
			}


			if (editContext && currentParam == i && !isTypeEnum(sType))
			{
				showCursor = true;
				cursorX = x + valueDrawOffset + inCurPos;
				cursorY = y + insideY + max(subParam,0)*lead;
			}

			y += lead*count;
		}
	}

	void ShowStack()
	{

		clickOnEmptyPlace = true;

		if (ui::rbDown && editContext)
		{
			yPos = yPosLast + editor::ui::mouseDelta.y;
		}

		ui::style::Base();

		top = ui::style::text::height;
		bottom = 1.f - ui::style::text::height*2.5f;
		lead = ui::style::text::height;
		insideX = ui::style::text::width * .5f * aspect / 2.f;
		insideY = .15f * ui::style::box::height;
		yPos = max(yPos, top - cmdCounter * lead + lead);
		yPos = min(yPos, bottom - lead);

		gapi.setScissors(rect{ 0, (int)(top*dx11::height), dx11::width, (int)(bottom*dx11::height) });

		x = ui::style::text::width / 2.f;
		y = yPos;
		showBoxes();
		y = yPos;
		ui::Text::Setup();
		showLabels();
		ui::style::Base();
		x += .1;
		showParams();

		if (showCursor)
		{
			ui::Box::Setup();
			ui::style::box::width = 2. / width;
			ui::style::box::height = ui::style::text::height * .5f;
			ui::style::box::rounded = .0f;
			ui::style::box::soft = 10.1f;
			ui::style::box::edge = 0.f;
			ui::style::box::outlineBrightness = 0.1f;
			ui::style::box::r = 0;
			ui::style::box::g = 0;
			ui::style::box::b = 0;
			ui::style::box::a = .5f + .5f * sinf((float)timer::frameBeginTime * .01f);

			ui::Box::Draw(cursorX, cursorY);
		}

		gapi.setScissors(rect{ 0, 0, width, height });

/*		if (!action && ui::lbDown && clickOnEmptyPlace) {

			if (currentParam == -1) currentCmd = -1;
			currentParam = -1;
			
		}*/

		if (!action && ui::lbDown && clickOnEmptyPlace) {

		//	if (currentParam == -1) currentCmd = -1;
			currentParam = -1;

		}

	}

}
