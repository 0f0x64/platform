namespace tracker
{

#define NOTES_IN_CLIP 128
#define MAX_CLIPS 128
#define CHANNELS 16
#define ENV_POINTS 32

	int clipCounter;

	typedef struct
	{
		int position;

		struct {
			int x;
			int y;
		} points[ENV_POINTS];

		byte pCount;

		byte variation;
		byte mixOp;
		byte assignTo;
		byte assignOp;
		byte link;
		byte loop;
		byte loopStart;
		byte loopEnd;

	} envelope_;


	typedef struct
	{
		byte note_pitch[NOTES_IN_CLIP];
		int cmdIndex;
	} note_;


	typedef struct
	{
		note_ note[8];

		int position;
		int length;
		int repeat;
		int bpmScale;
		int swing;
		overdub overDub;//overwrite or mix

		int cmdIndex;

	} clip_;


	struct {

		struct
		{
			byte volume;
			byte pan;
			byte send[4];

			envelope_ volumeEnv;
			envelope_ panEnv;
			envelope_ sendEnv[4];

			clip_ clip[MAX_CLIPS];
			int clipCounter;
			int cmdIndex;

		} channel[CHANNELS];

		int channelsCount;

		int masterBPM;

	} track_desc;
}