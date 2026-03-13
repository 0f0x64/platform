const int clipLen = 128;
const int clipsCount = 32;
const int channelsCount = 32;


struct clip {
	int16u pos;
	int16u len;
	int8u repeat;
	int8u bpmScale;
	switcher overDub;
	char layer[layersCount][clipLen];
};

struct channel
{
	int8u vol;
	int8s pan;
	switcher mute;
	switcher solo;
	clip clips[clipsCount];

};

struct _track {
	int16u  masterBPM;
	int8u  volume;
	channel channels[channelsCount];
};

