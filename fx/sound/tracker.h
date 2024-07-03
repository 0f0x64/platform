namespace tracker
{

#define NOTES_IN_CLIP 128
#define CLIPS_IN_CHANNEL 32
#define CHANNELS 16
#define ENV_POINTS 32

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
		byte note[NOTES_IN_CLIP];
		byte volume[NOTES_IN_CLIP];
		byte variation[NOTES_IN_CLIP];
		byte retrigger[NOTES_IN_CLIP];
		byte slide[NOTES_IN_CLIP];
		byte send[4][NOTES_IN_CLIP];
	} notes_;

	typedef struct
	{
		notes_ notes;

		int position;
		int length;
		int repeat;
		envelope_ volume;
		int bpmScale;
		float swing;
		bool mode;//overwrite or mix
		int grid;
	} clip_;


	struct {

		struct
		{
			clip_ clip[CLIPS_IN_CHANNEL];

			byte volume;
			byte pan;

			envelope_ volumeEnv;
			envelope_ panEnv;
			envelope_ sendEnv[4];


		} channel[CHANNELS];

		void master()
		{

		}


		float masterBPM;

	} track_desc;

	COMMAND (oscillator, int a, int b)
	{
		#include REFLINK(oscillator)
		int d = 011;
	}

	COMMAND (eq, int a)
	{
		#include REFLINK(eq)
		int b = 1;
	}

	COMMAND (channel_01_bass, int volume, int pan, visibility ms)
	{
		#include REFLINK(channel_01_bass)
		int c = 2;
		oscillator(1, 2);
	}

	COMMAND (channel_02_solo, int volume,int pan,visibility ms)
	{
		#include REFLINK(channel_02_solo)

		int a = 4;
	}



	COMMAND (mix, int level)
	{
		#include REFLINK(mix)
		int a = 5;
	}



	COMMAND (playTrack)
	{
		#include REFLINK(playTrack)

		channel_01_bass(100, 0, visibility::on);
		channel_02_solo(100, 0, visibility::on);
		mix( 123);
		

		
	
		
	}

}