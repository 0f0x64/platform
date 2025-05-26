namespace tracker
{
	int curChannel;

	
	cmd(oscillator, int a; int b;)
	{
		reflect;

		int d = 011;

		reflect_close;
	}


	cmd(Pitch, int count; int8u note[128];)
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

		Clip({25,10,6,1,overdub::off,0});
		Pitch({8,27,25,0,0,0,0,0,0});

		Clip({51,4,1,1,overdub::off,0});
		Pitch({4,1,3,0,0});

		oscillator({ 1, 2 });

		curChannel++;

		reflect_close;
	}

	void playTrack_dc(int i, float& x, float& y, float w, float lead, float sel)
	{

	}
	
#define sDecl(name) struct name
#define pDecl(type, name) type name

#define clipParam \
	pDecl(int16u, pos);\
	pDecl(int16u, len);\
	pDecl(int8u, repeat);\
	pDecl(int8u, bpmScale);\
	pDecl(int8u, overDub);\
	pDecl(int8u, swing);\
	pDecl(int8u, pitch[128]);\
	pDecl(int8u, vol[128]);\
	pDecl(int8u, variation[128]);\
	pDecl(int8u, slide[128]);\
	pDecl(int8u, retrigger[128]);\
	pDecl(int8u, send[128]);

	struct clip {
		clipParam
	};

#define channelParam \
	pDecl(int8u,vol);\
	pDecl(int8s,pan);\
	pDecl(switcher,mute);\
	pDecl(switcher,solo);\
	pDecl(clip,clips[32]);

	struct channel
	{
		channelParam
	};

#define trackParam \
	pDecl(int,masterBPM);\
	pDecl(int,volume);\
	pDecl(channel,channels[32]);

	struct track {
		trackParam
	};

#undef cDelc
#define cDecl(type,name) str+=  std::to_string(c.name);


	/*#pragma pack (push,1)
	struct clip{
		int16u pos;
		int16u len;
		int8u repeat;
		int8u bpmScale;
		int8u overDub;
		int8u swing;

		int8u pitch[128];
		int8u vol[128];
		int8u variation[128];
		int8u slide[128];
		int8u retrigger[128];
		int8u send[128];
	};

	struct channel
	{
		int8u vol;
		int8s pan;
		switcher mute;
		switcher solo;

		clip clips[32];

	};

	struct _ts {
		int masterBPM;
		int volume;
		channel channels[32];

	};
	#pragma pack (pop)

	_ts track;*/

	void ttt()
	{
		track track1 = {
			.masterBPM = 120,
			.volume = 10,
			.channels = {
				channel {
					.vol = 0,
					.pan = 0,
					.mute = switcher::off,
					.solo = switcher::off,

					.clips = {
						clip {
							.pos = 0,
							.len = 10,
							.repeat = 1,
							.bpmScale = 1,
							.overDub = 0,
							.swing = 0,
							.pitch = {1,1,2,1},
							.vol = {1,1,0},
							.variation = {},
							.slide = {0},
							.retrigger = {0},
							.send = {1,2,1,1}
						},
						clip {
							.pos = 0,
							.len = 10,
							.repeat = 1,
							.bpmScale = 1,
							.overDub = 0,
							.swing = 0,
							.pitch = {111,1,112,1},
							.vol = {1,1,0},
							.variation = {1,1,1},
							.slide = {0},
							.retrigger = {0},
							.send = {1,2,1,1}
						},

					}
				},
				channel {
					.vol = 0,
					.pan = 0,
					.mute = switcher::off,
					.solo = switcher::off,

					.clips = {
						clip {
							.pos = 0,
							.len = 10,
							.repeat = 1,
							.bpmScale = 1,
							.overDub = 0,
							.swing = 0,
							.pitch = {1,1,2,1},
							.vol = {1,1,0},
							.variation = {1,1,1},
							.slide = {0},
							.retrigger = {0},
							.send = {1,2,1,1}
						},
						clip {
							.pos = 0,
							.len = 10,
							.repeat = 1,
							.bpmScale = 1,
							.overDub = 0,
							.swing = 0,
							.pitch = {111,1,112,1},
							.vol = {1,1,0},
							.variation = {1,1,1},
							.slide = {0},
							.retrigger = {0},
							.send = {1,2,1,1}
						},

					}
				}
			}
			
		};

		int a = 0;
	}

	cmd(Track, int masterBPM;)
	{
		reflect;

		ttt();
		//regDrawer(playTrack_dc);
#if EditMode
		editor::TimeLine::bpm = track_desc.masterBPM = in.masterBPM;
#endif
		curChannel = 0;

		snare({
			.vol = 73,
			.pan = 90,
			.send = 74,
			.solo = switcher::off,
			.mute = switcher::off
		});

		track_desc.channelsCount = curChannel;

		reflect_close;
	}
	

}
