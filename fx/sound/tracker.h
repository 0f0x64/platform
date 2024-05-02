typedef void(*PVFN)();

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


namespace synth
{

	void oscillator(int a, int b)
	{
	}

	void filter(int a)
	{

	}


}

namespace instrument
{
	int getEnv(int a)
	{
		return 1;
	}

	void bass()
	{
		envelope_ freq;

		synth::oscillator(
			0,
			2 + getEnv(0)
		);

		synth::filter(2222);
	}

}



struct {

	struct
	{
		clip_ clip[CLIPS_IN_CHANNEL];

		byte volume;
		byte pan;
		byte send[4];
		envelope_ volumeEnv;
		envelope_ panEnv;
		envelope_ sendEnv[4];
		bool solo;
		bool mute;

		PVFN sound;

	} channel[CHANNELS];

	void master()
	{

	}


	float masterBPM;

} track;



void playTrack()
{
	/*track.channel[0].sound = &instrument::bass;
	track.channel[0].sound();
	auto n = track.channel[0].clip[0].layers.note;
	auto m = track.channel[0].clip[1].layers.note;*/


		



	int d = sizeof track;
	int b = 0;
	//track.channel[4].clip[0].layers.note[4] = 1;
	
}