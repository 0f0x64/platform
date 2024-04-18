namespace TimeLine
{

	#define fps 60
	#define freq 44100
	#define frameLen freq/fps
	#define second freq
	#define minute freq*60

	int zoomOut = 1000;
	int pos = 0;

	float TimeToScreen(int time)
	{
		float x =  time / (float)zoomOut;
		return x;
	}

	int ScreenToTime(double x)
	{
		int time = x * zoomOut;
		return time;
	}

	char timeStr[64];
	
	void TimeToStr(int time)
	{
		char minStr[64];
		char secStr[64];
		char frameStr[64];

		int _min = time / (minute);
		int _sec = (time % (minute))/(second);
		int _frame = time % (second);

		_itoa(_min, minStr, 10);
		_itoa(_sec, secStr, 10);
		_itoa(_frame, frameStr, 10);
		strcpy(timeStr, minStr);
		strcat(timeStr, ":");
		strcat(timeStr, secStr);
	}

	
	void Show()
	{
		pos = timer::frameBeginTime * 1111.1;

		ui::ShowBox(0, 0, .5, 2);
		ui::ShowBox(0.25, 0, .5, 2);
		ui::ShowBox(0.5, 0, .5, 2);
		ui::ShowBox(0.75, 0, .5, 2);

		float screenLeft = .25;
		float screenRight = .75;

		float left = screenLeft +TimeToScreen(pos);
		float right = screenRight + TimeToScreen(pos);

		api.rt(tex::mainRT);
		api.blend(blendmode::off);

		api.setIA(topology::lineList);

		ps::colorFill.params.r = 1;
		ps::colorFill.params.g = 1;
		ps::colorFill.params.b = 1;
		ps::colorFill.params.a = 1;
		ps::colorFill.set();


		int step = minute;
		int end = minute * 10;
		zoomOut = end / 1;// (1 + pow(2, timer::frameBeginTime * .0001));

		float minTimeStep = ScreenToTime(16./width);
		if (second * 30 > minTimeStep) step = second * 30;
		if (second * 15 > minTimeStep) step = second * 15;
		if (second * 10 > minTimeStep) step = second * 10;
		if (second*5 > minTimeStep) step = second*5;
		if (second > minTimeStep) step = second;
		if (frameLen > minTimeStep) step = frameLen;

		int screenEnd = ScreenToTime(right);
		end = min(end, screenEnd);

		int start = ScreenToTime(left) / step;

		int iter = end / step;

		

		ui::pCounter = 0;
		
		for (int i = start; i <= iter; i++)
		{
			int time = i * step;
			float x = TimeToScreen(time) - left + screenLeft;
			float h = 1.f/150.f;
			if (time % (second) == 0) h *= 2;
			if (time % (minute) == 0) h *= 2;

			ui::SetFloat4Const(x, 1);
			ui::SetFloat4Const(x, 1-h);
			
			if (ui::pCounter > 127 || i == iter)
			{
				vs::lineDrawer.set();
				api.drawLine(ui::pCounter);
				ui::pCounter = 0;
			}
		}

		//--timestamps
		ui::style::Base();

		api.setIA(topology::triList);
		api.blend(blendmode::alpha, blendop::add);

		using namespace ui::style;

		vs::letter.params.width = text::width * aspect;
		vs::letter.params.height = text::height;
		vs::letter.set();

		ps::letter_ps.samplers.s1Filter = filter::linear;
		ps::letter_ps.samplers.s1AddressU = addr::clamp;
		ps::letter_ps.samplers.s1AddressV = addr::clamp;

		ps::letter_ps.textures.tex = tex::font;

		ps::letter_ps.params.r = text::r;
		ps::letter_ps.params.g = text::g;
		ps::letter_ps.params.b = text::b;
		ps::letter_ps.params.a = text::a;
		ps::letter_ps.set();

		step = minute;
		 minTimeStep = ScreenToTime(ui::style::text::height*5);
		if (second * 30 > minTimeStep) step = second * 30;
		if (second*15 > minTimeStep) step = second*15;
		if (second * 10 > minTimeStep) step = second * 10;
		if (second * 5 > minTimeStep) step = second * 5;
		if (second > minTimeStep) step = second;
		if (frameLen > minTimeStep) step = frameLen;

		iter = end / step;

		start = ceil(ScreenToTime(left) / (double)step);



		for (int i = start; i <= iter; i++)
		{
			int time = i * step;
			float x = TimeToScreen(time)-left+ screenLeft;
			if ((time % (second) == 0)|| (time % (minute) == 0))
			{
				TimeToStr(time);
				char* str = timeStr;

				ui::pCounter = 0;
				float offset = 0;
				for (unsigned int j = 0; j < strlen(str); j++)
				{
					ui::SetFloat4Const(x + offset, 1-ui::style::text::height*1.5, (float)(str[j] - 32), 0);

					offset += ui::getLetterOffset(str[j])/2.;
				}

				ConstBuf::Update(6, ConstBuf::float4array);
				ConstBuf::SetToVertex(6);


				api.draw(1, strlen(timeStr));
			}

		}
		

		

	}

}
