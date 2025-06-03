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

	int currentOctave = 2;
	int currentStep = 1;

	void showTrackControls()
	{
		ui::style::button::selected = false;
		
		Rasterizer::Scissors({ 0,0,(float)dx11::width,(float)dx11::height });
		ui::Box::Setup();
		ui::style::Base();
		ui::style::button::hAlign = ui::style::align_h::center;
		ui::style::button::zoom = true;
		char octaveText[32];
		char octaveNum[2];
		strcpy(octaveText, "[-] octave: ");
		_itoa(currentOctave, octaveNum, 10);
		strcat(octaveText, octaveNum);
		strcat(octaveText, " [+]");

		float btn_x=.0;
		float btn_y = ui::style::text::height;
		float btn_w = ui::style::box::width;
		float btn_h = ui::style::box::height;

		ui::style::button::zoom = false;

		if (paramEdit::ButtonPressed(octaveText, btn_x, btn_y, btn_w, btn_h) && drag.isFree())
		{
			currentOctave += isMouseOver(btn_x, btn_y, btn_w / 2.f, btn_h) ? -1 : 1;
			currentOctave = clamp(currentOctave, 0, 8);

			drag.set(buttonIndex);
			currentCmd = -1;
			paramEdit::currentParam = -1;
		}

		btn_x += ui::style::text::width/2.+ btn_w;
		char stepText[32];
		char stepNum[2];
		strcpy(stepText, "[-] step: ");
		_itoa(currentStep, stepNum, 10);
		strcat(stepText, stepNum);
		strcat(stepText, " [+]");

		if (paramEdit::ButtonPressed(stepText, btn_x, btn_y, btn_w, btn_h) && drag.isFree())
		{
			currentStep += isMouseOver(btn_x, btn_y, btn_w / 2.f, btn_h) ? -1 : 1;
			currentStep = clamp(currentStep, 0, 8);

			drag.set(buttonIndex);
			currentCmd = -1;
			paramEdit::currentParam = -1;
		}

	}

	void showTrack()
	{
		TimeLine::screenLeft = .15*1920./dx11::width;;

		float xScaller = 1920. / dx11::width;
		float yScaller = 1080. / dx11::height;
		float yTextH = ui::style::text::height * 1080. / dx11::height;

		int frame = SAMPLING_FREQ / FRAMES_PER_SECOND;
		x = ui::style::text::height / 6.f;
		ui::Box::Setup();

		if (drag.isFree())
		{
			currentClipIndex = -1;
		}
		bool clippingTest = ui::mousePos.x >= TimeLine::screenLeft && ui::mousePos.x < TimeLine::screenRight;

			clipYpos = ui::style::box::height*2.6*yScaller;
			buttonIndex = 0;


			

			for (int ch = 0; ch < track.channelsCount; ch++)
			{
				Rasterizer::Scissors(float4{ 0, (float)top * dx11::height, (float)dx11::width, (float)(bottom * dx11::height) });
				float ch_lead = ui::style::box::height * 6.2f* yScaller;

				float ch_y = clipYpos;
				float ch_h = ch_lead * .8f;
				bool over = isMouseOver(0, ch_y - x / 2.f, TimeLine::screenLeft - x, ch_lead * .9f);

				ui::style::Base();
				ui::style::BaseButton();
				ui::style::button::zoom = false;
				ui::style::button::vAlign = ui::style::align_v::top;

				float channelW = TimeLine::screenLeft - x;
				float channelH = ch_lead * .9f;
				if (currentChannel == ch)
				{
					ui::style::button::selected = true;
				}
				if (ButtonPressed("channel", 0, ch_y, TimeLine::screenLeft - x, ch_lead * .9f))
				{
					currentChannel = ch;
				}

				ui::style::box::rounded = .5f;
				float bw = aspect * .05 / 2.2f;
				float bx = TimeLine::screenLeft - x * 2.f - bw;

				ui::style::button::zoom = true;
				ui::style::button::hAlign = ui::style::align_h::center;
				ui::style::button::vAlign = ui::style::align_v::center;
				ui::style::box::rounded = .5f;
				float sm_h = ch_h / 2.2f;
				float slot_h = ch_h / 2.f;
				float low = ch_y + ch_h / 2.f + (slot_h - sm_h) / 2.f;

				ui::style::button::zoom = true;
				processSwitcher("solo", bx, ch_y + (slot_h - sm_h) / 2.f, bw, bw, ch, track.channels[ch].solo, "   S   ");
				processSwitcher("mute", bx, low, bw, bw, ch, track.channels[ch].mute, "   M   ");

				ui::style::button::inverted = false;
				ui::style::box::rounded = .15f;

				ui::style::button::zoom = false;

				processSlider("vol", x, ch_y + yTextH, channelW / 1.75, yTextH, track.channels[ch].vol, dir::x, 0, 100);

				ui::style::box::rounded = .5f;

				ui::style::button::zoom = true;
				processSlider("   pan   ", x + bw * 5.2, ch_y + slot_h / 10., .035*aspect, .035, track.channels[ch].pan, dir::r, -90, 90, false);
				ui::Text::Draw(std::to_string(track.channels[ch].pan).c_str(), x + bw * 5.2 + channelH / 1.75 / 2. * aspect, ch_y + slot_h * 1.25, channelH / 1.9, channelH / 1.9, true);

				ui::style::button::zoom = false;
				ui::style::Base();


				Rasterizer::Scissors(float4{ (float)(TimeLine::screenLeft * dx11::width), (float)(top * dx11::height), (float)dx11::width, (float)(bottom * dx11::height) });

				for (int clp = 0; clp <= track.channels[ch].clipsCount; clp++)
				{
					auto* clp_desc = &track.channels[ch].clips[clp];
					if (clp_desc->repeat == 0) continue;
					
					float sWidth = TimeLine::TimeToScreen(frame * 60 * 60 * clp_desc->len * clp_desc->repeat / (editor::TimeLine::bpm * clp_desc->bpmScale));
					float h = ch_h;
					float clip_x = TimeLine::getScreenPos(frame * 60 * 60 * clp_desc->pos / editor::TimeLine::bpm);
					float clip_y = clipYpos;
					float note_step = sWidth / clp_desc->len / clp_desc->repeat;
					bool over = drag.isFree() && isMouseOver(clip_x, clip_y, sWidth, h);

					if (ui::mousePos.x < TimeLine::screenLeft || ui::mousePos.x >= TimeLine::screenRight)
					{
						over = false;
						if (ui::lbDown) {
							currentClipIndex = -1;
						}
					}

					buttonIndex++;
					if (over && ui::lbDown && drag.isFree())
					{
			

						storedParam = clp_desc->pos;
						currentClipIndex = buttonIndex;
						currentClipID = clp;
						currentChannel = ch;
					}



					float c = (float)(clp + 222);
					//ui::style::box::r = .4f + .3f * sin(c * 12.123f);
					//ui::style::box::g = .4f + .3f * sin(c * 23.123f);
					//ui::style::box::b = .4f + .3f * sin(c * 44.123f);
					clipStyleApply(buttonIndex, currentClipIndex, over);
					//ui::Box::Draw(clip_x, clip_y, sWidth, h / 3);

					for (int r = 0; r < clp_desc->repeat; r++)
					{
						ui::Box::Draw(clip_x + r * clp_desc->len * note_step, clip_y, note_step * clp_desc->len, h / 4.f);
					}


					/*
					// show all layers
					for (int r = 0; r < clp_desc->repeat; r++)
					{
						for (int n = 1; n <= clp_desc->len; n++)
						{

							float h = ui::style::text::height * .58f / 1.f;
							float x = clip_x + (n - 1) * note_step + (r * clp_desc->len * note_step);
							float y = clip_y + h;

							bool over = clippingTest && drag.isFree() && isMouseOver(x, y, note_step, h);

							if (iter == 0)
							{
								//if (over) topUIElementIndex = pitchLayerIndex;
								continue;
							}

							//over = (pitchLayerIndex == topUIElementIndex) && over;

							if (iter == 1)
							{
								if (ui::lbDown && over)
								{
									currentChannel = ch;
									//currentClipIndex = clipIndex;
									currentClip = clp;
								}

								//noteStyleApply(pitchLayerIndex, over);

								//ui::style::BaseColor((n - 1 == currentNote) && currentClipIndex == clipIndex);

								auto pitchValue = track.channels[ch].clips[clp].pitch[n - 1];
								if (pitchValue == 0)
								{
									ui::style::box::r *= 0.75;
									ui::style::box::g *= 0.75;
									ui::style::box::b *= 0.75;
								}

								ui::Box::Draw(x, y, note_step, h);
								CreateNoteText(pitchValue);

								int lp = 2;
								while (note_step < ui::Text::getTextLen("A#0", ui::style::text::width, lp + 1) && lp >= 0)
								{
									noteText[lp--] = 0;
								}

								ui::Text::Draw(noteText, x, y);

								if (over && ui::lbDown)
								{
									storedParam = pitchValue;
									//currentCmd = pitchLayerIndex;
									currentParam = n;

									currentChannel = ch;
									currentClip = clp;
									currentNote = n - 1;
									currentLayer = 0;

								}
							}
						}
					}


				}
				*/


				}
				clipYpos += ch_lead;
			}


			if (ui::lbDown && currentClipIndex >= 0)
			{
				drag.set(currentClipIndex);
				track.channels[currentChannel].clips[currentClipID].pos = max(0, storedParam + TimeLine::ScreenToTime(ui::mouseDelta.x) / (frame * 60 * 60 / editor::TimeLine::bpm));

			}

		showTrackControls();
	}



	int getNoteFromKey(WPARAM wParam)
	{
		if (GetAsyncKeyState(VK_CONTROL)||
			GetAsyncKeyState(VK_SHIFT) || 
			GetAsyncKeyState(164)) return -1;

		char noteKey[] = "ZSXDCVGBHNJMQ2W3ER5T6Y7U";
		int s = strlen(noteKey);
		for (int i = 0; i < s; i++)
		{
			if (noteKey[i] == (char)wParam)
			{
				return i + currentOctave * 12 + 1;
			}
		}

		if (wParam == VK_OEM_3) return 255;

		return -1;
	}

	void stepControl(WPARAM wParam)
	{
		if (GetAsyncKeyState(VK_CONTROL) && (wParam >= '0' && wParam <= '8'))
		{
			currentStep = wParam - '0';
		}
	}

	void trackerKeys(WPARAM wParam)
	{

		stepControl(wParam);

		switch (wParam)
		{
		case VK_OEM_4:
			currentOctave = clamp(currentOctave - 1, 0, 8);
			break;
		case VK_OEM_6:
			currentOctave = clamp(currentOctave + 1, 0, 8);
			break;
		}

		//note editing section
		if (currentChannel < 0 || currentClip < 0 && currentNote < 0) return;

		/*
		auto i = tracker::track.channels[currentChannel].clip[currentClip].note[(int)layers::pitch].cmdIndex;
		auto note = getNoteFromKey(wParam);

		if (note >= 0)
		{
			cmdParamDesc[i].param[currentNote + 1].value = note;
			if (currentNote + currentStep < tracker::track.channel[currentChannel].clip[currentClip].length)
			{
				currentNote += currentStep;
			}
			//currentNote = currentNote % tracker::track.channel[currentChannel].clip[currentClip].length;
		}

		switch (wParam)
		{
			case VK_DELETE:
			{
				auto len = tracker::track.channel[currentChannel].clip[currentClip].length;	
				auto index = tracker::track.channel[currentChannel].clip[currentClip].note[(int)layers::pitch].cmdIndex;
				for (int n = currentNote; n < len-1 ; n++)
				{
					cmdParamDesc[index].param[n+1].value = cmdParamDesc[index].param[n+2].value;
				}
				cmdParamDesc[index].param[len].value = 0;
				break;
			}

			case VK_INSERT:
			{
				auto len = tracker::track.channel[currentChannel].clip[currentClip].length;
				auto index = tracker::track.channel[currentChannel].clip[currentClip].note[(int)layers::pitch].cmdIndex;
				for (int n = len - 2; n >= currentNote; n--)
				{
					cmdParamDesc[index].param[n + 2].value = cmdParamDesc[index].param[n + 1].value;
				}
				cmdParamDesc[index].param[currentNote+1].value = 0;
				break;
			}


			case VK_RETURN:
				cmdParamDesc[i].param[currentNote+1].value=0;
				currentNote += currentStep;
				currentNote = currentNote % tracker::track.channel[currentChannel].clip[currentClip].length;
				break;
			case VK_LEFT:
				if (currentNote > 0) currentNote--;
				break;
			case VK_RIGHT:
				if (currentNote < tracker::track.channel[currentChannel].clip[currentClip].length - 1) currentNote++;
				break;

		}*/
	}

}