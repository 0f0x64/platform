namespace paramEdit {

	void showStackItem(int i, float& x, float& y, float w, float lead, float sel);

	void removeDoubleSpaces(std::string& str) {
		std::regex pattern("\\s{2,}");
		str = std::regex_replace(str, pattern, " ");
	}

	void getTypeAndName(std::string& src, std::string& typeStr, std::string& nameStr)
	{
		bool type = false;
		bool name = false;

		for (int j = 0; j < src.size(); j++)
		{
			char a = src.at(j);

			if (a != ' ' && typeStr.empty()) type = true;
			if (a == ' ' && !typeStr.empty()) type = false;

			if (a != ' ' && !typeStr.empty() && !type) name = true;
			if (a == ' ' && !typeStr.empty() && !nameStr.empty()) break;

			if (type) typeStr += a;
			if (name) nameStr += a;

		}
	}


	void SetParamType(int index, const char* type)
	{
		strcpy(cmdParamDesc[cmdCounter].param[index].type, type);
		cmdParamDesc[cmdCounter].param[index].typeIndex = getTypeIndex(type);
		cmdParamDesc[cmdCounter].param[index].size = sizeof(int);
	}

	void SetParamName(int index, const char* name)
	{
		strcpy(cmdParamDesc[cmdCounter].param[index].name, name);
	}

	void FillCaller(const std::source_location caller)
	{
		strcpy(cmdParamDesc[cmdCounter].caller.fileName, caller.file_name());
		cmdParamDesc[cmdCounter].caller.line = caller.line();
	}

	void CopyStrWithSkipper(std::string& in, std::string& lineStr, std::string skippers)
	{
		unsigned int t = 0;
		while (t < in.length())
		{
			bool passed = true;
			for (int i = 0; i < skippers.size(); i++)
			{
				if (in.at(t) == skippers.at(i)) passed = false;
			}

			if (passed)	lineStr += in.at(t);


			t++;
		}
	}

	bool isParam(std::string line)
	{
		char* p;
		strtol(line.c_str(), &p, 10);
		return *p == 0;
	}

	void CreateCallStr(int cmdIndex, std::string& callStr, std::string& tab)
	{
		auto c = &cmdParamDesc[cmdIndex];
		std::string funcStr = c->funcName;
		callStr = funcStr;
		callStr += "({";
			if (!c->single_line)
			{
				callStr += "\n";
			}
		
		for (int i = 0; i < c->pCount; i++)
		{
			if (!c->single_line)
			{
				callStr += tab;
				callStr += "\t";
			}

			if (c->init_with_names)
			{
				callStr += ".";
				callStr += c->param[i].name;
				callStr += " = ";
			}


			if (c->param[i].bypass)
			{
				callStr += c->param[i].strValue;
			}
			else
			{
				if (isTypeEnum(c->param[i].typeIndex))
				{
					callStr += c->param[i].type;
					callStr += "::";
					callStr += getStrValue(c->param[i].typeIndex, c->param[i].value);
				}
				else
				{
					callStr += std::to_string(c->param[i].value);
				}
			}

			if (i != c->pCount - 1) callStr += ",";

			if (!c->single_line) { callStr += "\n"; }
		}

		if (!c->single_line) {
			callStr += tab;
		}
		
		callStr += "});";
	}

	void SaveToSource(int cmdIndex)
	{
		if (cmdIndex < 0) return;
		auto c = &cmdParamDesc[cmdIndex];

		if (c->pCount == 0) return;

		std::string callStr;
		

		//save
		const char* filename = c->caller.fileName;
		const int lineNum = c->caller.line;

		using namespace std;
		string inFilePath = filename;
		string outFilePath = inFilePath + "_";

		editor::justSaved = true;
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

			std::string tab;
			int nc = 0;
			int cCount = 1;
			while (true)
			{
				char a;
				ifile.get(a);

				if (a == c->funcName[nc])
				{
					nc++;
				}
				else
				{
					nc = 0;
				}

				tab += a;

				
				cCount++;

				if (nc == strlen(c->funcName))
				{
					break;
				}

			}
			tab.erase(tab.size() - nc,nc);

			ofile << tab;

			for (int i = 0; i < tab.size(); i++)
			{
				if (tab.at(i) != ' ' && tab.at(i) != '\t') tab.at(i) = ' ';
			}

			CreateCallStr(cmdIndex, callStr, tab);

			ofile << callStr;

			while (true)
			{
				char a;
				ifile.get(a);
				if (a == ';') break;
			}

			while (true)
			{
				char a;
				ifile.get(a);
				ofile << a;
				if (a == '\n') break;
			}


			while (getline(ifile, s))
			{
				ofile << s << "\n";
			}

			ifile.close();
			ofile.close();

			remove(inFilePath.c_str());
			rename(outFilePath.c_str(), inFilePath.c_str());

			Log("Modified:  ");
			Log(inFilePath.c_str());
			Log("\n");
		}


	}

	void reflect_f(auto* in, const std::source_location caller, const std::source_location currentFunc)//name and types without names
	{
		auto c = &cmdParamDesc[cmdCounter];

		if (!paramsAreLoaded)  //variables -> reflected struct
		{
			FillCaller(caller);

			std::string fn = currentFunc.function_name();
			auto rb = fn.find("(");
			auto fp = fn.rfind("::", rb);
			auto op = fn.rfind("::", fp - 2);
			if (op == std::string::npos)
			{
				op = fn.rfind(" ", fp - 2)-1;
			}
			std::string objName = fn.substr(op + 2, fp - op - 2);
			std::string funcName = fn.substr(fp + 2, rb - fp - 2);;

			//auto fnStart = fn.rfind(" ", rb)+1;
			//std::string fullName = fn.substr(fnStart, rb - fnStart);

			strcpy(c->funcName, (objName + "::" + funcName).c_str());
			std::ifstream ifile(currentFunc.file_name());
			std::string s;

			int lc = 1;

			bool obj_is_found = false;

			if (ifile.is_open())
			{
				while (true)
				{
					if (!getline(ifile, s)) break;
					if (lc == currentFunc.line()) break;

					std::string lineStr;
					CopyStrWithSkipper(s, lineStr, " \t");

					if (std::string::npos != lineStr.find("namespace" + objName))
					{
						obj_is_found = true;
					}

					if (obj_is_found)
					{
						std::string Declaration = "cmd(" + funcName;
						auto cmdDecl = lineStr.find(Declaration);
						if (std::string::npos != cmdDecl)
						{
							std::string pStr;

							if (std::string::npos == lineStr.find(")", cmdDecl))
							{
								while (true)
								{
									char a;
									ifile.get(a);
									s += a;
									if (a == ')') break;
								}
							}

							auto cmdOfs = s.find("cmd");
							auto commaOfs = s.find(",",cmdOfs);

							if (commaOfs == std::string::npos)//no params
							{
								c->pCount = 0;
								c->stackLevel = cmdLevel;
								c->uiDraw = &showStackItem;
								cmdLevel++;
								cmdCounter++;
								return;
							}

							auto end = s.find(")");
							end = s.rfind(";", end);
							pStr = s.substr(commaOfs + 1, end -1 - commaOfs);
							std::erase(pStr, '\t');
							std::erase(pStr, '\n');


							const std::regex reg{ R"(;)" };
							auto tokens = regex_split(pStr, reg);

							int param_ofs = 0;
							int j = 0;
							for (int i = 0; i < tokens.size(); i++)
							{
								std::string typeStr, nameStr;
								getTypeAndName(tokens[i], typeStr, nameStr);
								auto typeID = getTypeIndex(typeStr.c_str());
								c->param[j].typeIndex = typeID;
								c->param[j]._min = typeID == -1 ? INT_MIN : typeDesc[typeID]._min;
								c->param[j]._max = typeID == -1 ? INT_MAX : typeDesc[typeID]._max;

								auto br = nameStr.find("[");
								int cnt = 1;
								if (br != std::string::npos)
								{
									auto rbr = nameStr.find("]");
									auto cs = nameStr.substr(br + 1, rbr - br);
									cnt = std::stoi(cs);

								}

								for (int k=0;k<cnt;k++)
								{
									SetParamType(j, typeStr.c_str());
									SetParamName(j, nameStr.c_str());
									c->param[j].offset = param_ofs;
									param_ofs += c->param[i].size;
									j++;
									
								}
							}

							c->pCount = j;

							break;
						}
					}

					lc++;
				}

				ifile.close();
			}
			else
			{
				Log(currentFunc.file_name());
				Log(" - file reading error\n");
			}


			//get caller string
			std::ifstream ifileCaller(caller.file_name());

			std::string funcStr;
			lc = 1;

			if (ifileCaller.is_open())
			{
				while (true)
				{
					if (!getline(ifileCaller, s)) break;
					if (lc == caller.line()) break;

					lc++;
				}
				if (s.find(";") !=std::string::npos)
				{
					c->single_line = true;
				}
				else
				{
					c->single_line = false;
				}

				auto s2 = s;
				std::erase(s2, ' ');
				std::erase(s2, '\t');

				auto br = s2.find("(");
				auto fcn = s2.substr(0, br);

				strcpy(c->funcName, fcn.c_str());


				auto pStart = s.find("{") + 1;
				std::string funcStr = s.substr(pStart, funcStr.size()-pStart);
				auto pEnd = funcStr.find("}");
				if (std::string::npos == pEnd)
				{
					while (true)
					{
						char a;
						ifileCaller.get(a);
						if (a == '}') break;
						funcStr += a;
						
					}
				}
				else
				{
					funcStr = funcStr.substr(0, pEnd);
				}

				std::erase(funcStr, ' ');
				std::erase(funcStr, '\t');
				std::erase(funcStr, '\n');

				std::string paramStr = funcStr;

				const std::regex reg{ R"(,)" };
				auto pTokens = regex_split(paramStr, reg);
				c->pCount = pTokens.size();
				//TODO - prevent missmatch pTokens.size with pCount
				for (int i = 0; i < c->pCount; i++)
				{
					std::string pname;
					std::string pvalue;
					auto pId = i;
					auto eqPos = pTokens[i].find('=');

					if (eqPos != std::string::npos)//init with field names
					{
						pname = pTokens[i].substr(1, eqPos - 1);
						pId = getParamIndexByStr(cmdCounter, pname.c_str());
						pvalue = pTokens[i].substr(pTokens[i].find('=') + 1, pTokens[i].size());
						c->init_with_names = true;
					}
					else //init without field names
					{
						pvalue = pTokens[i];
						c->init_with_names = false;
					}

					c->param[i].bypass = false;

					if (pvalue.length()>0)
					{

						if (isTypeEnum(cmdParamDesc[cmdCounter].param[pId].typeIndex))
						{
							std::string pTypeStr = cmdParamDesc[cmdCounter].param[pId].type;
							pTypeStr += "::";
							if (pvalue.find(pTypeStr) == std::string::npos)
							{
								c->param[i].bypass = true;
								strcpy(c->param[i].strValue, pvalue.c_str());
								c->param[i].value = *(int*)((char*)in + c->param[i].offset);
							}
							else {
								auto enumStart = pvalue.find("::") + 2;
								std::string enumStr = pvalue.substr(enumStart, pvalue.size() - enumStart);
								c->param[i].value = GetEnumValue(c->param[i].typeIndex, enumStr.c_str());
							}
						}
						else
						{
							if (!isParam(pvalue)) {
								c->param[i].bypass = true;
								c->param[i].value = *(int*)((char*)in + c->param[i].offset);
								strcpy(c->param[i].strValue, pvalue.c_str());
							}
							else
							{
								c->param[i].value = std::stoi(pvalue);
							}

						}
					}
				}

				ifileCaller.close();
			}
			else
			{
				Log(caller.file_name());
				Log(" - file reading error\n");
			}
		}
		
		
		//variables <- reflected struct
		{

			for (int i = 0; i < c->pCount; i++)
			{
				*(int*)((char*)in + c->param[i].offset) = c->param[i].value;
			}
		}

		c->stackLevel = cmdLevel;
		c->uiDraw = &showStackItem;
		cmdLevel++;

		cmdCounter++;

	}

	//-------------------

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
			}
			else
			{
				cmdParamDesc[cmdCounter].param[i]._min = typeDesc[pid]._min;
				cmdParamDesc[cmdCounter].param[i]._max = typeDesc[pid]._max;
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
					
					j+= 1;
				}
			}
		}

		ifile.close();

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

				if (ui::lbDown && i != currentCmd && drag.isFree()) {
					action = true;
					currentCmd = i;
					currentParam = -1;
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
				float _x = 0;
				float _y = 0;
				float _z = 0;

				if (isType(cmdParamDesc[i].param[j].type, "pos_x"))
				{
					_x = (float)cmdParamDesc[i].param[j].value;
					_y = (float)cmdParamDesc[i].param[j + 1].value;
					_z = (float)cmdParamDesc[i].param[j + 2].value;
				}

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

				int _x = cmdParamDesc[i].param[j].value;

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
					storedParam = cmdParamDesc[i].param[j].value;
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
						cmdParamDesc[i].param[j].value = storedParam + TimeLine::ScreenToTime( ui::mouseDelta.x)/SAMPLES_IN_FRAME;
						pLimits(currentCmd,j);
					}
					else
					{
						storedParam = cmdParamDesc[i].param[j].value;
					}
				}

				ui::Box::Draw(px, py, h * dx11::aspect, h);

			}
		}
	}

	

bool ShowButton(const char* str,float x, float y, float w,float h, bool over)
{

	ui::style::BaseColor(ui::style::button::inverted);
	ui::style::box::outlineBrightness = over ? .25f : 0.1f;
	ui::Box::Draw(x, y, w, h);
	float th = ui::style::button::zoom ? (h*1.25f - h*ui::style::button::inner*2.f) : ui::style::text::height;
	float tw = ui::Text::getTextLen(str, th);

	if (ui::style::button::zoom)
	{
		th *= w / (tw+ w*ui::style::button::inner * 2.f);
		tw *= w / (tw + w*ui::style::button::inner * 2.f);
	}

	float tx = 0;
	float ty = 0;

	switch (ui::style::button::hAlign)
	{
	case ui::style::align_h::left:
		tx = x + ui::style::button::inner*h;
		break;
	case ui::style::align_h::center:
		tx = x + (w  - tw) / 2.f;
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
		ty = y + h/2.f-th/4.f;
		break;
	case ui::style::align_v::bottom:
		//todo
		break;
	}

	ui::Text::Draw(str, tx, ty, th, th);

	return over;
}


bool Button(int cmdIndex, const char* str, float x, float y, float w, float h)
{
	bool over = isMouseOver(x, y, w, h);
	if (over && ui::lbDown && drag.isFree()) currentCmd = cmdIndex;
	return ShowButton(str, x, y, w, h, over);
}

bool Button(const char* str, float x, float y, float w, float h)
{
	bool over = isMouseOver(x, y, w, h);
	return ShowButton(str, x, y, w, h, over);
}

bool ButtonPressed(int cmdIndex, const char* str, float x, float y, float w, float h)
{
	return ui::lbDown & drag.isFree() & Button(cmdIndex, str, x, y, w, h);
}

bool ButtonPressed(const char* str, float x, float y, float w, float h)
{
	return ui::lbDown & drag.isFree() & Button(str, x, y, w, h);
}

enum class dir {x,y,r};

void processSlider(int cmdIndex, std::string pName,float x, float y,float w,float h, dir direction)
{

	int paramIndex = getParamIndexByStr(cmdIndex, pName.c_str());
	float range = (float)(cmdParamDesc[cmdIndex].param[paramIndex]._max - cmdParamDesc[cmdIndex].param[paramIndex]._min);
	ui::style::box::signed_progress = cmdParamDesc[cmdIndex].param[paramIndex]._min < 0.f ? 1.f :0.f;
	
	ui::style::box::progress = cmdParamDesc[cmdIndex].param[paramIndex].value / range;;
	ui::style::box::slider_type = (int)direction+1;
		
	std::string buttonText = pName +"::" + std::to_string(cmdParamDesc[cmdIndex].param[paramIndex].value);
	if (ButtonPressed(cmdIndex,buttonText.c_str(), x, y, w, h))
	{
		storedParam = cmdParamDesc[cmdIndex].param[paramIndex].value;
		drag.set(cmdIndex, paramIndex);

		if (ui::dblClk && ui::style::box::signed_progress)
		{
			storedParam = cmdParamDesc[cmdIndex].param[paramIndex].value = 0;
			ui::dblClk = false;
		}
	}

	if (drag.check(cmdIndex, paramIndex))
	{
		float delta = direction == dir::y ? -ui::mouseDelta.y : ui::mouseDelta.x;
		delta *= dx11::width;
		cmdParamDesc[cmdIndex].param[paramIndex].value = (int)(storedParam + delta);
		pLimits(cmdIndex, paramIndex);
	}
	ui::style::box::progress = 0;
}

void processSwitcher(int cmdIndex, std::string pName, float x, float y, float w, float h, const char* shortName = "")
{
	int paramIndex = getParamIndexByStr(cmdIndex, pName.c_str());
	ui::style::button::inverted = cmdParamDesc[cmdIndex].param[paramIndex].value == 0 ? false : true;

	if (ButtonPressed(cmdIndex,shortName ? shortName : pName.c_str(), x, y, w, h))
	{
		drag.set(cmdIndex, paramIndex);
		cmdParamDesc[cmdIndex].param[paramIndex].value = 1 - cmdParamDesc[cmdIndex].param[paramIndex].value;
	}
}



	void showParams()
	{
		if (currentCmd < 0) return;
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

			if (!cmdParamDesc[currentCmd].param[i].bypass)
			{

				if (isMouseOver(_x, y, w, ui::style::box::height))
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
								storedParam = cmdParamDesc[currentCmd].param[currentParam].value;
							}

						}
				}
				
			}

			float sel = currentParam == i ? 1.f : 0.f;
			set2Style(sel);

			ui::Text::Setup();
			ui::Text::Draw(cmdParamDesc[currentCmd].param[i].name, x + insideX, y + insideY);

			
			
			float sel_ = currentParam == i ? 1.f : 0.f;
			setBStyle(sel_);

			bool mo = isMouseOver(_x, y, w, ui::style::box::height);

			ui::style::box::outlineBrightness = lerp(0.1f, 1.f, (float)mo);
			ui::style::box::a = cmdParamDesc[currentCmd].param[i].bypass ? .25f : 1.f;

			ui::Box::Setup();
			ui::Box::Draw(_x, y, w, ui::style::text::height * .8f);

			auto val =  getStrValue(sType, (int)cmdParamDesc[currentCmd].param[i].value);

			setTStyle(sel_);
			ui::Text::Draw(val, x + valueDrawOffset, y + insideY);

			if (sel_) {
				inCurPos = ui::Text::getTextLen(vstr, ui::style::text::width, cursorPos);
				cursorPos = clamp(cursorPos, 0, (int)strlen(vstr));
			}
			

			if (sel && isTypeEnum(sType) && getEnumCount(sType) == 2)
			{
				cmdParamDesc[currentCmd].param[i].value = 1 - cmdParamDesc[currentCmd].param[i].value;
				action = false;
			}

			if (sel && isTypeEnum(sType) && getEnumCount(sType) != 2)
			{
				auto val = cmdParamDesc[currentCmd].param[i].value;

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
							cmdParamDesc[currentCmd].param[i].value = e;
						}
					}
				}
			}


			if (sel && !isTypeEnum(sType))
			{
				showCursor = true;
				cursorX = x + valueDrawOffset + inCurPos;
				cursorY = y + insideY;
			}

			pCountV += 1;
			y += lead;
		}

		//drag by mouse
		if (!isTypeEnum(getTypeIndex(cmdParamDesc[currentCmd].param[currentParam].type)) && currentParam >= 0)
		{
			if (ui::lbDown)
			{
				cmdParamDesc[currentCmd].param[currentParam].value = storedParam + (int)(ui::mouseDelta.x * width);
				pLimits();
			}
			else
			{
				storedParam = cmdParamDesc[currentCmd].param[currentParam].value;
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

		Rasterizer::Scissors(rect{ 0, (int)(top*dx11::height), dx11::width, (int)(bottom*dx11::height) });

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

		Rasterizer::Scissors(rect{ 0, 0, width, height });

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

	}

}
