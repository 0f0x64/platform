namespace paramEdit {

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
				//for (auto& i : tokens)
				for (int n=0; n< cmdParamDesc[cmdCounter].pCount;n++)
				{
					bool enumValue = false;

					if (tokens[j].find("::") != string::npos)
					{//check for known types
						const auto t = tokens[j].substr(0, tokens[j].find("::"));
						enumValue = isTypeEnum(getTypeIndex(t.c_str()));
					}

					bool intValue = isNumber(tokens[j]);

					bool bypass = !(intValue || enumValue);

					cmdParamDesc[cmdCounter].param[n].bypass = bypass;
					
					j+= cmdParamDesc[cmdCounter].param[n]._dim;
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
				auto tokens = regex_split(s2, reg);

				caller.append(cmdParamDesc[cmdIndex].funcName);
				caller.append("(");

				int j = 0;
				for (int i = 0; i < cmdParamDesc[cmdIndex].pCount; i++)
				{
					auto sType = cmdParamDesc[cmdIndex].param[i].typeIndex;
					for (int x = 0; x < getTypeDim(sType); x++)
					{
						if (j >= tokens.size()) tokens.push_back("0");

						/*if (!isNumber(tokens[i].c_str()))
						{
							caller.append(tokens[i]);
						}*/

						if (cmdParamDesc[cmdIndex].param[i].bypass)
						{
							caller.append(tokens[i]);
						}
						else
						{
							caller.append(genParams[j]);
						}

						caller.append(", ");

						j++;
					}
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

		Log("Modified:  ");
		Log(inFilePath.c_str());
		Log("\n");
	}



	

	//SHOW STACK

	bool mouseOverItem = false;
	float prevY;

	bool showButton(float x, float y, float w, float h, float sel, int i)
	{
		bool mo = isMouseOver(x, y, w, h);

		ui::style::box::outlineBrightness = mo ? 1.f : 0.1f;

		ui::Box::Setup();
		setBStyle(sel);
		ui::Box::Draw(x, y, w, h);

		ui::Text::Setup();
		setTStyle(sel);
		ui::Text::Draw(cmdParamDesc[i].funcName, x + insideX, y + insideY);

		return mo;
	}

	void showStackItem(int i, float& x, float& y, float w, float lead, float sel)
	{
		mouseOverItem = showButton(x, y, w, ui::style::text::height * .8f, sel, i);
		y += lead;
	}

	int searchParent(int start)
	{
		auto ccl = max(0,cmdParamDesc[start].stackLevel);

		for (int j = start; j >= 0; j--)//search for prev level
		{
			if (cmdParamDesc[j].stackLevel < ccl || j == 0) {
				return j;
			}
		}

		return 0;
	}

	bool isContentInside(int pos)
	{

		auto ccl = max(0, cmdParamDesc[pos].stackLevel);

		for (int j = pos + 1; j < cmdCounter; j++)
		{
			if (ccl < cmdParamDesc[j].stackLevel)
			{
				return true;
			}

			if (ccl >= cmdParamDesc[j].stackLevel)
			{
				return false;
			}
		}

		return false;
	}

	void stepOut(int i)
	{
		curCmdLevel = max(cmdParamDesc[i].stackLevel, 0);
		startCmd = searchParent(i);

		for (int j = startCmd; j < i; j++)//displayed level cmd count for adjust pos
		{
			if (j == startCmd || curCmdLevel == cmdParamDesc[j].stackLevel) yPos -= lead;
		}

	}

	void stepIn(int i)
	{
		if (!isContentInside(i)) return;

		startCmd = i;
		curCmdLevel = cmdParamDesc[i].stackLevel + 1;
		yPos = prevY;
	}

	void showCommands()
	{

		for (int i = startCmd; i < cmdCounter; i++)
		{
			prevY = y;
			if (i > startCmd && curCmdLevel > cmdParamDesc[i].stackLevel) break;
			if (i > startCmd && curCmdLevel != cmdParamDesc[i].stackLevel) continue;

			//draw
			float sel = currentCmd == i ? 1.f : 0.f;
			if (sel) selYpos = y;

			if (cmdParamDesc[i].uiDraw)
			{
				cmdParamDesc[i].uiDraw(i, x, y, ui::Text::getTextLen(cmdParamDesc[i].funcName, ui::style::text::width) + insideX * 2.f, lead, sel);
			}

			//process clicks
			if (mouseOverItem)
			{
				clickOnEmptyPlace = false; 

				if (ui::dblClk && ui::lbDown)
				{
					if (i != startCmd) {
						stepIn(i);
					} else {
						stepOut(i);
					}

					ui::dblClk = false;
					break;
				}

				if (ui::lbDown && i != currentCmd && !action) {
					action = true;
					currentCmd = i;
					currentParam = -1;
					subParam = 0;
				}
			}
		}
	}

	void ObjHandlers()
	{
		if (uiContext != uiContext_::camera) return;

		ui::Box::Setup();

		hilightedCmd = currentCmd;

		for (int i = startCmd; i < cmdCounter; i++)
		{
			auto cl = cmdParamDesc[i].stackLevel;
			if (i > startCmd && curCmdLevel > cl) break;
			if (i > startCmd && curCmdLevel != cl) continue;
			//if (!isType(cmdParamDesc[i].funcName, "ShowObject")) continue;

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

				if (over)
				{
					hilightedCmd = i;
				}

				if (over && ui::lbDown && drag.isFree())
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

				if (drag.check(drag.context::timeKey))
				{
					over = currentCmd == i;
				}

				ui::style::box::rounded = .990;
				ui::style::box::edge = 100.1;
				ui::style::box::soft = 1000.;
				ui::style::box::outlineBrightness = over ? .7f : 0.2f;
				ui::style::box::r = ui::style::box::g = ui::style::box::b = (currentCmd == i) ? .7f : 0.2f;
				ui::style::box::a = 1;

				if (over && ui::lbDown && drag.isFree())
				{
					drag.set(drag.context::timeKey);
					currentCmd = i;
					currentParam = -1;
					subParam = 0;
					storedParam[0] = cmdParamDesc[i].param[j].value[0];
					vScroll = true;

					curCmdLevel = cmdParamDesc[currentCmd].stackLevel;

					for (int j = currentCmd; j >= 0; j--)//search for prev level
					{
						if (cmdParamDesc[j].stackLevel < curCmdLevel || j == 0) {
							startCmd = j; break;
						}
					}
					 
				}

				if (currentCmd == i && drag.check(drag.context::timeKey))
				{
					if (ui::lbDown)
					{
						cmdParamDesc[i].param[j].value[0] = storedParam[0] + TimeLine::ScreenToTime( ui::mouseDelta.x)/SAMPLES_IN_FRAME;
						pLimits(currentCmd,j);
					}
					else
					{
						storedParam[0] = cmdParamDesc[i].param[j].value[0];
					}
				}

				ui::Box::Draw(px, py, h * dx11::aspect, h);

			}
		}
	}

	void clipStyleApply(int i, bool over)
	{
		ui::style::box::rounded = .145;
		ui::style::box::edge = 100;
		ui::style::box::soft = 30;
		ui::style::box::outlineBrightness = over ? .8f : 0.5f;

		if (currentCmd == i)
		{
			ui::style::box::r = ui::style::box::g = ui::style::box::b = .6f;
		}

		ui::style::box::a = 1;
	}

	void noteStyleApply(int i, bool over)
	{
		ui::style::box::rounded = .145 * 2.;
		ui::style::box::edge = 150;
		ui::style::box::soft = 1135;
		ui::style::box::outlineBrightness = over ? .5f : 0.3f;

		ui::style::box::rounded = .145;
		//ui::style::box::outlineBrightness = over ? .5f : 0.1f;

	}


	

	float clipYpos;

	//float clip_x = 0;
	//float clip_y = 0;
	//float clip_l = 0;
	//float note_step = 0;
	//int clip_r = 0;
	//int clip_nc;
	//float clip_counter = 0;

	char noteText[7] = "      ";

	void CreateNoteText(BYTE p)
	{
		const char* table = "C-0C#0D-0D#0E-0F-0F#0G-0G#0A-0A#0B-0";
		//const char* table = "C 0C#0D 0D#0E 0F 0F#0G 0G#0A 0A#0B 0";
		const char* vtable = "0123456789";
		strcpy(noteText, " - ");

		if (p == 0) return;

		if (p == 255)
		{
			strcpy(noteText, "off");
			return;
		}

		if (p < 12 * 8 + 1) //in range
		{
			p--;//skip nonote
			int octave = p / 12;
			char _octave[10];
			_itoa(octave, _octave, 10);
			int note = p - octave * 12;

			noteText[0] = table[note * 3];
			noteText[1] = table[note * 3 + 1];
			noteText[2] = _octave[0];
			noteText[3] = 0;
		}

	}
	/*
	void showTrack()
	{
		ui::Box::Setup();

		int topUIElementIndex = -1;

		int channelIndex = -1;

		for (int iter = 0; iter < 2; iter++)
		{
			clip_counter = 0;
			clipYpos = ui::style::box::height;
			TimeLine::screenLeft = x + enumDrawOffset - insideX;
			int trackStartLvl = -1;

			for (int i = 0; i < cmdCounter; i++)
			{
				if (cmdParamDesc[i].stackLevel == trackStartLvl) break;
				if (!strcmp(cmdParamDesc[i].funcName, "Track")) trackStartLvl = cmdParamDesc[i].stackLevel;
				if (trackStartLvl == -1) continue;

				if (!strcmp(cmdParamDesc[i].funcGroup, "channel"))
				{
					channelIndex = i;
					clipYpos += ui::style::box::height*2;
				}

				if (!strcmp(cmdParamDesc[i].funcName, "Clip"))
				{
					clip_counter++;
					float bpm = editor::TimeLine::bpm;
					int _x = getParamByStr(i, "pos");
					int clipRepeat = getParamByStr(i, "repeat");
					int clipBpmScale = getParamByStr(i, "bpmScale");
					int len = getParamByStr(i, "len");
					float frame = SAMPLING_FREQ / FRAMES_PER_SECOND;
					float sWidth = TimeLine::TimeToScreen(frame * 60 * 60 * len * clipRepeat / (bpm * clipBpmScale));

					float px = TimeLine::getScreenPos(frame * 60 * 60 * _x / bpm);
					float py = clipYpos;

					float h = ui::style::text::height * .58f / 1.5f;
					py -= h / 2.f;

					clip_x = px;
					clip_y = py;
					clip_l = sWidth;
					clip_nc = len;
					note_step = sWidth / len/ clipRepeat;
					clip_r = clipRepeat;

					bool over = isMouseOver(px, py, sWidth, h);

					if (iter == 0)
					{
						if (over) topUIElementIndex = i;
						continue;
					}
					else
					{
						//over = (i == topUIElementIndex) & over;
						if (over && i == topUIElementIndex) over = true; else over = false;
					}

					if (lbDragContext == DragContext::clipMove)
					{
						over = currentCmd == i;
					}

					clipStyleApply(i, over);

					int j = 0;//adjust "pos"

					if (over && ui::lbDown && lbDragContext == DragContext::free)
					{
						lbDragContext = DragContext::clipMove;
						currentCmd = i;
						currentParam = -1;
						subParam = 0;
						storedParam[j] = cmdParamDesc[i].param[j].value[0];
						vScroll = true;

						curCmdLevel = cmdParamDesc[currentCmd].stackLevel;

						for (int j = currentCmd; j >= 0; j--)//search for prev level
						{
							if (cmdParamDesc[j].stackLevel < curCmdLevel || j == 0) {
								startCmd = j; break;
							}
						}

					}

					if (currentCmd == i && lbDragContext == DragContext::clipMove)
					{
						if (ui::lbDown)
						{
							cmdParamDesc[i].param[j].value[0] = storedParam[j] + TimeLine::ScreenToTime(ui::mouseDelta.x) / (frame * 60 * 60 / bpm);
							pLimits(currentCmd, j);
						}
						else
						{
							storedParam[j] = cmdParamDesc[i].param[j].value[0];
						}
					}

					ui::Box::Draw(px, py, sWidth, h/1.125);
					for (int r = 0; r < clip_r; r++)
					{
						ui::style::box::r = .4 + .2 * sin(clip_counter * 12.123);
						ui::style::box::g = .4 + .2 * sin(clip_counter * 23.123);
						ui::style::box::b = .4 + .2 * sin(clip_counter * 44.123);
						ui::Box::Draw(px + r * len * note_step, py+h/1.5, note_step * len, h/1.5);
					}

					if (channelIndex >= 0)
					{
						ui::Text::Draw(cmdParamDesc[channelIndex].funcName, TimeLine::screenLeft, clipYpos - ui::style::box::height * .25);
					}
				}

				// show all layers

				if (iter == 0) continue;
				
				if (!strcmp(cmdParamDesc[i].funcName, "Pitch"))
				{
					for (int r = 0; r < clip_r; r++)
					{
						for (int n = 1; n < cmdParamDesc[i].pCount; n++)
						{
							float h = ui::style::text::height * .58f / 1.f;
							float x = clip_x + (n - 1)* note_step+ (r * clip_nc*note_step);
							float y = clip_y + h;

							bool over = isMouseOver(x, y, note_step, h);
							noteStyleApply(i, over);

							ui::style::box::r = .2;
							ui::style::box::g = .2;
							ui::style::box::b = .2;

							if (cmdParamDesc[i].param[n].value[0] == 0)
							{
								ui::style::box::r = 0.1;
								ui::style::box::g = 0.1;
								ui::style::box::b = 0.1;
							}

							ui::Box::Draw(x, y, note_step, h);
							CreateNoteText(cmdParamDesc[i].param[n].value[0]);
							
							int lp = 2;
							while (note_step < ui::Text::getTextLen("A#0", ui::style::text::width, lp+1) && lp >= 0)
							{
								noteText[lp--]=0;
							}
							ui::Text::Draw(noteText, x, y);

							if (over && ui::lbDown)
							{
								currentParam = n;
								cursorPos = 0;
								storedParam[0] = cmdParamDesc[i].param[currentParam].value[0];
								subParam = 0;
							}

						}
					}
				}
			}
		}
	}
	*/

bool Button(const char* str,float x, float y, float w,float h)
{
	bool over = isMouseOver(x, y, w,h);
	ui::style::BaseColor(ui::style::button::inverted);
	ui::style::box::outlineBrightness = over ? .25f : 0.1f;
	ui::Box::Draw(x, y, w, h);
	float th = ui::style::button::zoom ? (h*1.25 - h*ui::style::button::inner*2.) : ui::style::text::height;
	float tw = ui::Text::getTextLen(str, th);

	float tx = 0;
	float ty = 0;

	switch (ui::style::button::hAlign)
	{
	case ui::style::align_h::left:
		tx = x + ui::style::button::inner*h;
		break;
	case ui::style::align_h::center:
		tx = x + (w  - tw) / 2.;
		break;
	case ui::style::align_h::right:
		//todo
		break;
	}

	switch (ui::style::button::vAlign)
	{
	case ui::style::align_v::top:
		ty = y + ui::style::button::inner * h;
		break;
	case ui::style::align_v::center:
		ty = y + h/2.-th/4.;
		break;
	case ui::style::align_v::bottom:
		//todo
		break;

	}

	ui::Text::Draw(str, tx, ty, th, th);

	return over;
}



bool ButtonPressed(const char* str, float x, float y, float w, float h)
{
	return ui::lbDown & drag.isFree() & Button(str, x, y, w, h);
}

bool ButtonPressed(int cmdIndex, const char* str, float x, float y, float w, float h)
{
	if (ui::lbDown && drag.isFree()) currentCmd = cmdIndex;

	return ui::lbDown & drag.isFree() & Button(str, x, y, w, h);
}


void processSlider(int cmdIndex, std::string pName,float x, float y,float w,float h)
{

	int paramIndex = getParamIndexByStr(cmdIndex, pName.c_str());
	float range = cmdParamDesc[cmdIndex].param[paramIndex]._max - cmdParamDesc[cmdIndex].param[paramIndex]._min;
	ui::style::box::signed_progress = cmdParamDesc[cmdIndex].param[paramIndex]._min < 0 ? 1 :0;
	ui::style::box::progress_x = cmdParamDesc[cmdIndex].param[paramIndex].value[0] / range;
	std::string buttonText = pName + "::" + std::to_string(cmdParamDesc[cmdIndex].param[paramIndex].value[0]);
	if (ButtonPressed(cmdIndex,buttonText.c_str(), x, y, w, h))
	{
		storedParam[0] = cmdParamDesc[cmdIndex].param[paramIndex].value[0];
		drag.set(cmdIndex, paramIndex, 0);

		if (ui::dblClk && ui::style::box::signed_progress)
		{
			storedParam[0] = cmdParamDesc[cmdIndex].param[paramIndex].value[0] = 0;
			ui::dblClk = false;
		}
	}

	if (drag.check(cmdIndex, paramIndex, 0))
	{
		cmdParamDesc[cmdIndex].param[paramIndex].value[0] = storedParam[0] + ui::mouseDelta.x * dx11::width;
		pLimits(cmdIndex, paramIndex, 0);
	}
	ui::style::box::progress_x = 0;
}

void processSwitcher(int cmdIndex, std::string pName, float x, float y, float w, float h, const char* shortName = "")
{
	int paramIndex = getParamIndexByStr(cmdIndex, pName.c_str());
	ui::style::button::inverted = cmdParamDesc[cmdIndex].param[paramIndex].value[0] == 0 ? false : true;

	if (ButtonPressed(cmdIndex,shortName ? shortName : pName.c_str(), x, y, w, h))
	{
		drag.set(cmdIndex, paramIndex, 0);
		cmdParamDesc[cmdIndex].param[paramIndex].value[0] = 1 - cmdParamDesc[cmdIndex].param[paramIndex].value[0];
	}
}



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

			ui::Text::Setup();
			ui::Text::Draw(cmdParamDesc[currentCmd].param[i].name, x + insideX, y + insideY);

			for (int p = 0; p < subCount; p++)
			{
				float sel_ = currentParam == i ? 1.f : 0.f;
				sel_ *= (float)(subParam == p);
				setBStyle(sel_);

				bool mo = isMouseOver(_x, y + float(p) * lead, w, ui::style::box::height);

				ui::style::box::outlineBrightness = lerp(0.1f, 1.f, (float)mo);
				ui::style::box::a = cmdParamDesc[currentCmd].param[i].bypass ? .25f : 1.f;

				ui::Box::Setup();
				ui::Box::Draw(_x, y + float(p) * lead, w, ui::style::text::height * .8f);

				auto val =  getStrValue(sType, (int)cmdParamDesc[currentCmd].param[i].value[p]);

				setTStyle(sel_);
				ui::Text::Draw(val, x + valueDrawOffset, y + float(p) * lead + insideY);

				if (sel_) {
					inCurPos = ui::Text::getTextLen(vstr, ui::style::text::width, cursorPos);
					cursorPos = clamp(cursorPos, 0, (int)strlen(vstr));
				}
			}

			if (sel && isTypeEnum(sType) && getEnumCount(sType) == 2)
			{
				cmdParamDesc[currentCmd].param[i].value[0] = 1 - cmdParamDesc[currentCmd].param[i].value[0];
				action = false;
			}

			if (sel && isTypeEnum(sType) && getEnumCount(sType) != 2)
			{
				auto val = cmdParamDesc[currentCmd].param[i].value[0];

				for (int e = 0; e < getEnumCount(sType); e++)
				{
					float sel = e == val ? 1.f : 0.f;
					auto over = isMouseOver(__x, y + e * lead, w, ui::style::box::height);

					ui::style::box::r = ui::style::box::g = ui::style::box::b = .2f;
					ui::style::box::outlineBrightness = over ? 1.f : 0.1f;
					ui::Box::Setup();
					ui::Box::Draw(__x, y + e * lead, w, ui::style::text::height * .8f);
					
					ui::style::text::r = ui::style::text::g = ui::style::text::b = .8f;
					ui::Text::Draw(getStrValue(sType, e), x + enumDrawOffset, y + e * lead + insideY);

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


			if (sel && !isTypeEnum(sType))
			{
				showCursor = true;
				cursorX = x + valueDrawOffset + inCurPos;
				cursorY = y + insideY + subParam * lead;
			}

			pCountV += subCount;
			y += lead* subCount;
		}

		//drag by mouse
		if (!isTypeEnum(getTypeIndex(cmdParamDesc[currentCmd].param[currentParam].type)) && currentParam >= 0)
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

		if (ui::rbDown && !isKeyDown(CAM_KEY) && !isKeyDown(TIME_KEY))
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
