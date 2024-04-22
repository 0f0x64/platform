namespace TimeLine
{

	const int TIME_KEY = VK_LMENU;

	const int frame = SAMPLES_IN_FRAME;
	const int second = SAMPLING_FREQ;
	const int minute = (SAMPLING_FREQ * FRAMES_PER_SECOND);
	const int timelineLen = minute * 10;

	float screenLeft = .25f;
	float screenRight = .75f;
	
	int zoomOut = timelineLen/ (screenRight- screenLeft);

	int posLast = 0;

	bool play = false;

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

	int pos = (-ScreenToTime(screenLeft));


	int timeCursorLast = 0;

	char timeStr[16];

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

	void TimeToStrWithFrame(int time)
	{
		char minStr[4];
		char secStr[4];
		char frameStr[4];

		int _min = time / (minute);
		int _sec = (time % (minute)) / (second);
		int _frame = time % (second) / (frame);

		_itoa(_min, minStr, 10);
		_itoa(_sec, secStr, 10);
		_itoa(_frame, frameStr, 10);
		strcpy(timeStr, minStr);
		strcat(timeStr, ":");
		strcat(timeStr, secStr);
		strcat(timeStr, ":");
		strcat(timeStr, frameStr);
	}

	int GetAdaptiveStep(int minTimeStep)
	{
		int step = minute;
		if (second * 30 > minTimeStep) step = second * 30;
		if (second * 15 > minTimeStep) step = second * 15;
		if (second * 10 > minTimeStep) step = second * 10;
		if (second * 5  > minTimeStep) step = second * 5;
		if (second > minTimeStep) step = second;
		if (frame > minTimeStep) step = frame;
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
		int end = min(timelineLen, screenEnd);
		int start = (int)ceil(ScreenToTime(left) / (double)step);
		int iter = (int)ceil(end / (double)step);

		api.setIA(topology::lineList);

		int counter = 0;

		for (int i = start; i < iter; i++)
		{
			int time = i * step;
			float x = TimeToScreen(time) - left + screenLeft;
			float h = ui::style::text::height / 4.f;
			if (time % (second) == 0) h *= 1.5;
			if (time % (minute) == 0) h *= 1.5;

			ui::Line::buffer[counter].x = x;
			ui::Line::buffer[counter].y = 1;
			ui::Line::buffer[counter].x1 = x;
			ui::Line::buffer[counter].y1 = 1 - h;

			counter++;
		}

		ui::Line::Draw(counter);
	}

	void DrawCursor()
	{
		float cursor = TimeToScreen(timer::timeCursor-pos);

		if (cursor<screenLeft || cursor > screenRight) return;

		ui::Line::buffer[0].x = cursor;
		ui::Line::buffer[0].y = 1;
		ui::Line::buffer[0].x1 = cursor;
		ui::Line::buffer[0].y1 = 1 - ui::style::text::height*2. - (float)(isKeyDown(TIME_KEY));

		api.blend(blendmode::alpha);
		ui::Line::Draw(1,1,1,1,.75f+.25f*sinf(timer::frameBeginTime*.01));

		ui::style::Base();
		api.setIA(topology::triList);
		TimeToStrWithFrame(timer::timeCursor);
		ui::text::Draw(timeStr, cursor, 1 - ui::style::text::height * 2);

		TimeToStrWithFrame(pos+ScreenToTime(.5));
		ui::text::Draw(timeStr, cursor, 1 - ui::style::text::height * 4);

	}

	void DrawTimeStamps()
	{
		float left = screenLeft + TimeToScreen(pos);
		float right = screenRight + TimeToScreen(pos);

		int screenEnd = ScreenToTime(right);
		int end = min(timelineLen, screenEnd);

		ui::style::Base();

		api.setIA(topology::triList);
		api.blend(blendmode::alpha, blendop::add);

		ps::letter_ps.samplers.s1Filter = filter::linear;
		ps::letter_ps.samplers.s1AddressU = addr::clamp;
		ps::letter_ps.samplers.s1AddressV = addr::clamp;
		ps::letter_ps.textures.tex = ui::fontTextureIndex;

		int minTimeStep = ScreenToTime(ui::style::text::height * 5.f);
		int step = GetAdaptiveStep(minTimeStep);
		int iter = (int)ceil(end / (double)step);
		int start = (int)(ceil(ScreenToTime(left) / (double)step));

		for (int i = start; i < iter; i++)
		{
			int time = i * step;
			float x = TimeToScreen(time) - left + screenLeft;

			if ((time % (second) == 0) || (time % (minute) == 0))
			{
				TimeToStr(time);
				ui::text::Draw(timeStr, x, 1.f - ui::style::text::height * 1.25f);
			}
		}
	}

	void lbDown()
	{
		if (!isKeyDown(TIME_KEY)) return;

		editor::ui::mousePos = editor::ui::GetCusorPos();
		timer::timeCursor = ScreenToTime(editor::ui::mousePos.x) + pos;
		timeCursorLast = timer::timeCursor;
		editor::ui::mouseLastPos = editor::ui::mousePos;
	}

	void rbDown()
	{
		if (!isKeyDown(TIME_KEY)) return;
		posLast = pos;
	}

	void reTime()
	{
		timer::startTime = timer::frameBeginTime - timer::timeCursor / (double)second * 1000.;
	}

	void Space()
	{
		play = !play;
		reTime();
	}

	void Wheel(int delta)
	{
		if (!isKeyDown(TIME_KEY)) return;
		auto c = TimeToScreen(pos-timer::timeCursor);

		if (delta < 0) zoomOut *= 1.2;
		if (delta > 0) zoomOut /= 1.2;

		zoomOut = min(zoomOut, timelineLen / (screenRight - screenLeft));
		zoomOut = max(zoomOut, 1000);

		pos = ScreenToTime(c)+timer::timeCursor;

	}

	float lerp(float x, float y, float a)
	{
		return x * (1 - a) + y * a;
	}

	void Draw()
	{
		editor::ui::mousePos = editor::ui::GetCusorPos();
		float delta = editor::ui::mousePos.x - editor::ui::mouseLastPos.x;

		float margin = .00f;
		auto rightM = ScreenToTime(screenRight - margin) + pos;
		auto leftM = ScreenToTime(screenLeft + margin) + pos;

		if (ui::lbDown && isKeyDown(TIME_KEY))
		{
			timer::timeCursor = timeCursorLast + delta * ScreenToTime(1.);

			if (timer::timeCursor > rightM) pos += timer::timeCursor - rightM;
			if (timer::timeCursor < leftM) pos -= leftM - timer::timeCursor;

			reTime();
		}

		if (ui::rbDown && isKeyDown(TIME_KEY))
		{
			pos = posLast - delta * ScreenToTime(1.);
		}

		pos = max(pos, -ScreenToTime(screenLeft));
		pos = min(pos, timelineLen - ScreenToTime(screenRight));
		timer::timeCursor = max(timer::timeCursor, 0);
		timer::timeCursor = min(timer::timeCursor, timelineLen);

		//if (isKeyDown(TIME_KEY))
		{
			DrawMakers();
			DrawCursor();
			DrawTimeStamps();
		}

		if (play)
		{
			timer::timeCursor=(timer::frameBeginTime - timer::startTime)*second/1000.;
			if (timer::timeCursor > rightM) pos += timer::timeCursor - rightM;

			pos = lerp(pos, timer::timeCursor - ScreenToTime(.5), .15);

		}
	}

}
