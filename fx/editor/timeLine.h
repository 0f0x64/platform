namespace TimeLine
{

	const int TIME_KEY = VK_LMENU;

	const int frame = SAMPLES_IN_FRAME;
	const int second = SAMPLING_FREQ;
	const int minute = (SAMPLING_FREQ * FRAMES_PER_SECOND);
	const int timelineLen = minute * 10;

	float bpm = 60;
	float bpmDiv = 60 * 4;

	float screenLeft = .0f;
	float screenRight = 1.0f;
	
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

	int GetAdaptiveStep(int minTimeStep)
	{
		int step = minute;
		if (second * 30 > minTimeStep) step = second * 30;
		if (second * 15 > minTimeStep) step = second * 15;
		if (second * 5  > minTimeStep) step = second * 5;
		if (second > minTimeStep) step = second;
		if (frame > minTimeStep) step = frame;
		return step;
	}

	void DrawMakers(float y)
	{
		api.blend(blendmode::off);

		float left = screenLeft + TimeToScreen(pos);
		
		float right = screenRight + TimeToScreen(pos);
		int minTimeStep = ScreenToTime(0.003125);
		int step = GetAdaptiveStep(minTimeStep);

		int screenEnd = ScreenToTime(right);
		int end = min(timelineLen, screenEnd);
		int start = (int)ceil(ScreenToTime(left) / (double)step);
		start = max(start, 0);
		int iter = (int)ceil(end / (double)step);

		api.setIA(topology::lineList);

		int counter = 0;

		for (int i = start; i < iter; i++)
		{
			float baseH = ui::style::text::height / 4.f;
			int time = i * step;
			float x = TimeToScreen(time) - left + screenLeft;
			float h = baseH*1.5;
			if (time % (second) == 0) h = baseH*1.5;
			if (time % (second*5) == 0) h = baseH * 1.5;
			if (time % (second * 15) == 0) h = baseH * 2.;
			if (time % (second * 30) == 0) h = baseH * 2.5;
			if (time % (minute) == 0) h = baseH * 2.5;

			ui::Line::buffer[counter].x = x;
			ui::Line::buffer[counter].y = y;
			ui::Line::buffer[counter].x1 = x;
			ui::Line::buffer[counter].y1 = y - h;

			counter++;
		}

		ui::Line::Draw(counter);
	}

	void DrawCursor(float y)
	{
		float cursor = TimeToScreen(timer::timeCursor-pos);

		if (cursor<screenLeft || cursor > screenRight) return;

		ui::Line::buffer[0].x = cursor;
		ui::Line::buffer[0].y = y;
		ui::Line::buffer[0].x1 = cursor;
		ui::Line::buffer[0].y1 = y - ui::style::text::height*1.25;

		if (isKeyDown(TIME_KEY))
		{
			ui::Line::buffer[0].y = 1;
			ui::Line::buffer[0].y1 = 0;
		}

		api.blend(blendmode::alpha);
		ui::Line::Draw(1,1,1,1,.75f+.25f*sinf(timer::frameBeginTime*.01));

		ui::style::Base();
		api.setIA(topology::triList);
		TimeToStr(timer::timeCursor, true);
		ui::text::Draw(timeStr, cursor, y - ui::style::text::height * 2);
	}

	void DrawTimeStamps(float y)
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

		int minTimeStep = ScreenToTime(ui::style::text::height * 2.f);
		int step = GetAdaptiveStep(minTimeStep);
		int iter = (int)ceil(end / (double)step);
		int start = (int)(ceil(ScreenToTime(left) / (double)step));
		start = max(start, 0);

		for (int i = start; i < iter; i++)
		{
			int time = i * step;
			float x = TimeToScreen(time) - left + screenLeft;

			if ((time % (second) == 0) || (time % (minute) == 0))
			{
				TimeToStr(time);
				ui::text::Draw(timeStr, x, y);
			}
		}
	}

	void lbDown()
	{
		if (!isKeyDown(TIME_KEY)) return;

		editor::ui::mousePos = editor::ui::GetCusorPos();
		timer::timeCursor = ScreenToTime(editor::ui::mousePos.x) + pos;

		timer::timeCursor = max(timer::timeCursor, 0);
		timer::timeCursor = min(timer::timeCursor, timelineLen);

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
		posLast = pos;
		timeCursorLast = timer::timeCursor;
		reTime();
	}

	void Wheel(int delta)
	{
		if (!isKeyDown(TIME_KEY)) return;

		auto c = TimeToScreen(pos-timer::timeCursor);

		zoomOut *= delta<0 ? 1.05 : 1/1.05;

		zoomOut = min(zoomOut, timelineLen / (screenRight - screenLeft));
		zoomOut = max(zoomOut, 1000);

		pos = ScreenToTime(c)+timer::timeCursor;
		posLast = pos;
		timeCursorLast = timer::timeCursor;
		editor::ui::mouseLastPos.x = editor::ui::mousePos.x;

		
	}

	void Draw()
	{
		editor::ui::lbDown = isKeyDown(VK_LBUTTON) ? true : false;
		editor::ui::rbDown = isKeyDown(VK_RBUTTON) ? true : false;

		editor::ui::mousePos = editor::ui::GetCusorPos();
		
		float delta = editor::ui::mousePos.x - editor::ui::mouseLastPos.x;

		float margin = .05f;
		auto rightM = ScreenToTime(screenRight - margin) + pos;
		auto leftM = ScreenToTime(screenLeft + margin) + pos;

		if (isKeyDown(TIME_KEY))
		{
			if (ui::lbDown)
			{
				timer::timeCursor = timeCursorLast + delta * ScreenToTime(1.);
				if (timer::timeCursor > rightM && pos < timelineLen - ScreenToTime(screenRight-margin) )
				{
					pos += timer::timeCursor - rightM; editor::ui::mouseLastPos.x -= .5* (editor::ui::mousePos.x-(screenRight-margin));
				}
				if (timer::timeCursor < leftM && pos>-ScreenToTime(screenLeft+margin) )
				{
					pos += timer::timeCursor - leftM; editor::ui::mouseLastPos.x += .5 * ((screenLeft + margin)- editor::ui::mousePos.x );
				}
				reTime();
			}

			if (ui::rbDown)
			{
				pos = posLast - delta * ScreenToTime(1.);
			}
		}


		if (play)
		{
			timer::timeCursor = (timer::frameBeginTime - timer::startTime) * second / 1000.;
			if (timer::timeCursor > rightM) {
				pos += timer::timeCursor - rightM;
			}

			//pos = lerp(pos, timer::timeCursor - ScreenToTime(.5), .15);

		}

		pos = max(pos, -ScreenToTime(.5));
		pos = min(pos, timelineLen - ScreenToTime(.5));
		timer::timeCursor = max(timer::timeCursor, 0);
		timer::timeCursor = min(timer::timeCursor, timelineLen);

		DrawMakers(1);
		DrawCursor(1);
		DrawTimeStamps(1 - ui::style::text::height * 1.25f);

	}

}
