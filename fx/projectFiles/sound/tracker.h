namespace tracker
{
	int curChannel;
	int curClip;

	cmd(Layer, layerType ltype, char data[clipLen])
	{
		reflect;
		strcpy(track.channels[curChannel].clips[curClip].layer[(int)in.ltype], in.data);
	}

	cmd(Pitch,char data[clipLen])
	{
		reflect;
		strcpy(track.channels[curChannel].clips[curClip].layer[(int)layerType::pitch], in.data);
	}

	cmd(Octave, char data[clipLen])
	{
		reflect;
		strcpy(track.channels[curChannel].clips[curClip].layer[(int)layerType::octave], in.data);
	}

	cmd(Clip, 
		int16u pos,
		int16u len,
		int8u repeat,
		int8u bpmScale,
		switcher overDub)
	{
		reflect;

		curClip++;

		track.channels[curChannel].clips[curClip] = {
			.pos = in.pos,
			.len = in.len,
			.repeat = in.repeat,
			.bpmScale = in.bpmScale,
			.overDub = in.overDub
		};
	}

	cmd(Channel,
		int8u vol,
		int8s pan,
		switcher mute,
		switcher solo)
	{
		reflect;
		
		curClip = -1;
		curChannel++;

		track.channels[curChannel] = {
			.vol = in.vol,
			.pan = in.pan,
			.mute = in.mute,
			.solo = in.solo
		};

	}

	cmd(Track, int16u masterBPM, int8u volume)
	{
		reflect;
		
		curChannel = -1;

		track = {
			.masterBPM = in.masterBPM,
			.volume = in.volume
		};

		#if EditMode
			editor::TimeLine::bpm = track.masterBPM;
		#endif
		
	}
	
	void tr()
	{
		Track({ .masterBPM = 120,.volume = 100 });
		Channel({ .vol = 100,.pan = 0,.mute = switcher::off, .solo = switcher::off });
			Clip({.pos = 0,.len = 32,.repeat = 1,.bpmScale = 1,.overDub = switcher::off});
				//		|0.......8.......16......24......|
				//		|01234567012345670123456701234567|
				Pitch({ "DDDCDEFGDDDCDEFGDDDCDEFGDDDCDEFG" });
				Octave({"12121212121212121212121212121212" });

	}

}
