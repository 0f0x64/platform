bool isWatching = false;
HANDLE file = NULL;
OVERLAPPED overlapped;
BOOL success = false;
const int changeBufLen = 1024 * 4;
const int nameBufLen = 1024;
uint8_t change_buf[changeBufLen];

bool codeRecompiled = false;
bool justSaved = false;

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

						if (editor::justSaved)
						{
							editor::justSaved = false;
						}
						else 
						{
							paramsAreLoaded = false;
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

						if (pShadersDir[1] == 'u' && pShadersDir[2] == 'i')
						{
							pShadersDir +=3;
						}

						char* pSlash = strstr(pShadersDir, "\\");
						strcpy(pureName, pSlash+4);
						pureName[strlen(pureName) - shaderExtensionLen] = 0;

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
