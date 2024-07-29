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
		unsigned char pitch;
		unsigned char variation;
		unsigned char retrigger;
		unsigned char slide;
		unsigned char send[4];
	} note_;

	typedef struct
	{
		note_ note[NOTES_IN_CLIP];

		int position;
		int length;
		int repeat;
		int bpmScale;
		float swing;
		bool mode;//overwrite or mix

	} clip_;


	struct {

		clip_ clip[MAX_CLIPS];

		struct
		{
			byte volume;
			byte pan;

			envelope_ volumeEnv;
			envelope_ panEnv;
			envelope_ sendEnv[4];


		} channel[CHANNELS];

		float masterBPM;

	} track_desc;

	COMMAND(oscillator, int a, int b)
	{
		#include REFLECT(oscillator)
		
		int d = 011;
		
		REFLECT_CLOSE;
	}

	COMMAND(eq, int a)
	{
		#include REFLECT(eq)

		int b = 1;

		REFLECT_CLOSE;
	}

	COMMAND(Pitch, int count, unsigned char ...)
	{
		#include REFLECT(Pitch)

		auto c = track_desc.clip[clipCounter];
		for (int i = 0; i < count; i++)
		{
			c.note[i].pitch = params[i];
		}

		REFLECT_CLOSE;
	}

	COMMAND(Clip, int pos, int len, int repeat, int bpmScale, bool mode, int swing)
	{
		#include REFLECT(Clip)

		//check range

		clip_* c = &track_desc.clip[++clipCounter];

		c->position = pos;
		c->length = len;
		c->repeat = repeat;
		c->bpmScale = bpmScale;
		c->mode = mode;
		c->swing = swing;

		REFLECT_CLOSE;
	}

#if EditMode
	#pragma push_macro("Draw")
	#undef Draw

	void channelDraw(int i, float &x, float &y, float w, float lead, float sel)
	{
		w = editor::paramEdit::tabLen;
		editor::paramEdit::mouseOverItem = editor::paramEdit::showButton(x, y, w, editor::ui::style::text::height * 2.f - editor::ui::style::text::height * .2f , sel, i);

		/*
		editor::paramEdit::mouseOverItem = editor::isMouseOver(x, y, w, editor::ui::style::box::height*2);
		editor::ui::style::box::outlineBrightness = editor::paramEdit::mouseOverItem ? 1.f : 0.1f;

		editor::ui::Box::Setup();
		editor::paramEdit::setBStyle(sel);
		editor::ui::Box::Draw(x, y, w, editor::ui::style::text::height * .8f*2.);

		editor::ui::Text::Setup();
		editor::paramEdit::setTStyle(sel);
		editor::ui::Text::Draw(cmdParamDesc[i].funcName, x + editor::paramEdit::insideX, y + editor::paramEdit::insideY);*/

		y += lead*2;

		editor::paramEdit::expandTree = true;
	}

	#pragma pop_macro("Draw")
#endif



	COMMAND(channel_01, volume vol, panorama pan, volume send, switcher solo, switcher mute)
	{
		#include REFLECT(channel_01)
		regDrawer(channelDraw);

		clipCounter = -1;

		Clip(0, 16, 4, 1, false, 0);
		Pitch(10, 255, 204, 192, 1, 0, 0, 0, 0, 0, 0);

		Clip(0, 16, 4, 1, false, 0);
		Pitch(10, 255, 204, 192, 0, 0, 0, 0, 0, 0, 0);

		oscillator(1, 2);

		REFLECT_CLOSE;
	}

	COMMAND (playTrack)
	{
		#include REFLECT(playTrack)
		
		channel_01(0, 90, 14, switcher::off, switcher::off);
	
		REFLECT_CLOSE;
		
	}

}
