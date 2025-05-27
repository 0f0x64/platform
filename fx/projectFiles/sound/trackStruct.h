#pragma pack (push,1)
struct clip {
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
	int8u  clipsCount;
	clip clips[32];

};

struct _track {
	int16u  masterBPM;
	int8u  volume;
	int8u  channelsCount;
	channel channels[32];

};
#pragma pack (pop)