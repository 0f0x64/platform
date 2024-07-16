namespace paramEdit {

	int floatConvertDigits = 5;

	float yPos=0.05;
	float yPosLast;

	int storedParam[4] = { 0,0,0,0 };
	int currentParam = -1;
	int subParam = 0;

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

	float top;
	float bottom;
	float lead;
	float insideX;
	float insideY;
	float x;
	float y;

	void set2Style(float sel)
	{
		ui::style::box::r = ui::style::box::g = ui::style::box::b = lerp(0.2f, .8f, sel);
		ui::style::text::r = ui::style::text::g = ui::style::text::b = lerp(.8f, 1.f, sel);
	}

	void setBStyle(float sel)
	{
		ui::style::box::r = ui::style::box::g = ui::style::box::b = lerp(0.2f, .8f, sel);
	}

	void setTStyle(float sel)
	{
		ui::style::text::r = ui::style::text::g = ui::style::text::b = ui::style::text::b = lerp(.8f, .2f, sel);;
	}

	float selYpos = 0.5;

	void setParamsAttr()
	{
		for (int i = 0; i < cmdParamDesc[cmdCounter].pCount; i++)
		{
			auto pid = getTypeIndex(cmdParamDesc[cmdCounter].param[i].type);
			cmdParamDesc[cmdCounter].param[i].typeIndex = pid;

			if (pid < 0)
			{
				cmdParamDesc[cmdCounter].param[i]._min = INT_MIN;
				cmdParamDesc[cmdCounter].param[i]._max = INT_MAX;
				cmdParamDesc[cmdCounter].param[i]._dim = 1;
			}
			else
			{
				cmdParamDesc[cmdCounter].param[i]._min = typeDesc[pid]._min;
				cmdParamDesc[cmdCounter].param[i]._max = typeDesc[pid]._max;
				cmdParamDesc[cmdCounter].param[i]._dim = typeDesc[pid]._dim;
			}
		}
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

				int j = 0;
				for (auto& i : tokens)
				{
					bool enumValue = false;

					if (i.find("::") != string::npos)
					{//check for known types
						const auto t = i.substr(0, i.find("::"));
						enumValue = isTypeEnum(getTypeIndex(t.c_str()));
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
			auto sType = cmdParamDesc[cmdIndex].param[i].typeIndex;
			
			if (isTypeEnum(sType))
			{
				genParams.push_back(std::string(cmdParamDesc[cmdIndex].param[i].type) +
					"::" + std::string(getStrValue(sType, (int)cmdParamDesc[cmdIndex].param[i].value[0])));
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
				string post = s.substr(posEnd, s.length() - posEnd);

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

	void pLimits()
	{
		cmdParamDesc[currentCmd].param[currentParam].value[subParam] = clamp(cmdParamDesc[currentCmd].param[currentParam].value[subParam], cmdParamDesc[currentCmd].param[currentParam]._min, cmdParamDesc[currentCmd].param[currentParam]._max);
	}

	//TEXT EDITOR
	void insertNumber(int p)
	{
		if (cmdParamDesc[currentCmd].param[currentParam].bypass) return;
		auto sType = cmdParamDesc[currentCmd].param[currentParam].typeIndex;
		
		if (isTypeEnum(sType)) return;

		if (p == '0' && cursorPos == 0) return;
		if (p == VK_OEM_MINUS && cursorPos != 0) return;

		char vstr[_CVTBUFSIZE];
		char vstr2[_CVTBUFSIZE];

		_itoa(cmdParamDesc[currentCmd].param[currentParam].value[subParam], vstr, 10);

		strcpy(vstr2, vstr);
		vstr2[cursorPos] = p == VK_OEM_MINUS ? '-' : (unsigned char)p;
		vstr2[cursorPos + 1] = 0;
		strncat(vstr2 + cursorPos + 1, vstr + cursorPos, strlen(vstr) - cursorPos);

		cmdParamDesc[currentCmd].param[currentParam].value[subParam] = atoi(vstr2);
		pLimits();
		cursorPos++;

	}

	void Delete()
	{
		auto sType = cmdParamDesc[currentCmd].param[currentParam].typeIndex;
		if (isTypeEnum(sType)) return;

		char vstr[_CVTBUFSIZE];
		char vstr2[_CVTBUFSIZE];

		_itoa(cmdParamDesc[currentCmd].param[currentParam].value[subParam], vstr, 10);

		if (cursorPos == strlen(vstr))
		{
			return;
		}

		strcpy(vstr2, vstr);
		strncpy(vstr2 + cursorPos, vstr + cursorPos + 1, strlen(vstr) - cursorPos);

		cmdParamDesc[currentCmd].param[currentParam].value[subParam] = atoi(vstr2);
		pLimits();

	}

	void BackSpace()
	{
		auto sType = cmdParamDesc[currentCmd].param[currentParam].typeIndex;
		if (isTypeEnum(sType)) return;
		if (cursorPos == 0) return;

		char vstr[_CVTBUFSIZE];
		char vstr2[_CVTBUFSIZE];

		_itoa(cmdParamDesc[currentCmd].param[currentParam].value[subParam], vstr, 10);
		strcpy(vstr2, vstr);
		strncpy(vstr2 + max(cursorPos - 1, 0), vstr + cursorPos, max(strlen(vstr) - cursorPos + 1, 0));

		cmdParamDesc[currentCmd].param[currentParam].value[subParam] = atoi(vstr2);
		pLimits();

		cursorPos--;
	}

	void Wheel(float delta)
	{
		if (!editContext) return;
		if (currentCmd == -1) return;
		if (currentParam == -1) return;

		if (cmdParamDesc[currentCmd].param[currentParam].bypass) return;
		auto sType = cmdParamDesc[currentCmd].param[currentParam].typeIndex;

		if (isTypeEnum(sType)) return;

		float scale = ui::lbDown ? 10.f : 1.f;

		cmdParamDesc[currentCmd].param[currentParam].value[subParam] += (int)(sign(delta) * scale);
		pLimits();

	}

	//SHOW STACK
	void showCommands()
	{
		for (int i = startCmd; i < cmdCounter; i++)
		{
			auto w = ui::Text::getTextLen(cmdParamDesc[i].funcName, ui::style::text::width) + insideX * 2.f;

			auto cl = cmdParamDesc[i].stackLevel;
			if (i > startCmd && curCmdLevel > cl) break;
			if (i > startCmd && curCmdLevel != cl) continue;

			auto mo = isMouseOver(x, y, w, ui::style::box::height);

			if (mo)//process click
			{
				clickOnEmptyPlace = false;

				if (ui::dblClk && ui::lbDown)
				{
					if (i == startCmd) //close
					{
						curCmdLevel = max(cl , 0);

						for (int j = startCmd; j >= 0; j--)//search for prev level
						{
							if (cmdParamDesc[j].stackLevel < curCmdLevel  || j == 0) {
								startCmd = j; break;
							}
						}

						int cnt = 0;
						for (int j = startCmd; j < i; j++)//displayed level cmd count for adjust pos
						{
							if (j == startCmd || curCmdLevel == cmdParamDesc[j].stackLevel) cnt++;
						}
						
						yPos -= float(cnt)* lead;

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
					break;
				}

				if (ui::lbDown && i != currentCmd &&!action) {
					action = true;
					currentCmd = i;
					currentParam = -1;
					subParam = 0;
				}
			}

			//draw

			ui::style::box::outlineBrightness = mo ? 1.f : 0.1f;

			float sel = currentCmd == i ? 1.f : 0.f;
			if (sel) selYpos = y;

			ui::Box::Setup();
			setBStyle(sel);
			ui::Box::Draw(x, y, w, ui::style::text::height * .8f);

			ui::Text::Setup();
			setTStyle(sel);
			ui::Text::Draw(cmdParamDesc[i].funcName, x + insideX, y + insideY);

			y += lead;
		}
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
				
				p = XMVector4Transform(p, XMMatrixTranspose(ConstBuf::camera.view[0]) * XMMatrixTranspose(ConstBuf::camera.proj[0]));

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
					subParam = 0;
					ViewCam::TransCam(_x / intToFloatDenom, _y / intToFloatDenom, _z / intToFloatDenom);
					vScroll = true;
				}

				ui::Box::Draw(px, py, h * dx11::aspect, h);

			}
		}
	}


	void CamKeys()
	{
		if (!isKeyDown(VK_LBUTTON)) stillDragCamKey = false;

		ui::Box::Setup();

		for (int i = startCmd; i < cmdCounter; i++)
		{
			if (strcmp(cmdParamDesc[i].funcName, "setCamKey")) continue;

			for (int j = 0; j < cmdParamDesc[i].pCount; j++)
			{
				if (!isType(cmdParamDesc[i].param[j].type, "timestamp")) continue;

				int _x = cmdParamDesc[i].param[j].value[0];

				float px = TimeLine::getScreenPos(_x*SAMPLES_IN_FRAME);
				float py = .94;

				float h = ui::style::text::height * .58f;
				px -= h * dx11::aspect / 2.f;
				py -= h / 2.f;

				bool over = isMouseOver(px, py, h * dx11::aspect, h) ;

				if (stillDragCamKey)
				{
					over = currentCmd == i;
				}

				ui::style::box::rounded = .990;
				ui::style::box::edge = 100.1;
				ui::style::box::soft = 1000.;
				ui::style::box::outlineBrightness = over ? .7f : 0.2f;
				ui::style::box::r = ui::style::box::g = ui::style::box::b = (currentCmd == i) ? .7f : 0.2f;
				ui::style::box::a = 1;

				if (over && ui::lbDown && !stillDragCamKey)
				{
					stillDragCamKey = true;
					currentCmd = i;
					currentParam = -1;
					subParam = 0;
					storedParam[j] = cmdParamDesc[i].param[j].value[0];
					vScroll = true;
				}

				if (currentCmd == i && stillDragCamKey)
				{
					if (ui::lbDown)
					{
						cmdParamDesc[i].param[j].value[0] = storedParam[j] + TimeLine::ScreenToTime( ui::mouseDelta.x)/SAMPLES_IN_FRAME;
					}
					else
					{
						storedParam[j] = cmdParamDesc[i].param[j].value[0];
					}
				}

				ui::Box::Draw(px, py, h * dx11::aspect, h);

			}
		}
	}

	float valueDrawOffset = .1f;
	float enumDrawOffset = .2f;
	float enumPos = 0;

	void showParams()
	{
		float inCurPos = 0;
		pCountV = 0;
		showCursor = false;
		float _x = x + valueDrawOffset - insideX;
		float __x = x + enumDrawOffset - insideX;
		float y = selYpos;

		for (int i = 0; i < cmdParamDesc[currentCmd].pCount; i++)
		{
			ui::style::text::a = cmdParamDesc[currentCmd].param[i].bypass ? .75f : 1.f;

			char vstr[_CVTBUFSIZE];

			auto w = tabLen * .9f;
			auto sType = cmdParamDesc[currentCmd].param[i].typeIndex;
			int subCount = cmdParamDesc[currentCmd].param[i]._dim;

			if (!cmdParamDesc[currentCmd].param[i].bypass)
			{
				for (int p = 0; p < subCount; p++) //process clicks
				{
					if (isMouseOver(_x, y + float(p) * lead, w, ui::style::box::height))
					{
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
								storedParam[p] = cmdParamDesc[currentCmd].param[currentParam].value[p];
								subParam = p;
							}

						}
					}
				}
			}

			float sel = currentParam == i ? 1.f : 0.f;
			set2Style(sel);

			ui::Box::Setup();
			for (int p = 0; p < subCount; p++)
			{
				float sel_ = currentParam == i ? 1.f : 0.f;
				sel_ *= (float)(subParam == p);
				setBStyle(sel_);

				bool mo = isMouseOver(_x, y + float(p) * lead, w, ui::style::box::height);

				ui::style::box::outlineBrightness = lerp(0.1f, 1.f, (float)mo);
				ui::style::box::a = cmdParamDesc[currentCmd].param[i].bypass ? .25f : 1.f;

				ui::Box::Draw(_x, y + float(p) * lead, w, ui::style::text::height * .8f);
			}

			if (sel && isTypeEnum(sType))
			{
				auto val = cmdParamDesc[currentCmd].param[i].value[0];

				for (int e = 0; e < getEnumCount(sType); e++)
				{
					float sel = e == val ? 1.f : 0.f;
					auto over = isMouseOver(__x, y + e * lead, w, ui::style::box::height);

					ui::style::box::r = ui::style::box::g = ui::style::box::b = .2f;
					ui::style::box::outlineBrightness = over ? 1.f : 0.1f;

					ui::Box::Draw(__x, y + e * lead, w, ui::style::text::height * .8f);

					if (over)
					{
						clickOnEmptyPlace = false;

						if (ui::lbDown && !action)
						{
							action = true;
							currentParam = -1;
							cmdParamDesc[currentCmd].param[i].value[0] = e;
						}

					}

				}
			}

			ui::Text::Setup();
			ui::Text::Draw(cmdParamDesc[currentCmd].param[i].name, x + insideX, y + insideY);

			strcpy(vstr, getStrValue(sType, (int)cmdParamDesc[currentCmd].param[i].value[0]));

			setTStyle(sel);

			char val[_CVTBUFSIZE];
			for (int k = 0; k < subCount; k++)
			{
				float sel_ = currentParam == i ? 1.f : 0.f;

				if (subCount > 1) {
					sel_ *= (float)(subParam == k);
					_itoa(cmdParamDesc[currentCmd].param[i].value[k], val, 10);
					strcpy(vstr, val);
				}

				setTStyle(sel_);
				ui::Text::Draw(vstr, x + valueDrawOffset, y + float(k) * lead + insideY);

				if (sel_) {
					inCurPos = ui::Text::getTextLen(vstr, ui::style::text::width, cursorPos);
					cursorPos = clamp(cursorPos, 0, (int)strlen(vstr));
				}
			}

			if (sel)
			{
				if (isTypeEnum(sType))//enum dropdown menu
				{
					ui::style::text::r = ui::style::text::g = ui::style::text::b = .8f;

					for (int e = 0; e < getEnumCount(sType); e++)
					{
						ui::Text::Draw(getStrValue(sType, e), x + enumDrawOffset, y + e * lead + insideY);
					}
				} 
				else//show cursor on textfield
				{
					showCursor = true;
					cursorX = x + valueDrawOffset + inCurPos;
					cursorY = y + insideY + subParam * lead;
				}
			}

			pCountV += subCount;
			y += lead* subCount;
		}

		//drag by mouse
		auto ti = getTypeIndex(cmdParamDesc[currentCmd].param[currentParam].type);
		if (!isTypeEnum(ti))
		{
			if (currentParam >= 0)
			{
				if (ui::lbDown)
				{
					cmdParamDesc[currentCmd].param[currentParam].value[subParam] = storedParam[subParam] + (int)(ui::mouseDelta.x * width);
					pLimits();
				}
				else
				{
					storedParam[subParam] = cmdParamDesc[currentCmd].param[currentParam].value[subParam];
				}
			}
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

		if (ui::rbDown)
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
		insideY = .19f * ui::style::box::height;
		yPos = max(yPos, top - cmdCounter * lead + lead);
		yPos = min(yPos, bottom - lead);

		gapi.setScissors(rect{ 0, (int)(top*dx11::height), dx11::width, (int)(bottom*dx11::height) });

		x = ui::style::text::width / 2.f;
		y = yPos;
		showCommands();

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
