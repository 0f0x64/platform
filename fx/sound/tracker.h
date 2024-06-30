namespace tracker
{
#define CALLER_INFO const char* srcFileName, int srcLine 
#define EDITABLE __FILE__, __LINE__
#define FUNCTION(fname, ...) void fname(CALLER_INFO, __VA_ARGS__)

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

	API oscillator(int a, int b)
	{
		int d = 011;
	}

	API eq(int a)
	{
		int b = 1;
	}

	API channel_01_bass(int volume, int pan, visibility ms)
	{
		int c = 2;
		api::oscillator(EDITABLE, 1, 2);
	}

	API channel_02_solo(int volume,int pan,visibility ms)
	{
		int a = 4;
	}



#define INCF #include "..\generated\accel.h"


#define REF(fname) \
	if (cmdParamDesc[cmdCounter].loaded) {\
		a = (int)cmdParamDesc[cmdCounter].param[0].value[0];\
	}\
	else {\
		strcpy(cmdParamDesc[cmdCounter].caller.fileName, srcFileName);\
		cmdParamDesc[cmdCounter].caller.line = srcLine;\
		cmdParamDesc[cmdCounter].pCount = 1;\
		cmdParamDesc[cmdCounter].param[0].value[0] = (int)a;\
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "int");\
		cmdParamDesc[cmdCounter].loaded = true;\
	}\
	AddToUI(fname);\
	cmdCounter++;\
	


	FUNCTION(run, int a, int b)
	{
		
		REF(__FUNCTION__);
		#define run(...) run( EDITABLE, __VA_ARGS__)

		int d = 111;
	}

	//#define osc2(target, level) osc2( __FILE__, __LINE__ , target, level)

	//#define func(...) func(__VA_ARGS__, __FILE__, __LINE__)
	
	API mix(int level)
	{
		int a = 5;
	}

	API playTrack()
	{
		api::channel_01_bass(EDITABLE, 100, 0, visibility::on);
	//	api.channel_02_solo(100, 0, visibility::on);
	//	tracker::mix( 123);
	//  tracker::osc2(1, 2);
		
		run(1,2);

	}

}