namespace tracker
{

	API(oscillator, int a, int b)
	{
		int d = 011;
	}

	API(eq, int a)
	{
		int b = 1;
	}

	byte curChannel;

	API(Pitch, int count, unsigned char ...)
	{

	#if !EditMode
		VA_READ
	#endif 

		auto& c = track_desc.channel[curChannel].clip[track_desc.channel[curChannel].clipCounter];

		for (int i = 0; i < count; i++)
		{
			c.note[(int)layers::pitch].note_pitch[i] = va_params[i];
		}

		c.note[(int)layers::pitch].cmdIndex = cmdCounter - 1;
	}

	API(Clip, timestamp pos, int len, int repeat, int bpmScale, overdub overDub, int swing)
	{
		//check range
		track_desc.channel[curChannel].clipCounter++;
		clip_& c = track_desc.channel[curChannel].clip[track_desc.channel[curChannel].clipCounter];

		c.position = pos;
		c.length = len;
		c.repeat = repeat;
		c.bpmScale = bpmScale;
		c.swing = swing;
		c.overDub = overDub;

		c.cmdIndex = cmdCounter - 1;
	}

	#if EditMode
		#pragma push_macro("Draw")
	#undef Draw

	void channelDraw(int i, float &x, float &y, float w, float lead, float sel)
	{
		w = editor::paramEdit::tabLen;
		editor::paramEdit::mouseOverItem = editor::paramEdit::showButton(x, y, w, editor::ui::style::text::height * 2.f - editor::ui::style::text::height * .2f , sel, i);

		/*
		editor::paramEdit::mouseOverItem = editor::isMouseOver(x, y, w, editor::ui::style::box::height*2);
		editor::ui::style::box::outlineBrightness = editor::paramEdit::mouseOverItem ? 1.f : 0.1f;

		editor::ui::Box::Setup();
		editor::paramEdit::setBStyle(sel);
		editor::ui::Box::Draw(x, y, w, editor::ui::style::text::height * .8f*2.);

		editor::ui::Text::Setup();
		editor::paramEdit::setTStyle(sel);
		editor::ui::Text::Draw(cmdParamDesc[i].funcName, x + editor::paramEdit::insideX, y + editor::paramEdit::insideY);*/

		y += lead*2;
	}

	#pragma pop_macro("Draw")
#endif



	API(kick, volume vol, panorama pan, volume send, switcher solo, switcher mute)
	{
	#if EditMode
			regfuncGroup(channel);
			regDrawer(channelDraw);

	#endif // Editor


		track_desc.channel[curChannel].clipCounter = -1;
		
		Clip(33, 11, 6, 1, overdub::off, 0);
		Pitch(11, 249, 253, 252, 253, 253, 255, 255, 255, 255, 255, 255);

		Clip(23, 4, 1, 1, overdub::off, 0);
		Pitch(4, 255, 204, 192, 0);

		oscillator(1, 2);

		curChannel++;
	}



	API(snare, volume vol, panorama pan, volume send, switcher solo, switcher mute)
	{
	#if EditMode
		regfuncGroup(channel);
		regDrawer(channelDraw);
	#endif	

		track_desc.channel[curChannel].clipCounter = -1;

		Clip(22, 10, 6, 1, overdub::off, 0);
		Pitch(8, 0, 0, 0, 255, 255, 255, 255, 255);

		Clip(38, 8, 1, 1, overdub::off, 0);
		Pitch(3, 0, 90, 255);

		oscillator(1, 2);

		curChannel++;
	}

	void playTrack_dc(int i, float& x, float& y, float w, float lead, float sel)
	{

	}

	API (Track, int masterBPM)
	{
		//regDrawer(playTrack_dc);
#if EditMode
		editor::TimeLine::bpm = track_desc.masterBPM = masterBPM;
#endif
		curChannel = 0;

		kick(0, -90, 14, switcher::off, switcher::off);
		snare(0, -90, 14, switcher::off, switcher::off);

		track_desc.channelsCount = curChannel;
	
	}

}
