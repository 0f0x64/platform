bool isWatching = false;
HANDLE file = NULL;
OVERLAPPED overlapped;
BOOL success = false;
const int changeBufLen = 1024 * 4;
const int nameBufLen = 1024;
uint8_t change_buf[changeBufLen];

typedef struct {
	int srcFlieLine;
	int indexInStack;
} callInfo;

bool compareBySrcLine(const callInfo& a, const callInfo& b)
{
	return a.srcFlieLine < b.srcFlieLine;
}

void reflectSourceChanges(std::filesystem::path fileName)
{
	std::vector<callInfo> lines;
	bool empty = true;

	for (int i = 0; i < cmdCounter; i++)
	{
		auto fn = fileName.string();
		auto callerFn = std::string(cmdParamDesc[i].caller.fileName);
		
		if (!callerFn.compare(fn))
		{
			callInfo c = { cmdParamDesc[i].caller.line ,i };
			lines.push_back(c);
			empty = false;
		}
	}

	if (empty) return;

	std::sort(lines.begin(), lines.end(), compareBySrcLine);
	auto aaa = lines.size();
	using namespace std;

	string s;
	ifstream ifile(fileName);

	std::string caller;

	int lc = 1;
	if (ifile.is_open())
	{
		int currentLine = lines[0].srcFlieLine;
		int lineArrayCounter = 0;

		while (getline(ifile, s))
		{
			if (lc == currentLine)
			{
				//get all numeric params

				unsigned int pos = 0;
				int cCmd = lines[lineArrayCounter].indexInStack;
				pos = s.find(cmdParamDesc[cCmd].funcName);
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

					int pC = 0;
					for (int j=0;j<cmdParamDesc[cCmd].pCount;j++)
					{
						auto ts = getTypeDim(cmdParamDesc[cCmd].param[j].type);

						if (cmdParamDesc[cCmd].param[j].bypass) {
							pC+= ts; continue;
						}

						if (isTypeEnum(cmdParamDesc[cCmd].param[j].type))
						{
							auto evP = tokens[j].find("::") + 2;
							std::string eV = tokens[j].substr(evP,tokens[j].length()-evP);
						 	auto v = GetEnumValue(cmdParamDesc[cCmd].param[j].type, eV.c_str());
							if (v != INT_MAX)
							{
								cmdParamDesc[cCmd].param[j].value[0] = v;
							}
						}
						else
						{
							for (int k = 0; k < ts; k++)
							{
								auto t = tokens[pC + k];
								if (isNumber(t))
								{
									auto v = atoi(t.c_str());
									cmdParamDesc[cCmd].param[j].value[k] = v;
								}
							}
						}

						pC += ts;
					}
				}
				//

				lineArrayCounter++;

				if (lineArrayCounter == lines.size())
				{
					ifile.close();
					return;
				}

				currentLine = lines[lineArrayCounter].srcFlieLine;
			}


			lc++;
		}

/*		getline(ifile, s);

		unsigned int pos = 0;
		pos = s.find(cmdParamDesc[currentCmd].funcName);
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

			caller.append(cmdParamDesc[currentCmd].funcName);
			caller.append("(");

			int j = 0;
			for (auto& i : tokens)
			{
				if (cmdParamDesc[currentCmd].param[j].bypass)
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
		*/
	}

	ifile.close();

	auto a = lines;
}

void WatchFiles()
{
	const char* headerExtension = ".h";
	int shaderExtensionLen = strlen(Shaders::shaderExtension);
	int headerExtensionLen = strlen(headerExtension);

	if (!isWatching)//init
	{

		Log("watching for changes: ");
		Log(userSpacePath);
		Log("\n");

		file = CreateFile(userSpacePath,
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			NULL);

		assert(file != INVALID_HANDLE_VALUE);

		overlapped.hEvent = CreateEvent(NULL, FALSE, 0, NULL);

		success = ReadDirectoryChangesW(
			file, change_buf, changeBufLen, TRUE,
			FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
			NULL, &overlapped, NULL);

		isWatching = true;
	}

	//--

	DWORD result = WaitForSingleObject(overlapped.hEvent, 0);

	if (result == WAIT_OBJECT_0)
	{
		DWORD bytes_transferred;
		GetOverlappedResult(file, &overlapped, &bytes_transferred, FALSE);

		FILE_NOTIFY_INFORMATION* event = (FILE_NOTIFY_INFORMATION*)change_buf;

		for (;;)
		{
			DWORD name_len = event->FileNameLength / sizeof(wchar_t);

			switch (event->Action)
			{
			case FILE_NOTIFY_CHANGE_FILE_NAME:
			case FILE_ACTION_MODIFIED:
			case FILE_ACTION_RENAMED_NEW_NAME:
			{
				char fileName[nameBufLen];
				DWORD offset = 0;

				do
				{
					memset(fileName, NULL, nameBufLen);
					event = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&change_buf[offset]);
					WideCharToMultiByte(CP_ACP, NULL, event->FileName, event->FileNameLength / sizeof(WCHAR), fileName, sizeof(fileName), NULL, NULL);

					char modifedExt[nameBufLen];
					strcpy(modifedExt, Shaders::shaderExtension);
					strcat(modifedExt, "~RF");//visual studio only hack!

					
					char* h = strstr(fileName, headerExtension);
					if (h)
					{
						char s2[nameBufLen];
						memset(s2, NULL, nameBufLen);
						ptrdiff_t bytes = ((char*)h) - ((char*)fileName) + headerExtensionLen;
						memcpy(s2, fileName, bytes);
						s2[bytes + 1] = 0;

						Log("Modified:  ");
						Log(s2);
						Log("\n");

						char fn[1024];
						strcpy(fn, userSpacePath);
						strcat(fn, "\\");
						strcat(fn, s2);

						std::filesystem::path p = fn;
						auto ap = std::filesystem::absolute(p);

						reflectSourceChanges(ap);

					}

					char* s = strstr(fileName, modifedExt);
					if (s)
					{
						char s2[nameBufLen];
						memset(s2, NULL, nameBufLen);
						ptrdiff_t bytes = ((char*)s) - ((char*)fileName) + shaderExtensionLen;
						memcpy(s2, fileName, bytes);
						s2[bytes + 1] = 0;

						Log("Modified:  ");
						Log(s2);
						Log("\n");

						char pureName[255];
						char* pShadersDir = strstr(s2, "\\");
						char* pSlash = strstr(pShadersDir, "\\");
						strcpy(pureName, pSlash+4);
						//strcpy(pureName, s2 + 3);// 3= len vs/ps with backslash
						pureName[strlen(pureName) - shaderExtensionLen] = 0;

						//char s3[255];
						//strcpy(s3, "/");
						//strcat(s3, s2);

						// detect vertex/pixel shader and slot
						if (pSlash[1] == 'v')
						{
							int i = 0;
							while (i < Shaders::vsCount)
							{
								if (!strcmp(Shaders::vsList[i], pureName))
								{
									Shaders::CreateVS(i, pSlash);
									break;
								}
								i++;
							}

						}

						if (pSlash[1] == 'p')
						{
							int i = 0;
							while (i < Shaders::psCount)
							{
								if (!strcmp(Shaders::psList[i], pureName))
								{
									//    / ps\basic.shader
									Shaders::CreatePS(i, pSlash);
									break;
								}
								i++;
							}

						}

						if (pSlash[1] == 'l')
						{
							Shaders::Init();
						}

					}

					offset += event->NextEntryOffset;
				} while (event->NextEntryOffset != 0);


			}
			break;
			}

			break;

		}

		// Queue the next event

		BOOL success = ReadDirectoryChangesW(
			file, change_buf, changeBufLen, TRUE,
			FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
			NULL, &overlapped, NULL);

	}
}
