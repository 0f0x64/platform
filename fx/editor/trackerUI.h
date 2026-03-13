namespace paramEdit 
{
	int currentChannel = -1;
	int currentClip = -1;
	int currentClipIndex = -1;
	int currentNote = -1;
	int currentLayer = -1;
	int currentClipID = -1;

	void clipStyleApply(int i, int k, bool over)
	{
		ui::style::box::rounded = .35;
		ui::style::box::edge = 21;
		ui::style::box::soft = 20;
		ui::style::box::outlineBrightness = over ? .1f : 0.0f;

		if (k == i)
		{
			ui::style::box::r = ui::style::box::g = ui::style::box::b = .5f;
		}

		ui::style::box::a = 1;
	}

	void noteStyleApply(int i, bool over)
	{
		ui::style::box::rounded = .145 * 2.;
		ui::style::box::edge = 150;
		ui::style::box::soft = 1135;
		ui::style::box::outlineBrightness = over ? .5f : 0.3f;

		ui::style::box::rounded = .145;
		//ui::style::box::outlineBrightness = over ? .5f : 0.1f;

	}




	float clipYpos;

	char noteText[7] = "      ";

	void CreateNoteText(BYTE p)
	{
		const char* table = "C-0C#0D-0D#0E-0F-0F#0G-0G#0A-0A#0B-0";
		//const char* table = "C 0C#0D 0D#0E 0F 0F#0G 0G#0A 0A#0B 0";
		const char* vtable = "0123456789";
		strcpy(noteText, " - ");

		if (p == 0) return;

		if (p == 255)
		{
			strcpy(noteText, "off");
			return;
		}

		if (p < 12 * 10 + 1) //in range
		{
			p--;//skip nonote
			int octave = p / 12;
			char _octave[10];
			_itoa(octave, _octave, 10);
			int note = p % 12;

			noteText[0] = table[note * 3];
			noteText[1] = table[note * 3 + 1];
			noteText[2] = _octave[0];
			noteText[3] = 0;
		}

	}

	char noteKey[] = "ZSXDCVGBHNJMQ2W3ER5T6Y7U";

	

}