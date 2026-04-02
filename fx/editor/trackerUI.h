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


#include <fstream>
#include <string>
#include <cstdlib>

	static const int NOTE_OFFSETS[] = { 9, 11, 0, 2, 4, 5, 7 };

	// Вспомогательная функция для безопасного копирования "слова" в char[]
	void safe_copy(char* dest, const std::string& str, size_t& i) {
		size_t start = i;
		while (i < str.size() && !std::isspace((unsigned char)str[i])) {
			i++;
		}
		size_t len = i - start;
		if (len >= maxClipLen) len = maxClipLen - 1;
		std::strncpy(dest, str.c_str() + start, len);
		dest[len] = '\0';
	}

	void parseNoteLine(const std::string& line, int patternID, int posInPattern) {

		Track.pattern[patternID].pitch[posInPattern] = -1;

		if (line.empty()) {
			return;
		}

		size_t i = 0;

		while (i < line.size()) {
			// Пропуск пробелов и табов
			if (std::isspace((unsigned char)line[i])) {
				i++;
				continue;
			}

			char current = line[i];

			// 1. Определение ноты (c, d, e, f, g, a, b)
			char cL = std::tolower(current);

			if (cL >= 'a' && cL <= 'g') {

				char note = NOTE_OFFSETS[cL - 'a'];

				size_t start = i;
				i++;
				// Проверка на знак альтерации (#, b)
				if (i < line.size() && (line[i] == '#'))
				{
					note++;
					i++;
				}

				Track.pattern[patternID].pitch[posInPattern] = note;

				//size_t len = i - start;
				//std::strncpy(Track.pattern[patternID].pitch, line.c_str() + start, len);
				//Track.pattern[patternID].pitch[len] = 0;
				continue;
			}

			// 2. Определение октавы (если это цифра без префикса)
			//if (std::isdigit(current)) {
				//safe_copy(Track.pattern[patternID].octave, line, i);
				//continue;
			//}

			// 3. Определение модификаторов по первой букве
			i++; // Пропускаем букву-префикс
			/*switch (current) {
			case 'v': safe_copy(Track.pattern[patternID].velocity, line, i); break;
			case 'p': safe_copy(Track.pattern[patternID].position, line, i); break;
			case 'i': safe_copy(Track.pattern[patternID].take, line, i);     break;
			case 'f': safe_copy(Track.pattern[patternID].offset, line, i);   break;
			case 's': safe_copy(Track.pattern[patternID].slide, line, i);    break;
			case 'r': safe_copy(Track.pattern[patternID].retrigger, line, i); break;
			default:
				// Если встретили неизвестный символ, просто пропускаем до пробела
				while (i < line.size() && !std::isspace((unsigned char)line[i])) i++;
				break;
			}*/
		}
	}

	void CompilePattern(char* name, int id)
	{
		std::string n = name;
		const std::string& filePath = "..\\fx\\projectFiles\\sound\\" + n + ".pattern";

		std::ifstream file(filePath);
		if (!file.is_open()) return;

		std::string line;
		int noteID = 0;

		int oct = 0;
		int pos = 0;

		while (std::getline(file, line)) {
	
			parseNoteLine(line, id, pos);
			pos++;


		}

		file.close();

		auto a = Track.pattern[id].pitch;
	}

	void CompilePatterns() {

		int pID = 0;

		for (int i = 0; i < Track.channelsCount; i++)
		{
			for (int j = 0; j < Track.channel[i].clipsCount; j++)
			{
				char* name = Track.channel[i].clip[j].pattern;

				CompilePattern(name, pID);

				Track.channel[i].clip[j].patternID = pID;
				pID++;
			}
		}
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

		CompilePatterns();
		/*
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

				auto notes = Track.pattern[Track.channel[i].clip[j].patternID];

				for (int k = 0; k < strlen(notes.pitch); k++)
				{
					float x = clipX + noteScreenLen * k;
					int octIndex = 0;
					if (strlen(notes.octave)>0) octIndex = min(k, strlen(notes.octave-1));
					char oct = notes.octave[octIndex];
					unsigned char note = getNoteFromChar(notes.pitch[k], oct);

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
		*/
	}

}