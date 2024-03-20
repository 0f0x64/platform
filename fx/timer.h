namespace timer
{

	double prevtick = 0.0;
	double fp_delta = 0.0;

	double PCFreq = 0.0;
	__int64 CounterStart = 0;

	double StartTime = 0;
	double currentTime = 0;

	void StartCounter()
	{
		LARGE_INTEGER li;
		if (!QueryPerformanceFrequency(&li)) MessageBox(hWnd, "timer fail", NULL, 0);

		PCFreq = double(li.QuadPart) / 1000.0;

		QueryPerformanceCounter(&li);
		CounterStart = li.QuadPart;
	}

	double GetCounter()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return double(li.QuadPart - CounterStart) / PCFreq;
	}

}