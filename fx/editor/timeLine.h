namespace TimeLine
{
	#define TIME_KEY VK_LMENU

	const int frame = SAMPLES_IN_FRAME;
	const int second = SAMPLING_FREQ;
	const int minute = SAMPLING_FREQ * FRAMES_PER_SECOND;
	const int timelineLen = second * (int) DEMO_DURATION;

	int bpm = 120;
	int bpmMaj = 4;
	int bpmMin = 4;

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

	int pos = -ScreenToTime(screenLeft);	
	
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
		float baseH = .01*1080./dx11::height;
		float h = baseH * .75f;
		if (time % (second) == 0) h = baseH * 1.25f;
		if (time % (second * 5) == 0) h = baseH * 1.5f;
		if (time % (second * 15) == 0) h = baseH * 2.f;
		if (time % (second * 30) == 0) h = baseH * 2.5f;
		if (time % (minute) == 0) h = baseH * 3.f;
		return h;
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
		step = GetAdaptiveStepBPM(ScreenToTime(.04*1920./width));
		calcIterVars();

		float baseH = .03 * 1080. / dx11::height;

		for (int i = 0; i < iter; i++)
		{
			int time = (int)( (i+start) * step);
			float x = TimeToScreen(time) - left + screenLeft;
			float x1 = TimeToScreen((int)((i + start + 1) * step)) - left + screenLeft;

			editor::ui::style::box::a = 1.f;
			editor::ui::style::box::r = .1f * ((i+start) % 2) + .15f;
			editor::ui::style::box::g = .1f * ((i+start) % 2) + .15f;
			editor::ui::style::box::b = .1f * ((i+start) % 2) + .15f;

			ui::Box::Draw(x, y, x1 - x, baseH);
		}
	}

	void DrawMakers(float y)
	{
		step = GetAdaptiveStepTime(ScreenToTime(.01*1920. / width));
		calcIterVars();

		for (int i = 0; i < iter; i++)
		{
			int time = (int)((i+start) * step);
			float x = TimeToScreen(time) - left + screenLeft;
			float h = getHeight(time);

			ui::Line::StoreLine(i, x, y, x, y - h);
		}

		ui::Line::Draw(iter);
	}

	float getScreenPos(int time)
	{
		return TimeToScreen(time) - left + screenLeft;
	}

	void DrawTimeStamps(float y)
	{
		ui::style::text::height = .03 * 1080. / dx11::height;
		ui::style::text::width = ui::style::text::height;

		step = GetAdaptiveStepTime(ScreenToTime(.1*1920./width));
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
		float baseH = .03 * 1080. / dx11::height;
		step = GetAdaptiveStepBPM(ScreenToTime(.04 * 1920. / width));
		calcIterVars();

		for (int i = 0; i < iter; i++)
		{
			int time = (int)((i + start) * step);
			float x = TimeToScreen(time) - left + screenLeft;
			BPMToStr(time);
			ui::Text::Draw(timeStr, x, y+baseH*.25);
		}
	}

	void DrawCursor(float y)
	{
		float h = 10*.01*1080. / dx11::height;
		float hc= 8 * .01 * 1080. / dx11::height;

		InputAssembler::IA(topology::lineList);
		Blend::Set(blendmode::alpha);

		float cursor = TimeToScreen(timer::timeCursor - pos);
		if (cursor<screenLeft || cursor > screenRight) return;
		ui::Line::StoreLine(0, cursor, y, cursor, y - hc);

		if (uiContext == uiContext_::timeline)
		{
			ui::Line::StoreLine(0, cursor, 1, cursor, 0);
		}

		ui::Line::Draw(1, 1, 1, 1, .75f + .25f * sinf((float)timer::frameBeginTime * .01f));

		InputAssembler::IA(topology::triList);
		TimeToStr(timer::timeCursor, true);
		ui::Text::Draw(timeStr, cursor, y - h);
	}

	int Quantize(int x, int q)
	{
		return ((int)(x / q)) * q;
	}

	void reTime()
	{
		timer::startTime = timer::frameBeginTime - timer::timeCursor / (double)second * 1000.;
	}

	void Space()
	{
		play = !play;
		posLast = pos;
		timeCursorLast = timer::timeCursor;
		reTime();

		Camera::viewCam.overRide = !play;

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

	bool clickRb = false;

	void playMode()
	{
		int rightM = ScreenToTime(screenRight - scrollMargin) + pos;
		int leftM = ScreenToTime(screenLeft + scrollMargin) + pos;

		timer::timeCursor = (int)((timer::frameBeginTime - timer::startTime) * second / 1000.f);
		if (timer::timeCursor > rightM)
		{
			pos += timer::timeCursor - rightM;
		}
		timer::timeCursor = timer::timeCursor % timelineLen;
		if (pos > 0) pos = 0;
	}

	void ProcessInput()
	{
		if (uiContext != uiContext_::timeline) return;

		if (ui::lbDown)
		{
			if (drag.isFree()) {

				editor::ui::mousePos = editor::ui::GetCusorPos();
				timer::timeCursor = ScreenToTime(editor::ui::mousePos.x) + pos;
				timer::timeCursor = clamp(timer::timeCursor, 0, timelineLen);

				timeCursorLast = timer::timeCursor;
				editor::ui::mouseLastPos = editor::ui::mousePos;

				drag.set(drag.context::timeCursor);

			}
		}

		if (ui::rbDown) 
		{
			if (!clickRb) {
				clickRb = true;
				posLast = pos;
			}
		}
		else
		{
			clickRb = false;
		}

		int rightM = ScreenToTime(screenRight - scrollMargin) + pos;
		int leftM = ScreenToTime(screenLeft + scrollMargin) + pos;

		if (ui::lbDown && drag.check(drag.context::timeCursor))
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
		

		if (!play)
		{
			pos = clamp(pos, -ScreenToTime(.5), timelineLen - ScreenToTime(.5));
			timer::timeCursor = clamp(timer::timeCursor, 0, timelineLen);
		}


	}

	void Draw()
	{
		left = screenLeft + TimeToScreen(pos);
		right = screenRight + TimeToScreen(pos);
		screenEnd = ScreenToTime(right);
		end = min(timelineLen, screenEnd);
		float baseH = .03 * 1080. / dx11::height;

		Rasterizer::Scissors(float4{ screenLeft*dx11::width, 0, screenRight*dx11::width, (float)dx11::height });

		ui::style::Base();
		editor::ui::style::box::outlineBrightness = 0;
		editor::ui::style::box::edge = 100;
		editor::ui::style::box::rounded = .001;
		ps::letter_ps.samplers.s1Filter = filter::linear;
		ps::letter_ps.samplers.s1AddressU = addr::clamp;
		ps::letter_ps.samplers.s1AddressV = addr::clamp;
		ps::letter_ps.textures.tex = (texture)ui::fontTextureIndex;

		Blend::Set(blendmode::alpha);
		InputAssembler::IA(topology::lineList);
		DrawMakers(1);

		Blend::Set(blendmode::alpha);
		InputAssembler::IA(topology::triList);

		DrawBPMGrid(0.0);
		DrawTimeStamps(1 - baseH*2.);
		DrawGridStamps(0);

		DrawCursor(1);

		Rasterizer::Scissors(float4{ 0, 0, (float)dx11::width, (float)dx11::height });

	}

}
