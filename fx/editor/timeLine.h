namespace TimeLine
{
	const int TIME_KEY = VK_LMENU;

	const int frame = SAMPLES_IN_FRAME;
	const int second = SAMPLING_FREQ;
	const int minute = SAMPLING_FREQ * FRAMES_PER_SECOND;
	const int timelineLen = minute * 5;

	float bpm = 120;
	float bpmMaj = 4;
	float bpmMin = 4;

	float screenLeft = .0f;
	float screenRight = 1.f;
	float scrollMargin = .05f;
	
	int zoomOut = (int)(timelineLen / (screenRight- screenLeft));
	int timeCursorLast = 0;
	int posLast = 0;

	bool play = false;

	float TimeToScreen(int time)
	{
		return time / (float)zoomOut;
	}

	int ScreenToTime(double x)
	{
		return (int)(x * zoomOut);
	}

	int pos = (-ScreenToTime(screenLeft));	
	
	double GetAdaptiveStepTime(int minTimeStep)
	{
		double step = minute;
		if (second * 30 > minTimeStep) step = second * 30;
		if (second * 15 > minTimeStep) step = second * 15;
		if (second * 5 > minTimeStep) step = second * 5;
		if (second > minTimeStep) step = second;
		if (frame > minTimeStep) step = frame;

		return step;
	}

	double GetAdaptiveStepBPM(int minTimeStep)
	{
		double grid = 4 * minute / (bpm * bpmMin);

		double step = grid * bpmMaj * 50;
		if (grid * bpmMaj * 10> minTimeStep) step = grid * bpmMaj * 10;
		if (grid * bpmMaj * 5 > minTimeStep) step = grid * bpmMaj * 5;
		if (grid * bpmMaj  > minTimeStep) step = grid * bpmMaj ;
		if (grid > minTimeStep) step = grid;

		return step;
	}

	char timeStr[16];

	void TimeToStr(int time, bool frames =  false)
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
		if (_sec < 10) strcat(timeStr, "0");
		strcat(timeStr, secStr);

		if (!frames) return;

		strcat(timeStr, ":");
		if (_frame < 10) strcat(timeStr, "0");
		strcat(timeStr, frameStr);
	}

	void BPMToStr(int time)
	{
		int grid = (int)(4*minute / (bpm*bpmMin));
		int _maj = (int)(time / (grid*bpmMaj));
		int _min = (time % (int)(grid * bpmMaj)) / (grid);

		char majStr[8];
		_itoa(_maj+1, majStr, 10);
		strcpy(timeStr, majStr);

		if (_min > 0)
		{
			char minStr[8];
			_itoa(_min + 1, minStr, 10);
			strcat(timeStr, ".");
			strcat(timeStr, minStr);
		}
	}

	float getHeight(int time)
	{
		float baseH = ui::style::text::height / 4.f;
		float h = baseH * .75f;
		if (time % (second) == 0) h = baseH * 1.25f;
		if (time % (second * 5) == 0) h = baseH * 1.5f;
		if (time % (second * 15) == 0) h = baseH * 2.f;
		if (time % (second * 30) == 0) h = baseH * 2.5f;
		if (time % (minute) == 0) h = baseH * 3.f;
		return h;
	}

	void StoreLine(int counter, float x, float y, float x1, float y1)
	{
		ui::Line::buffer[counter*2].x = x;
		ui::Line::buffer[counter*2].y = y;
		ui::Line::buffer[counter*2 + 1].x = x1;
		ui::Line::buffer[counter*2 + 1].y = y1;
	}

	float left;
	float right;
	int screenEnd;
	int end;
	double step;
	int start;
	int iter;

	void calcIterVars()
	{
		start = (int)(floor(ScreenToTime(left) / step));
		start = max(start, 0);
		iter = (int)ceil(end / step) - start;
	}

	void DrawBPMGrid(float y)
	{
		step = GetAdaptiveStepBPM(ScreenToTime(ui::style::text::height * 1.f));
		calcIterVars();

		for (int i = 0; i < iter; i++)
		{
			int time = (int)( (i+start) * step);
			float x = TimeToScreen(time) - left + screenLeft;
			float x1 = TimeToScreen((int)((i + start + 1) * step)) - left + screenLeft;

			editor::ui::style::box::a = 1.f;
			editor::ui::style::box::r = .1f * ((i+start) % 2) + .15f;
			editor::ui::style::box::g = .1f * ((i+start) % 2) + .15f;
			editor::ui::style::box::b = .1f * ((i+start) % 2) + .15f;

			ui::Box::Draw(x, y - editor::ui::style::text::height*.75f , x1 - x, editor::ui::style::text::height/1.5f );
		}
	}

	void DrawMakers(float y)
	{
		step = GetAdaptiveStepTime(ScreenToTime(8.f / width));
		calcIterVars();

		for (int i = 0; i < iter; i++)
		{
			int time = (int)((i+start) * step);
			float x = TimeToScreen(time) - left + screenLeft;
			float h = getHeight(time);

			StoreLine(i, x, y, x, y - h);
		}

		ui::Line::Draw(iter);
	}

	void DrawTimeStamps(float y)
	{
		step = GetAdaptiveStepTime(ScreenToTime(ui::style::text::height * 1.f));
		calcIterVars();

		for (int i = 0; i < iter; i++)
		{
			int time = (int)((i + start) * step);
			float x = TimeToScreen(time) - left + screenLeft;

			if ((time % (second) == 0) || (time % (minute) == 0))
			{
				TimeToStr(time);
				ui::Text::Draw(timeStr, x, y);
			}
		}
	}

	void DrawGridStamps(float y)
	{
		step = GetAdaptiveStepBPM(ScreenToTime(ui::style::text::height * 1.f));
		calcIterVars();

		for (int i = 0; i < iter; i++)
		{
			int time = (int)((i + start) * step);
			float x = TimeToScreen(time) - left + screenLeft;
			BPMToStr(time);
			ui::Text::Draw(timeStr, x, y);
		}
	}

	void DrawCursor(float y)
	{
		gapi.blend(blendmode::alpha);
		gapi.setIA(topology::lineList);

		float cursor = TimeToScreen(timer::timeCursor - pos);
		if (cursor<screenLeft || cursor > screenRight) return;
		StoreLine(0, cursor, y, cursor, y - ui::style::text::height * 1.25f);
		if (isKeyDown(TIME_KEY)) StoreLine(0, cursor, 1, cursor, 0);
		ui::Line::Draw(1, 1, 1, 1, .75f + .25f * sinf((float)timer::frameBeginTime * .01f));

		gapi.setIA(topology::triList);
		TimeToStr(timer::timeCursor, true);
		ui::Text::Draw(timeStr, cursor, y - ui::style::text::height * 2.f);
	}

	int Quantize(int x, int q)
	{
		return ((int)(x / q)) * q;
	}

	void reTime()
	{
		timer::startTime = timer::frameBeginTime - timer::timeCursor / (double)second * 1000.;
	}

	void lbDown()
	{
		if (!isKeyDown(TIME_KEY)) return;

		editor::ui::mousePos = editor::ui::GetCusorPos();
		timer::timeCursor = ScreenToTime(editor::ui::mousePos.x) + pos;
		timer::timeCursor = clamp(timer::timeCursor, 0, timelineLen);

		timeCursorLast = timer::timeCursor;
		editor::ui::mouseLastPos = editor::ui::mousePos;
	}

	void rbDown()
	{
		if (!isKeyDown(TIME_KEY)) return;
		posLast = pos;
	}

	void Space()
	{
		play = !play;
		posLast = pos;
		timeCursorLast = timer::timeCursor;
		reTime();
	}

	void Wheel(int delta)
	{
		if (!isKeyDown(TIME_KEY)) return;

		float c = TimeToScreen(pos-timer::timeCursor);
		zoomOut = (int)(zoomOut * (delta < 0 ? 1.05 : 1./1.05));
		zoomOut = (int)clamp((float)zoomOut, 1000.f, timelineLen / (screenRight - screenLeft));
		pos = ScreenToTime(c)+timer::timeCursor;
		posLast = pos;
		timeCursorLast = timer::timeCursor;
		editor::ui::mouseLastPos.x = editor::ui::mousePos.x;
	}

	void Left()
	{
		timer::timeCursor -= frame;
		timer::timeCursor = Quantize(timer::timeCursor, frame);
		timer::timeCursor = clamp(timer::timeCursor, 0, timelineLen);
		if (TimeToScreen(timer::timeCursor - pos) < screenLeft + scrollMargin)	pos -= frame;
		pos = clamp(pos, -ScreenToTime(.5), timelineLen - ScreenToTime(.5));
	}

	void Right()
	{
		timer::timeCursor += frame;
		timer::timeCursor = Quantize(timer::timeCursor, frame);
		timer::timeCursor = clamp(timer::timeCursor, 0, timelineLen);
		if (TimeToScreen(timer::timeCursor - pos) > screenRight - scrollMargin)	pos += frame;
		pos = clamp(pos, -ScreenToTime(.5), timelineLen - ScreenToTime(.5));
	}

	void ProcessInput()
	{

		int rightM = ScreenToTime(screenRight - scrollMargin) + pos;
		int leftM = ScreenToTime(screenLeft + scrollMargin) + pos;

		if (isKeyDown(TIME_KEY))
		{
			if (ui::lbDown)
			{
				float scrollSpeed = .5;
				timer::timeCursor = timeCursorLast + (int)(ui::mouseDelta.x * ScreenToTime(1.f));

				if (timer::timeCursor > rightM && pos < timelineLen - ScreenToTime(screenRight - scrollMargin))
				{
					pos += timer::timeCursor - rightM;
					editor::ui::mouseLastPos.x -= scrollSpeed * (editor::ui::mousePos.x - (screenRight - scrollMargin));
				}

				if (timer::timeCursor < leftM && pos > -ScreenToTime(screenLeft + scrollMargin))
				{
					pos += timer::timeCursor - leftM;
					editor::ui::mouseLastPos.x += scrollSpeed * ((screenLeft + scrollMargin) - editor::ui::mousePos.x);
				}

				reTime();
			}

			if (ui::rbDown)
			{
				pos = posLast - (int)(ui::mouseDelta.x * ScreenToTime(1.f));
			}
		}

		if (play)
		{
			timer::timeCursor = (int)((timer::frameBeginTime - timer::startTime) * second / 1000.f);

			if (timer::timeCursor > rightM)
			{
				pos += timer::timeCursor - rightM;
			}
		}
	}

	void Draw()
	{
		ProcessInput();

		gapi.setScissors(screenLeft, 0, screenRight, 1);

		pos = clamp(pos, -ScreenToTime(.5), timelineLen - ScreenToTime(.5));
		timer::timeCursor = clamp(timer::timeCursor, 0, timelineLen);

		left = screenLeft + TimeToScreen(pos);
		right = screenRight + TimeToScreen(pos);
		screenEnd = ScreenToTime(right);
		end = min(timelineLen, screenEnd);

		ui::style::Base();
		editor::ui::style::box::outlineBrightness = 0;
		editor::ui::style::box::edge = 100;
		editor::ui::style::box::rounded = .001;
		ps::letter_ps.samplers.s1Filter = filter::linear;
		ps::letter_ps.samplers.s1AddressU = addr::clamp;
		ps::letter_ps.samplers.s1AddressV = addr::clamp;
		ps::letter_ps.textures.tex = ui::fontTextureIndex;

		gapi.blend(blendmode::off);
		gapi.setIA(topology::lineList);
		DrawMakers(1);

		gapi.blend(blendmode::alpha);
		gapi.setIA(topology::triList);
		DrawBPMGrid(0.025);
		DrawTimeStamps(1 - ui::style::text::height * 1.25f);
		DrawGridStamps(.025f - ui::style::text::height * .75f);

		DrawCursor(1);

		gapi.setScissors(0, 0, 1, 1);

	}

}
