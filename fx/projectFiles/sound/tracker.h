namespace tracker
{
	int curChannel;

	
	cmd(oscillator, int a; int b;)
	{
		reflect;

		int d = 011;

		reflect_close;
	}


	cmd(Pitch, int count; int note[128];)
	{
		reflect;

		auto& c = track_desc.channel[curChannel].clip[track_desc.channel[curChannel].clipCounter];

		for (int i = 0; i < in.count; i++)
		{
			c.note[(int)layers::pitch].note_pitch[i] = in.note[i];
		}

		c.note[(int)layers::pitch].cmdIndex = cmdCounter - 1;

		reflect_close;
	}

	cmd(Clip, timestamp pos; int len; int repeat; int bpmScale; overdub overDub; int swing;)
	{
		reflect;

		track_desc.channel[curChannel].clipCounter++;
		clip_& c = track_desc.channel[curChannel].clip[track_desc.channel[curChannel].clipCounter];

		c.position = in.pos;
		c.length = in.len;
		c.repeat = in.repeat;
		c.bpmScale = in.bpmScale;
		c.swing = in.swing;
		c.overDub = in.overDub;

		c.cmdIndex = cmdCounter - 1;

		reflect_close;
	}


	cmd(snare, volume vol; panorama pan; volume send; switcher solo; switcher mute;)
	{
		reflect;

	#if EditMode
		regfuncGroup(channel);
		regDrawer(channelDraw);
	#endif	

		track_desc.channel[curChannel].clipCounter = -1;

		Clip({30,10,6,1,overdub::off,0});
		Pitch({8,27,27,26,25,27,27,29,30});

		Clip({51,4,1,1,overdub::off,0});
		Pitch({4,1,3,0,0});

		oscillator({ 1, 2 });

		curChannel++;

		reflect_close;
	}

	void playTrack_dc(int i, float& x, float& y, float w, float lead, float sel)
	{

	}
	

	cmd(Track, int masterBPM;)
	{
		reflect;

		//regDrawer(playTrack_dc);
#if EditMode
		editor::TimeLine::bpm = track_desc.masterBPM = in.masterBPM;
#endif
		curChannel = 0;

		snare({
			.vol = 46,
			.pan = -31,
			.send = 74,
			.solo = switcher::off,
			.mute = switcher::off
		});

		track_desc.channelsCount = curChannel;

		reflect_close;
	}
	

}
