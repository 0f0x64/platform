namespace tracker
{
	int curChannel;
	int curClip;
	int curLayer;

	/*
	cmd(Layer, layer ltype, pStr data[maxClipLen])
	{
		reflect;

		curLayer++;

		memcpy(track.channels[curChannel].clips[curClip].layer[(int)in.ltype], in.data, maxClipLen);
//		OutputDebugString((LPCSTR)in.data);
//		OutputDebugString("\n");

		track.channels[curChannel].clips[curClip].layersCount = curLayer + 1;

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
		curLayer = -1;

		track.channels[curChannel].clips[curClip] = {
			.pos = in.pos,
			.len = in.len,
			.repeat = in.repeat,
			.bpmScale = in.bpmScale,
			.overDub = in.overDub
		};

		track.channels[curChannel].clipsCount = curClip + 1;

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

		track.channelsCount = curChannel + 1;

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
	*/
		
	void Music()
	{
		/*Master({.masterBPM = 120,.volume = 100});
		Channel({ .vol = 100,.pan = 0,.mute = switcher::off, .solo = switcher::off });
		Clip({ .pos = 0,.len = 16,.repeat = 1,.bpmScale = 1,.overDub = switcher::off });
		//		 1.......9.......17......25...... 
		//		 123412341234
		//		 |...|...|...|...|...|...|...|...
		Layer({ layer::pitch,"cdefgcdefgcdefg" });
		Layer({ layer::octave,"1" });*/
			
		//#include "trackData.h"

		char s[] = "pitch:cdedce/"
				   "var  :112123/";

char d[] = 
"d#2 v1."
"c."
"c.";

		int a = 0;
#if EditMode
		
		editor::TimeLine::bpm = Track.masterBPM;
		editor::TimeLine::bpmMaj = Track.timeNumerator;
		editor::TimeLine::bpmMin = Track.timeDenominator;
#endif // EditMode

	}

}
