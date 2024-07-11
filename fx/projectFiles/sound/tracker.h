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
		refStackBack;
	}

	COMMAND (eq, int a)
	{
		#include REFLINK(eq)
		int b = 1;
		refStackBack;
	}


	COMMAND(mainbeat, int pos, int len, int repeat)
	{
		#include REFLINK(mainbeat)

		//notes = "DdddDded";
		//vol = "1123";
		//send = "00001";


		refStackBack;
	}

	COMMAND (channel_01, int volume, int pan)
	{
		#include REFLINK(channel_01)

		mainbeat(0, 19, 10);


		oscillator(1, 2);



		refStackBack;
	}

	COMMAND (channel_02, int volume,int pan)
	{
		#include REFLINK(channel_02)

		int a = 4;
		refStackBack;
	}



	COMMAND (mix, int level)
	{
		#include REFLINK(mix)




		refStackBack;
	}



	COMMAND (playTrack)
	{
		#include REFLINK(playTrack)

		channel_01(100, 0);
		channel_02(100, 0);
		mix( 100);
		
		refStackBack;
		
	}

}