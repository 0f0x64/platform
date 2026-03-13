namespace tracker
{
	int curChannel;
	int curClip;

	cmd(Layer, layer ltype, pStr data[clipLen])
	{
		reflect;
		memcpy(track.channels[curChannel].clips[curClip].layer[(int)in.ltype], in.data, clipLen);
//		OutputDebugString((LPCSTR)in.data);
//		OutputDebugString("\n");
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

	cmd(Master, int16u masterBPM, int8u volume)
	{
		reflect;
		
		curChannel = -1;

		track.masterBPM = in.masterBPM;
		track.volume = in.volume;


		#if EditMode
			editor::TimeLine::bpm = track.masterBPM;
		#endif
		
	}
	
	void Music()
	{
		Master({ .masterBPM = 120,.volume = 100 });
		Channel({ .vol = 100,.pan = 0,.mute = switcher::off, .solo = switcher::off });
			Clip({.pos = 0,.len = 32,.repeat = 1,.bpmScale = 1,.overDub = switcher::off});
							//		 1.......9.......17......25...... 
							//		 123412341234
							//		 |...|...|...|...|...|...|...|...
				Layer({ layer::pitch,"ef" });
				Layer({layer::octave,"1" });


	}

}
