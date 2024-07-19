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
		#include REFLECT(oscillator)
		int d = 011;
		REFLECT_CLOSE;
	}

	COMMAND (eq, int a)
	{
		#include REFLECT(eq)
		int b = 1;
		REFLECT_CLOSE;
	}

	COMMAND(Notes, int count, unsigned char ...)
	{
		unsigned char params[255];
		va_list args;
		va_start(args, count);
		params[0] = count;
		for (int i = 1; i <= count; i++)
		{
			params[i] = va_arg(args, unsigned char);
		}

		#include REFLECT(Notes)

		REFLECT_CLOSE;
	}

	COMMAND(mainbeat, int pos, int len, int repeat)
	{
		#include REFLECT(mainbeat)

		Notes(3, 255, 0, 255);

		REFLECT_CLOSE;
	}

	COMMAND (channel_01, int volume, int pan)
	{
		#include REFLECT(channel_01)

		mainbeat(0, 19, 10);


		oscillator(1, 2);



		REFLECT_CLOSE;
	}

	COMMAND (channel_02, int volume,int pan)
	{
		#include REFLECT(channel_02)

		int a = 4;
		REFLECT_CLOSE;
	}



	COMMAND (mix, int level)
	{
		#include REFLECT(mix)




		REFLECT_CLOSE;
	}



	COMMAND (playTrack)
	{
		#include REFLECT(playTrack)

		channel_01(100, 0);
		channel_02(100, 0);
		mix(100);
		
		REFLECT_CLOSE;
		
	}

}
