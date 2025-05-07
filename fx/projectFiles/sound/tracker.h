namespace tracker
{
	/*
	cmd(oscillator, int a, int b)
	{
		int d = 011;
	}

	cmd(eq, int a)
	{
		int b = 1;
	}

	byte curChannel;

	cmd(Pitch, int count, unsigned char ...)
	{

	#if !EditMode
		VA_READ
	#endif 

		auto& c = track_desc.channel[curChannel].clip[track_desc.channel[curChannel].clipCounter];

		for (int i = 0; i < count; i++)
		{
			c.note[(int)layers::pitch].note_pitch[i] = va_params[i+1];
		}

		c.note[(int)layers::pitch].cmdIndex = cmdCounter - 1;
	}

	cmd(Clip, timestamp pos, int len, int repeat, int bpmScale, overdub overDub, int swing)
	{
		
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

		y += lead*2;
	}

	#pragma pop_macro("Draw")
#endif



	cmd(kick, volume vol, panorama pan, volume send, switcher solo, switcher mute)
	{
	#if EditMode
			regfuncGroup(channel);
			regDrawer(channelDraw);

	#endif 


		track_desc.channel[curChannel].clipCounter = -1;
		
		Clip(0, 11, 6, 1, overdub::off, 0);
		Pitch(11, 29, 29, 29, 29, 0, 27, 25, 27, 25, 27, 0);

		Clip(3, 4, 1, 1, overdub::off, 0);
		Pitch(4, 255, 204, 192, 0);

		oscillator(1, 2);

		curChannel++;
	}



	cmd(snare, volume vol, panorama pan, volume send, switcher solo, switcher mute)
	{
	#if EditMode
		regfuncGroup(channel);
		regDrawer(channelDraw);
	#endif	

		track_desc.channel[curChannel].clipCounter = -1;

		Clip(21, 10, 6, 1, overdub::off, 0);
		Pitch(8, 27, 29, 29, 29, 29, 29, 29, 29);

		Clip(33, 4, 1, 1, overdub::off, 0);
		Pitch(3, 27, 39, 27);

		oscillator(1, 2);

		curChannel++;
	}

	void playTrack_dc(int i, float& x, float& y, float w, float lead, float sel)
	{

	}

	cmd (Track, int masterBPM)
	{
		//regDrawer(playTrack_dc);
#if EditMode
		editor::TimeLine::bpm = track_desc.masterBPM = masterBPM;
#endif
		curChannel = 0;

		kick(100, 0, 14, switcher::on, switcher::on);
		snare(0, -25, 14, switcher::off, switcher::off);

		track_desc.channelsCount = curChannel;
	
	}
	*/

}
