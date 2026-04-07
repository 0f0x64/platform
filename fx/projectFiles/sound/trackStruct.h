const int maxClipLen = 128*4;
const int maxClipsCount = 32;
const int maxChannelsCount = 32;

struct track {

	int channelsCount;
	int16u  masterBPM;
	int8u  timeNumerator;
	int8u  timeDenominator;
	int8u  volume;

	struct
	{
		int clipsCount;
		int8u vol;
		int8s pan;
		switcher mute;
		switcher solo;
		
		struct {
			int16u pos;
			int16u len;
			int8u repeat;
			int8u bpmScaleNumerator;
			int8u bpmScaleDenominator;
			switcher overDub;
			char pattern[maxClipLen];
			int8u patternID;

		} clip[maxClipsCount];

	} channel[maxChannelsCount];

	struct {
		char note[maxClipLen];
		struct {
			char velocity[maxClipLen];
			char position[maxClipLen];
			char articulation[maxClipLen];
			char offset[maxClipLen];
			char slide[maxClipLen];
			char retrigger[maxClipLen];
		} params;
	} pattern[100];
	
};

