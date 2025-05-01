bool isWatching = false;
HANDLE file = NULL;
OVERLAPPED overlapped;
BOOL success = false;
const int changeBufLen = 1024 * 4;
const int nameBufLen = 1024;
uint8_t change_buf[changeBufLen];

bool codeRecompiled = false;
bool justSaved = false;

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
	paramsAreLoaded = false;
	return;

	std::vector<callInfo> lines;
	bool empty = true;

	for (int i = 0; i < cmdCounter; i++)
	{
		if (cmdParamDesc[i].reflection_type == 0)
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
					const auto tCount = tokens.size();

					int pC = 0;
					for (int j=0;j<cmdParamDesc[cCmd].pCount;j++)
					{
						auto typeIndex = getTypeIndex(cmdParamDesc[cCmd].param[j].type);
						auto ts = getTypeDim(typeIndex);

						if (cmdParamDesc[cCmd].param[j].bypass) {
							pC+= ts; continue;
						}

						if (isTypeEnum(typeIndex))
						{
							auto evP = tokens[pC].find("::") + 2;
							std::string eV = tokens[pC].substr(evP,tokens[pC].length()-evP);
						 	auto v = GetEnumValue(typeIndex, eV.c_str());
							if (v != INT_MAX)
							{
								cmdParamDesc[cCmd].param[j].value[0] = v;
							}
						}
						else
						{
							for (unsigned int k = 0; k < (unsigned int)ts; k++)
							{
								if (pC + k>= tCount)
								{
									cmdParamDesc[cCmd].param[j].value[k] = 0;
								}
								else
								{
									auto t = tokens[pC + k];
									if (isNumber(t))
									{
										auto v = atoi(t.c_str());
										cmdParamDesc[cCmd].param[j].value[k] = v;
									}
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
	}

	ifile.close();
}

//#define SUBSCRIBE FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME
#define SUBSCRIBE FILE_NOTIFY_CHANGE_FILE_NAME

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
			SUBSCRIBE,
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
			auto a = event;

			switch (event->Action)
			{
			case FILE_ACTION_ADDED:
			//case FILE_NOTIFY_CHANGE_FILE_NAME:
			//case FILE_ACTION_MODIFIED:
			//case FILE_ACTION_RENAMED_NEW_NAME:
			//case FILE_ACTION_RENAMED_OLD_NAME:
			{
				char fileName[nameBufLen];
				DWORD offset = 0;

				while (event->NextEntryOffset != 0) {
					event = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&change_buf[offset]);
					offset += event->NextEntryOffset;
				};

					memset(fileName, NULL, nameBufLen);
					//event = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&change_buf[offset]);
					WideCharToMultiByte(CP_ACP, NULL, event->FileName, event->FileNameLength / sizeof(WCHAR), fileName, sizeof(fileName), NULL, NULL);

					char modifedExt[nameBufLen];
					strcpy(modifedExt, Shaders::shaderExtension);
					strcat(modifedExt, "~RF");//visual studio only hack!

					#if SRC_WATCH
					char* h = strstr(fileName, headerExtension);
					if (h)
					{
						char s2[nameBufLen];
						memset(s2, NULL, nameBufLen);
						ptrdiff_t bytes = ((char*)h) - ((char*)fileName) + headerExtensionLen;
						memcpy(s2, fileName, bytes);
						s2[bytes + 1] = 0;

						char fn[1024];
						strcpy(fn, userSpacePath);
						strcat(fn, "\\");
						strcat(fn, s2);

						std::filesystem::path p = fn;
						auto ap = std::filesystem::absolute(p);

						if (justSaved)
						{
							justSaved = false;
						}
						else {
							reflectSourceChanges(ap);

							Log("Modified: ");
							Log(s2);
							Log(" - data updated from source file");
							Log("\n");
						}

					}
					#endif	

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




			}
			break;
			}

			break;

		}

		// Queue the next event

		BOOL success = ReadDirectoryChangesW(
			file, change_buf, changeBufLen, TRUE,
			SUBSCRIBE,
			NULL, &overlapped, NULL);

	}
}

//------------
bool isWatchingRecompilation = false;
HANDLE fileRec = NULL;
OVERLAPPED overlappedRec;
BOOL successRec = false;
const int changeBufLen_rec = 1024 * 4;
uint8_t change_buf_rec[changeBufLen_rec];

void WatchForRecompilation()
{
	const char* objExtension = ".obj";
	int objExtensionLen = strlen(objExtension);

	if (!isWatchingRecompilation)//init
	{
		Log("watching for recompilation ");
		Log(objPath);
		Log("\n");

		fileRec = CreateFile(objPath,
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			NULL);

		assert(fileRec != INVALID_HANDLE_VALUE);

		overlappedRec.hEvent = CreateEvent(NULL, FALSE, 0, NULL);

		successRec = ReadDirectoryChangesW(
			fileRec, change_buf_rec, changeBufLen_rec, TRUE,
			FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
			NULL, &overlappedRec, NULL);

		isWatchingRecompilation = true;
	}

	//--

	DWORD result = WaitForSingleObject(overlappedRec.hEvent, 0);

	if (result == WAIT_OBJECT_0)
	{
		DWORD bytes_transferred;
		GetOverlappedResult(fileRec, &overlappedRec, &bytes_transferred, FALSE);

		FILE_NOTIFY_INFORMATION* event = (FILE_NOTIFY_INFORMATION*)change_buf_rec;

		for (;;)
		{
			DWORD name_len = event->FileNameLength / sizeof(wchar_t);

			switch (event->Action)
			{
			case FILE_ACTION_RENAMED_OLD_NAME:
			{
				char fileName[nameBufLen];
				DWORD offset = 0;

				while (event->NextEntryOffset != 0)
				{
					event = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&change_buf_rec[offset]);
					offset += event->NextEntryOffset;
				};

				memset(fileName, NULL, nameBufLen);
				WideCharToMultiByte(CP_ACP, NULL, event->FileName, event->FileNameLength / sizeof(WCHAR), fileName, sizeof(fileName), NULL, NULL);

				if (strstr(fileName, objExtension))
				{
					codeRecompiled = true;
					Log("recompilation\n");
				}

			}
			break;
			}

			break;
		}

		// Queue the next event

		BOOL success = ReadDirectoryChangesW(
			fileRec, change_buf_rec, changeBufLen_rec, TRUE,
			FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
			NULL, &overlappedRec, NULL);

	}
}
