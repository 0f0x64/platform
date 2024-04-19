namespace TimeLine
{

	#define fps 60
	#define freq 44100
	#define frameLen freq/fps
	#define second freq
	#define minute freq*60

	float screenLeft = .25f;
	float screenRight = .75f;
	int zoomOut = minute * 10;
	int pos = 0;
	int end;

	float TimeToScreen(int time)
	{
		float x = time / (float)zoomOut;
		return x;
	}

	int ScreenToTime(double x)
	{
		int time = (int)(x * zoomOut);
		return time;
	}

	char timeStr[10];

	void TimeToStr(int time)
	{
		char minStr[3];
		char secStr[3];

		int _min = time / (minute);
		int _sec = (time % (minute)) / (second);

		_itoa(_min, minStr, 10);
		_itoa(_sec, secStr, 10);
		strcpy(timeStr, minStr);
		strcat(timeStr, ":");
		strcat(timeStr, secStr);
	}

	int GetAdaptiveStep(int minTimeStep)
	{
		int step = minute;
		if (second * 30 > minTimeStep) step = second * 30;
		if (second * 15 > minTimeStep) step = second * 15;
		if (second * 10 > minTimeStep) step = second * 10;
		if (second * 5  > minTimeStep) step = second * 5;
		if (second > minTimeStep) step = second;
		if (frameLen > minTimeStep) step = frameLen;
		return step;
	}

	void DrawMakers()
	{
		api.blend(blendmode::off);

		float left = screenLeft + TimeToScreen(pos);
		float right = screenRight + TimeToScreen(pos);
		int minTimeStep = ScreenToTime(16.f / width);
		int step = GetAdaptiveStep(minTimeStep);

		int screenEnd = ScreenToTime(right);
		end = min(end, screenEnd);
		int start = ScreenToTime(left) / step;
		int iter = end / step;

		api.setIA(topology::lineList);

		int counter = 0;

		for (int i = start; i < iter; i++)
		{
			int time = i * step;
			float x = TimeToScreen(time) - left + screenLeft;
			float h = ui::style::text::height / 8.f;
			if (time % (second) == 0) h *= 2;
			if (time % (minute) == 0) h *= 2;

			ui::lineBuffer[counter].x = x;
			ui::lineBuffer[counter].y = 1;
			ui::lineBuffer[counter].x1 = x;
			ui::lineBuffer[counter].y1 = 1 - h;

			counter++;
		}

		ui::ShowLine(counter);
	}

	void DrawTimeStamps()
	{
		float left = screenLeft + TimeToScreen(pos);
		float right = screenRight + TimeToScreen(pos);

		int screenEnd = ScreenToTime(right);
		end = min(end, screenEnd);

		using namespace ui::style;

		Base();

		api.setIA(topology::triList);
		api.blend(blendmode::alpha, blendop::add);

		ps::letter_ps.samplers.s1Filter = filter::linear;
		ps::letter_ps.samplers.s1AddressU = addr::clamp;
		ps::letter_ps.samplers.s1AddressV = addr::clamp;
		ps::letter_ps.textures.tex = tex::font;

		int minTimeStep = ScreenToTime(ui::style::text::height * 5.f);
		int step = GetAdaptiveStep(minTimeStep);
		int iter = end / step;
		int start = (int)(ceil(ScreenToTime(left) / (double)step));

		for (int i = start; i <= iter; i++)
		{
			int time = i * step;
			float x = TimeToScreen(time) - left + screenLeft;

			if ((time % (second) == 0) || (time % (minute) == 0))
			{
				TimeToStr(time);
				ui::ShowText(timeStr, x, 1.f - ui::style::text::height * 1.5f);
			}
		}
	}

	void Draw()
	{
		pos = (int)(timer::frameBeginTime * 1111.1f);
		end = minute * 10;

		DrawMakers();
		DrawTimeStamps();
	}

}
