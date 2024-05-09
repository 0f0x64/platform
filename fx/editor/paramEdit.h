namespace paramEdit {

	float yPos=0.05;
	float yPosLast;

	float  storedParam = 0;
	int currentParam = 0;

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
	float inside;
	float x;
	float y;

	void showBoxes()
	{
		ui::Box::Setup();

		for (int i = 0; i < cmdCounter; i++)
		{
			auto w = ui::Text::getTextLen(cmdParamDesc[i].funcName, ui::style::text::width) + inside * 2.f;

			if (isMouseOver(x, y, w, ui::style::box::height))
			{
				ui::style::box::outlineBrightness = 1.f;

				if (ui::lbDown && i != currentCmd) {
					currentCmd = i;
					currentParam = -1;
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
	
	void showLabels()
	{
		for (int i = 0; i < cmdCounter; i++)
		{
			float sel = currentCmd == i ? 1.f : 0.f;
			ui::style::text::r = lerp(1.f, .0f, sel);
			ui::style::text::g = lerp(1.f, .0f, sel);
			ui::style::text::b = lerp(1.f, .0f, sel);

			ui::Text::Draw(cmdParamDesc[i].funcName, x + inside, y + .15f * ui::style::box::height);
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
			isType(t1, "topology_") || 
			isType(t1, "blendmode_") || 
			isType(t1, "blendop_") || 
			isType(t1, "depthmode_") ||
			isType(t1, "filter_") || 
			isType(t1, "addr_") ||
			isType(t1, "cullmode_") ||
			isType(t1, "targetshader_")
			) 
		{
			return true;
		}

		return false;
	}


	//create c++ call string and save it into source file
	void Save()
	{
		char str[1024];
		
		strcpy(str, cmdParamDesc[currentCmd].funcName);
		strcat(str, "(");

		for (int i = 0; i < cmdParamDesc[currentCmd].pCount; i++)
		{
			auto sType = cmdParamDesc[currentCmd].paramType[i];

			if (isType(sType, "int"))
			{
				char pStr[32];
				_itoa((int)cmdParamDesc[currentCmd].params[i][0], pStr, 10);
				strcat(str, pStr);
			}

			if (isType(sType, "float"))
			{
				char pStr[32];
				_gcvt(cmdParamDesc[currentCmd].params[i][0], 10, pStr);
				strcat(str, pStr);
			}

			if (isType(sType, "texture_"))
			{
				auto a = Textures::nameList[(int)cmdParamDesc[currentCmd].params[i][0]];
				strcat(str, cmdParamDesc[currentCmd].paramType[i]);
				str[strlen(str) - 1] = 0;
				strcat(str, "::");
				strcat(str, a);
			}

			if (isTypeEnum(sType))
			{
				strcat(str, cmdParamDesc[currentCmd].paramType[i]);
				str[strlen(str) - 1] = 0;
				strcat(str, "::");
				strcat(str, getEnumStr(sType, (int)cmdParamDesc[currentCmd].params[i][0]));
			}


			if (isType(sType, "position_") == 0 ||
				isType(sType, "size_") == 0 ||
				isType(sType, "rotation_") == 0 || 
				isType(sType, "color_") == 0 )
			{
				char pStr[32];
				for (int x = 0; x < 3; x++)
				{
					_gcvt(cmdParamDesc[currentCmd].params[i][x], 10, pStr);
					strcat(str, pStr);
					if (x!=2) strcat(str, ", ");
				}
			}

			if (isType(sType, "color4_") == 0 ||
				isType(sType, "rect_") == 0 )
			{
				char pStr[32];
				for (int x = 0; x < 4; x++)
				{
					_gcvt(cmdParamDesc[currentCmd].params[i][x], 10, pStr);
					strcat(str, pStr);
					if (x != 2) strcat(str, ", ");
				}
			}

			if (i!= cmdParamDesc[currentCmd].pCount -1) strcat(str, ", ");
		}
		strcat(str, ");");

		//
	}


	void showParams()
	{
		float valueDrawOffset = .1f;

		for (int i = 0; i < cmdParamDesc[currentCmd].pCount; i++)
		{
			auto w = ui::Text::getTextLen(cmdParamDesc[currentCmd].paramName[i], ui::style::text::width);
			float y = yPos + float(currentCmd + i) * lead;

			float sel = currentParam == i ? 1.f : 0.f;
			ui::style::text::r = lerp(.5, 1, sel);
			ui::style::text::g = lerp(.5, 1, sel);
			ui::style::text::b = lerp(.5, 1, sel);

			ui::Text::Draw(cmdParamDesc[currentCmd].paramName[i], x, yPos + float(currentCmd + i) * lead);

			auto sType = cmdParamDesc[currentCmd].paramType[i];

			if (isType(sType, "int") || isType(sType, "float"))
			{
				char val[32];
				_gcvt(cmdParamDesc[currentCmd].params[i][0], 5, val);
				ui::Text::Draw(val, x + valueDrawOffset, yPos + float(currentCmd + i) * lead);
			}

			if (isType(sType, "position_") || isType(sType, "size"))
			{
				char val[32];
				char vstr[132];
				_gcvt(cmdParamDesc[currentCmd].params[i][0], 5, val);
				strcpy(vstr, val);
				_gcvt(cmdParamDesc[currentCmd].params[i][1], 5, val);
				strcat(vstr, " : "); strcat(vstr, val);
				_gcvt(cmdParamDesc[currentCmd].params[i][2], 5, val);
				strcat(vstr, " : "); strcat(vstr, val);

				ui::Text::Draw(vstr, x + valueDrawOffset, yPos + float(currentCmd + i) * lead);
			}
		}
	}

	/*
	void showParams()
	{

		for (int i = 0; i < cmdParamDesc[currentCmd].pCount; i++)
		{
			auto w = ui::Text::getTextLen(cmdParamDesc[currentCmd].paramName[i], ui::style::text::width);

			float y = yPos + float(currentCmd + i) * lead;

			if (i==currentParam && !ui::lbDown && saveMe)
			{
				Save();
				saveMe = false;
			}

			if (isMouseOver(x, y, w, ui::style::box::height))
			{
				if (ui::lbDown && i != currentParam)
				{
					currentParam = i;
					storedParam = cmdParamDesc[currentCmd].params[currentParam][0];
				}
			}

			float sel = currentParam == i ? 1.f : 0.f;
			ui::style::text::r = lerp(.5, 1, sel);
			ui::style::text::g = lerp(.5, 1, sel);
			ui::style::text::b = lerp(.5, 1, sel);

			ui::Text::Draw(cmdParamDesc[currentCmd].paramName[i], x, yPos + float(currentCmd + i) * lead);


			if ((strcmp(cmdParamDesc[currentCmd].paramType[i], "int") == 0) ||
				(strcmp(cmdParamDesc[currentCmd].paramType[i], "float") == 0))
			{
				char val[32];
				_gcvt(cmdParamDesc[currentCmd].params[i][0], 5, val);
				ui::Text::Draw(val, x + .22f, yPos + float(currentCmd + i) * lead);
			}

			if ((strcmp(cmdParamDesc[currentCmd].paramType[i], "position_") == 0) ||
				(strcmp(cmdParamDesc[currentCmd].paramType[i], "size") == 0))
			{

				if (i == currentParam)
				{
					if (ui::lbDown)
					{
						cmdParamDesc[currentCmd].params[i][0] = storedParam + ui::mouseDelta.x;
						saveMe = true;
					}
					else
					{
						storedParam = cmdParamDesc[currentCmd].params[currentParam][0];
					}
				}

				char val[32];
				char vstr[132];
				_gcvt(cmdParamDesc[currentCmd].params[i][0], 5, val);
				strcpy(vstr, val);
				_gcvt(cmdParamDesc[currentCmd].params[i][1], 5, val);
				strcat(vstr, " : "); strcat(vstr, val);
				_gcvt(cmdParamDesc[currentCmd].params[i][2], 5, val);
				strcat(vstr, " : "); strcat(vstr, val);

				ui::Text::Draw(vstr, x + .22f, yPos + float(currentCmd + i) * lead);
			}
		}
	}
	*/
	void ShowStack()
	{
		if (ui::rbDown)
		{
			yPos = yPosLast + editor::ui::mouseDelta.y;
		}

		ui::style::Base();

		top = ui::style::text::height;
		bottom = 1.f - ui::style::text::height*2.5f;
		lead = ui::style::text::height;
		inside = ui::style::text::width * .5f * aspect / 2.f;
		yPos = max(yPos, top - cmdCounter * lead + lead);
		yPos = min(yPos, bottom - lead);

		gapi.setScissors(rect_{ 0, top, 1, bottom });

		x = ui::style::text::width / 2.f;
		y = yPos;
		showBoxes();
		y = yPos;
		ui::Text::Setup();
		showLabels();
		ui::style::Base();
		x += .1;
		showParams();

		gapi.setScissors(rect_{ 0, 0, 1, 1 });
	}

}
