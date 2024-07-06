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

	int pCountV = 0;
	float tabLen = 0.f;

	float vScroll = false;

	void rbDown()
	{
		yPosLast = yPos;
	}

	bool saveMe = false;

	void lbDown()
	{
/*		if (ui::dblClk)
		{
			//cmdLevel
			ui::dblClk = false;
		}*/

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
		for (int i = startCmd; i < cmdCounter; i++)
		{
			if (i > startCmd && curCmdLevel > cmdParamDesc[i].stackLevel) break;
			if (i > startCmd && curCmdLevel != cmdParamDesc[i].stackLevel) continue;


			float sel = currentCmd == i ? 1.f : 0.f;
			editor::ui::style::text::r = lerp(1.f, .0f, sel);
			editor::ui::style::text::g = lerp(1.f, .0f, sel);
			editor::ui::style::text::b = lerp(1.f, .0f, sel);

			editor::ui::Text::Draw(cmdParamDesc[i].funcName, x + insideX, y + insideY);
			y += lead;
		}
	}

	

	float selYpos = 0.5;

	void showBoxes()
	{
		ui::Box::Setup();
		auto c = curCmdLevel;
		for (int i = startCmd; i < cmdCounter; i++)
		{
			auto w = ui::Text::getTextLen(cmdParamDesc[i].funcName, ui::style::text::width) + insideX * 2.f;

			auto cl = cmdParamDesc[i].stackLevel;
			if (i > startCmd && curCmdLevel > cmdParamDesc[i].stackLevel) break;
			if (i > startCmd && curCmdLevel !=cmdParamDesc[i].stackLevel) continue;

			if (isMouseOver(x, y, w, ui::style::box::height) && editContext)
			{
				ui::style::box::outlineBrightness = 1.f;

				clickOnEmptyPlace = false;

				if (ui::dblClk && ui::lbDown)
				{
					if (i == startCmd) //close
					{
						curCmdLevel = max(cl , 0);

						for (int j = startCmd; j >= 0; j--)
						{
							if (cmdParamDesc[j].stackLevel < curCmdLevel  || j == 0) {
								startCmd = j; break;
							}
						}

						int cnt = 0;
						for (int j = startCmd; j < i; j++)
						{
							if (j == startCmd || curCmdLevel == cmdParamDesc[j].stackLevel) cnt++;
						}
						
						yPos -= float(cnt)* lead;
						//y = float(i - startCmd) * lead

					}
					else //open
					{
						bool haveContent = false;
						for (int j = i+1; j < cmdCounter; j++)
						{
							if (cl < cmdParamDesc[j].stackLevel)
							{
								haveContent = true;
								break;
							}

							if (cl >= cmdParamDesc[j].stackLevel)
							{
								break;
							}
						}

						if (haveContent)
						{
							startCmd = i;
							curCmdLevel = cl + 1;
							yPos = y;
						}
					}

					ui::dblClk = false;
					//vScroll = true;
					break;
				}

				if (ui::lbDown && i != currentCmd &&!action) {
					action = true;
					currentCmd = i;
					currentParam = -1;
					subParam = -1;

				}
			}
			else
			{
				ui::style::box::outlineBrightness = 0.1f;
			}

			float sel = currentCmd == i ? 1.f : 0.f;

			if (sel) selYpos = y;

			ui::style::box::r = lerp(0.2f, .8f, sel);
			ui::style::box::g = lerp(0.2f, .8f, sel);
			ui::style::box::b = lerp(0.2f, .8f, sel);

			ui::Box::Draw(x, y, w, ui::style::text::height * .8f);

			y += lead;
		}

		//second control
		
	}



	void ObjHandlers()
	{
		ui::Box::Setup();

		hilightedCmd = currentCmd;

		for (int i = startCmd; i < cmdCounter; i++)
		{
			for (int j = 0; j < cmdParamDesc[i].pCount; j++)
			{
				if (!isType(cmdParamDesc[i].param[j].type, "position")) continue;

				float _x = (float)cmdParamDesc[i].param[j].value[0];
				float _y = (float)cmdParamDesc[i].param[j].value[1];
				float _z = (float)cmdParamDesc[i].param[j].value[2];

				XMVECTOR p = XMVECTOR{ _x / intToFloatDenom, _y / intToFloatDenom, _z / intToFloatDenom, 1. };
				p = XMVector4Transform(p, Camera::viewCam.view * Camera::viewCam.proj);

				if (XMVectorGetZ(p) < 0) continue;

				float px = .5f * XMVectorGetX(p) / XMVectorGetW(p) + .5f;
				float py = -.5f * XMVectorGetY(p) / XMVectorGetW(p) + .5f;

				float h = ui::style::text::height * .48f;
				px -= h * dx11::aspect / 2.f;
				py -= h / 2.f;

				ui::style::box::outlineBrightness = 1.5;

				ui::style::box::rounded = .5;
				ui::style::box::edge = 1.;
				ui::style::box::soft = 11.;

				bool over = isMouseOver(px, py, h * dx11::aspect, h);

				ui::style::box::r = ui::style::box::g = ui::style::box::b = over ? .7f : 0.f;

				if (over && isKeyDown(CAM_KEY))
				{
					hilightedCmd = i;
				}

				if (over && ui::lbDown && isKeyDown(CAM_KEY) )
				{
					
					currentCmd = i;
					currentParam = -1;
					subParam = -1;

					ViewCam::TransCam(_x / intToFloatDenom, _y / intToFloatDenom, _z / intToFloatDenom);

					vScroll = true;
				}

				ui::Box::Draw(px, py, h * dx11::aspect, h);

			}
		}
	}

	void insertNumber(int p)
	{
		if (cmdParamDesc[currentCmd].param[currentParam].bypass) return;
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

		if (cmdParamDesc[currentCmd].param[currentParam].bypass) return;
		auto sType = cmdParamDesc[currentCmd].param[currentParam].type;

		if (isTypeEnum(sType)) return;

		float scale = ui::lbDown ? 10.f : 1.f;

		cmdParamDesc[currentCmd].param[currentParam].value[max(subParam,0)] += (int)(sign(delta) * scale);

	}



	//detect expressions and variables in caller and set bypass



	

	void setBypass()
	{
		const char* filename = cmdParamDesc[cmdCounter].caller.fileName; 
		const int lineNum = cmdParamDesc[cmdCounter].caller.line;

		using namespace std;
		string inFilePath = filename;

		string s;
		ifstream ifile(inFilePath);

		int lc = 1;
		if (ifile.is_open())
		{
			while (getline(ifile, s) && lc != lineNum) lc++;

			unsigned int pos = 0;
			pos = s.find(cmdParamDesc[cmdCounter].funcName);
			if (pos != string::npos)
			{
				pos = s.find("(")+1;
				unsigned int posEnd = 0;
				posEnd = s.find(";",pos);
				posEnd = s.rfind(")", posEnd);
				string s2;
				s2.append(s.substr(pos, posEnd - pos));
				s2.erase(remove(s2.begin(), s2.end(), ' '), s2.end());
				s2.erase(remove(s2.begin(), s2.end(), '\t'), s2.end());

				constexpr auto regex_str = R"(,)";
				const std::regex reg{ regex_str };
				const auto tokens = regex_split(s2, reg);

				int j = 0;
				for (auto& i : tokens)
				{
					bool enumValue = false;

					if (i.find("::") != string::npos)
					{//check for known types
						const auto t = i.substr(0, i.find("::"));
						enumValue = isTypeEnum(t.c_str());
					}

					bool intValue = isNumber(i);

					bool bypass = !(intValue || enumValue);

					cmdParamDesc[cmdCounter].param[j].bypass = bypass;
					j++;
				}


			}

		}

		ifile.close();
		
	}

	//create c++ call string and save it into source file
	void Save(int cmdIndex)
	{
		if (cmdIndex < 0) return;

		std::vector<std::string> genParams;

		for (int i = 0; i < cmdParamDesc[cmdIndex].pCount; i++)
		{
			auto sType = cmdParamDesc[cmdIndex].param[i].type;

			if (isTypeEnum(sType))
			{
				genParams.push_back(std::string(cmdParamDesc[cmdIndex].param[i].type) +
					"::" + std::string(getEnumStr(sType, (int)cmdParamDesc[cmdIndex].param[i].value[0])));
			}
			else
			{
				char pStr[32];
				for (int x = 0; x < getTypeDim(sType); x++)
				{
					_itoa(cmdParamDesc[cmdIndex].param[i].value[x], pStr, 10);
					genParams.push_back(std::string(pStr));
				}
			}

		}

		const char* filename = cmdParamDesc[cmdIndex].caller.fileName;
		const int lineNum = cmdParamDesc[cmdIndex].caller.line;

		using namespace std;
		string inFilePath = filename;
		string outFilePath = inFilePath + "_";

		string s;
		ifstream ifile(inFilePath);
		ofstream ofile(outFilePath);

		std::string caller;

		int lc = 1;
		if (ifile.is_open())
		{
			while (lc < lineNum && getline(ifile, s))
			{
				ofile << s << "\n";
				lc++;
			}

			getline(ifile, s);

			unsigned int pos = 0;
			pos = s.find(cmdParamDesc[cmdIndex].funcName);
			if (pos != string::npos)
			{
				unsigned int startFuncPos = pos;
				pos = s.find("(") + 1;
				unsigned int posEnd = 0;
				posEnd = s.find(";", pos);
				posEnd = s.rfind(")", posEnd);
				string s2;
				s2.append(s.substr(pos, posEnd - pos));
				s2.erase(remove(s2.begin(), s2.end(), ' '), s2.end());
				s2.erase(remove(s2.begin(), s2.end(), '\t'), s2.end());

				constexpr auto regex_str = R"(,)";
				const std::regex reg{ regex_str };
				const auto tokens = regex_split(s2, reg);

				caller.append(cmdParamDesc[cmdIndex].funcName);
				caller.append("(");

				int j = 0;
				for (auto& i : tokens)
				{
					if (cmdParamDesc[cmdIndex].param[j].bypass)
					{
						caller.append(i);
					}
					else
					{
						caller.append(genParams[j]);
					}

					caller.append(", ");

					j++;
				}

				caller.erase(caller.size() - 2);
				caller.append(")");

				string pre = s.substr(0, startFuncPos);
				posEnd = s.find(";", startFuncPos);
				string post = s.substr(posEnd,s.length()-posEnd);
				
				ofile << pre << caller << post << "\n";
			}

			while (getline(ifile, s))
			{
				ofile << s << "\n";
			}


		}

		ifile.close();
		ofile.close();

		remove(inFilePath.c_str());
		rename(outFilePath.c_str(), inFilePath.c_str());
	}

	float valueDrawOffset = .1f;
	float enumDrawOffset = .2f;

	float enumPos = 0;

	void showParamBoxes()
	{
		pCountV = 0;

		ui::Box::Setup();

		float _x = x + valueDrawOffset - insideX;
		float __x = x + enumDrawOffset - insideX;

		float y = selYpos;

		for (int i = 0; i < cmdParamDesc[currentCmd].pCount; i++)
		{

			//if (cmdParamDesc[currentCmd].param[i].bypass) continue;

			auto w = tabLen*.9f;
			auto sType = cmdParamDesc[currentCmd].param[i].type;
			int count = getTypeDim(sType);

			if (editContext)
			{

				if (count == 1)
				{
					ui::style::box::outlineBrightness = 0.1f;
					if (isMouseOver(_x, y, w, ui::style::box::height)&&
						!cmdParamDesc[currentCmd].param[i].bypass)
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

						if (isMouseOver(_x , y+ float(p)*lead, w, ui::style::box::height)&&
							!cmdParamDesc[currentCmd].param[i].bypass)
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

				ui::style::box::a = cmdParamDesc[currentCmd].param[i].bypass ? .25 : 1;

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

			pCountV += count;
			y += lead* count;
		}

		if (!isTypeEnum(cmdParamDesc[currentCmd].param[currentParam].type))
		{
			if (editContext && currentParam >= 0)
			{
				if (ui::mbDown)
				{
					cmdParamDesc[currentCmd].param[currentParam].value[max(subParam, 0)] = storedParam[max(subParam, 0)] - (int)(ui::mouseDelta.y * height);
				}
				else
				{
					storedParam[max(subParam, 0)] = cmdParamDesc[currentCmd].param[currentParam].value[max(subParam, 0)];
				}
			}
		}
	}

	float paramAreaHeight = 0.f;

	void showParams()
	{
		paramAreaHeight = 0.f;

		float inCurPos = 0;

		showParamBoxes();

		ui::Text::Setup();

		showCursor = false;
		auto bw = .05f;

		float y = selYpos;

		for (int i = 0; i < cmdParamDesc[currentCmd].pCount; i++)
		{
			ui::style::text::a = cmdParamDesc[currentCmd].param[i].bypass ? .75 : 1;

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
					ui::Text::Draw(vstr, x + enumDrawOffset, selYpos + float(i + e) * lead + insideY);
				}
			}


			if (editContext && currentParam == i && !isTypeEnum(sType) && !cmdParamDesc[currentCmd].param[i].bypass)
			{
				showCursor = true;
				cursorX = x + valueDrawOffset + inCurPos;
				cursorY = y + insideY + max(subParam,0)*lead;
			}

			y += lead*count;
		}
	}

	float isOutside(float x, float top, float bottom)
	{
		if (x < top) return x - top;
		if (x > bottom) return x - bottom;
		return 0.f;
	}


	void ShowStack()
	{
		int currentCmd_backup = currentCmd;

		tabLen = ui::Text::getTextLen("000000000000000", ui::style::text::width);
		valueDrawOffset = tabLen *1.f;
		enumDrawOffset = tabLen *2.f;

		clickOnEmptyPlace = true;

		if (ui::rbDown && editContext)
		{
			yPos = yPosLast + editor::ui::mouseDelta.y;
		}

		ui::style::Base();

		//top = ui::style::text::height;
		//bottom = 1.f - ui::style::text::height*2.5f;
		top = 0.f;
		bottom = 1.f;


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
		x += tabLen;
		showParams();

		if (showCursor)
		{
			ui::Box::Setup();
			ui::style::box::width = 2.f / width;
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

		float scroll = isOutside(selYpos, top+lead*1.f, bottom - lead * (pCountV+1)) / 4.f;
		yPos -= vScroll ? scroll : 0.f;
		if (fabs(scroll) < 0.00125f) vScroll = false;

		if (currentCmd_backup != currentCmd) Save(currentCmd_backup);

	}

}
