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

		//if (p == 0) return;

		if (p == 255)
		{
			strcpy(noteText, "off");
			return;
		}

		if (p < 12 * 10) //in range
		{
			//p--;//skip nonote
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

	float getScreenPos(int x)
	{
		return TimeLine::TimeToScreen(x) - TimeLine::left + TimeLine::screenLeft;
	}

	float getScreenLen(int x)
	{
		return TimeLine::TimeToScreen(x);
	}

	float getNoteLen(float targetNote) {

		return (TimeLine::minute / (float)Track.masterBPM) * ((float)(Track.timeDenominator) / targetNote);
	}

	bool tableInit = false;
	unsigned char table[255];

	unsigned char getNoteFromChar(char note,char octave)
	{
		if (!tableInit) {
			memset(table, 255, 255);
			const char* str = "cCdDefFgGaAb";
			for (int i = 0; i < strlen(str); i++)
			{
				table[str[i]] = i;
			}
		}
		if (!isdigit(octave)) return 255;
		if (table[note] == 255) return 255;

		int oct = octave - '0';

		return table[note] * oct;
	}

	void showTrack()
	{
		float baseH = .05;
		float y = .5;

		ui::style::Base();
		editor::ui::style::box::outlineBrightness = 0;
		editor::ui::style::box::edge = 100;
		editor::ui::style::box::rounded = .001;
		ps::letter_ps.samplers.s1Filter = filter::linear;
		ps::letter_ps.samplers.s1AddressU = addr::clamp;
		ps::letter_ps.samplers.s1AddressV = addr::clamp;
		ps::letter_ps.textures.tex = (texture)ui::fontTextureIndex;

		Blend::Set(blendmode::alpha);
//		InputAssembler::IA(topology::lineList);
//		DrawMakers(1);

		Blend::Set(blendmode::alpha);
		InputAssembler::IA(topology::triList);

		for (int i = 0; i < Track.channelsCount; i++)
		{
			for (int j = 0; j < Track.channel[i].clipsCount; j++)
			{
				auto pos = Track.channel[i].clip[j].pos;
				auto clipLen = Track.channel[i].clip[j].len;
				float noteDuration = (float)Track.channel[i].clip[j].bpmScaleDenominator/ (float)Track.channel[i].clip[j].bpmScaleNumerator;

				auto noteLen = getNoteLen(noteDuration);
				auto noteScreenLen = getScreenLen(noteLen);

				float clipX = getScreenPos(pos * noteLen);
				float clipL = getScreenLen(clipLen * noteLen);

				float areaHeight = 1;
				float totalNotesRange = 8 * 12;
				float noteHeight = areaHeight / totalNotesRange;

				for (int k = 0; k < strlen(Track.channel[i].clip[j].pitch); k++)
				{
					float x = clipX + noteScreenLen * k;
					int octIndex = 0;
					if (strlen(Track.channel[i].clip[j].octave)>0) octIndex = min(k, strlen(Track.channel[i].clip[j].octave-1));
					char oct = Track.channel[i].clip[j].octave[octIndex];
					unsigned char note = getNoteFromChar(Track.channel[i].clip[j].pitch[k], oct);

					float y = areaHeight - .1- noteHeight * note ;

					editor::ui::style::box::a = 1.f;
					editor::ui::style::box::r = .25f;
					editor::ui::style::box::g = .25f;
					editor::ui::style::box::b = .25f;

					CreateNoteText(note);

					ui::Box::Draw(x, y, noteScreenLen, noteHeight);

					ui::style::text::height = noteHeight*2;
					ui::style::text::width = noteHeight*2;

					float textW = ui::Text::getTextLen(noteText, ui::style::text::width);
					if (textW < noteScreenLen)
					{
						ui::Text::Draw(noteText, x, y);
					}
				}
			}
		}
	}

}