namespace timer
{
	double PCFreq = 0.0;
	__int64 counterStart = 0;

	double startTime = 0;
	double frameBeginTime = 0;
	double frameEndTime = 0;
	double nextFrameTime = 0;
	double frameRenderingDuration = 0.0;

	void StartCounter()
	{
		LARGE_INTEGER li;

		#if Debug
			if (!QueryPerformanceFrequency(&li)) MessageBox(hWnd, "timer fail", NULL, 0);
		#else
			QueryPerformanceFrequency(&li);
		#endif	

		PCFreq = double(li.QuadPart) / 1000.0;

		QueryPerformanceCounter(&li);
		counterStart = li.QuadPart;
	}

	double GetCounter()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return double(li.QuadPart - counterStart) / PCFreq;
	}

}