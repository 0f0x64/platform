namespace tracker
{

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

	byte curChannel;

	COMMAND(Pitch, int count, unsigned char ...)
	{
		#include REFLECT(Pitch)

		auto& c = track_desc.channel[curChannel].clip[track_desc.channel[curChannel].clipCounter];

		for (int i = 0; i < count; i++)
		{
			c.note[(int)layers::pitch].note_pitch[i] = params[i];
		}

		c.note[(int)layers::pitch].cmdIndex = cmdCounter - 1;

		REFLECT_CLOSE;
	}

	COMMAND(Clip, timestamp pos, int len, int repeat, int bpmScale, overdub overDub, int swing)
	{
		#include REFLECT(Clip)

		//check range
		track_desc.channel[curChannel].clipCounter++;
		clip_& c = track_desc.channel[curChannel].clip[track_desc.channel[curChannel].clipCounter];

		c.position = pos;
		c.length = len;
		c.repeat = repeat;
		c.bpmScale = bpmScale;
		c.swing = swing;
		c.overDub = overDub;

		c.cmdIndex = cmdCounter - 1;
	

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
	}

	#pragma pop_macro("Draw")
#endif



	COMMAND(kick, volume vol, panorama pan, volume send, switcher solo, switcher mute)
	{
		#include REFLECT(kick)
		regfuncGroup(channel);
		regDrawer(channelDraw);

		track_desc.channel[curChannel].clipCounter = -1;
		
		Clip(28, 11, 6, 1, overdub::off, 0);
		Pitch(11, 2, 0, 0, 0, 0, 0, 0, 255, 255, 0, 0);

		Clip(12, 4, 1, 1, overdub::off, 0);
		Pitch(4, 255, 204, 192, 0);

		oscillator(1, 2);

		curChannel++;

		REFLECT_CLOSE;
	}

#define API(fname, ...) void fname##_impl(__VA_ARGS__)

	//---
	#define func1(a) func1_ref(__FILE__, __LINE__ , a)

	void func1_impl(float a);
	void func1_ref(CALLER_INFO, float a)
	{
		func1_impl(a);
		cmdLevel--;
	}

	API (func1, float a)
	{
	}

	
	void dodd()
	{
		func1(4);
	}



	COMMAND(snare, volume vol, panorama pan, volume send, switcher solo, switcher mute)
	{
		#include REFLECT(snare)
		regfuncGroup(channel);
		regDrawer(channelDraw);

		track_desc.channel[curChannel].clipCounter = -1;

		Clip(29, 11, 6, 1, overdub::off, 0);
		Pitch(8, 255, 0, 255, 255, 255, 247, 255, 255);

		Clip(22, 8, 1, 1, overdub::off, 0);
		Pitch(8, 255, 255, 1, 1, 255, 0, 255, 255);

		oscillator(1, 2);

		curChannel++;

		REFLECT_CLOSE;
	}

	void playTrack_dc(int i, float& x, float& y, float w, float lead, float sel)
	{

	}

	COMMAND (Track, int masterBPM)
	{
		#include REFLECT(Track)
		//regDrawer(playTrack_dc);

		editor::TimeLine::bpm = track_desc.masterBPM = masterBPM;
		curChannel = 0;

		kick(100, 0, 14, switcher::off, switcher::off);
		snare(55, 0, 14, switcher::off, switcher::off);
	
		track_desc.channelsCount = curChannel;

		REFLECT_CLOSE;
		
	}

}
