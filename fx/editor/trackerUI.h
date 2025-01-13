namespace paramEdit 
{

	int currentClip = -1;
	int currentNote = -1;
	int currentPitchLayer = -1;

	void showTrack()
	{

		x = ui::style::text::height / 6.f * aspect;
		ui::Box::Setup();

		int topUIElementIndex = -1;
		int channelIndex = 0;

		bool clippingTest = ui::mousePos.x >= TimeLine::screenLeft && ui::mousePos.x < TimeLine::screenRight;

		for (int iter = 0; iter < 2; iter++)
		{
			clipYpos = ui::style::box::height;
			TimeLine::screenLeft = x + enumDrawOffset - insideX;

			for (int ch = 0; ch < tracker::track_desc.channelsCount; ch++)
			{
				gapi.setScissors(rect{ 0, (int)(top * dx11::height), dx11::width, (int)(bottom * dx11::height) });
				channelIndex = tracker::track_desc.channel[ch].cmdIndex;
				float  ch_lead = ui::style::box::height * 2.2;
				clipYpos += ch_lead;

				float ch_y = clipYpos;
				float ch_h = ch_lead * .8;
				bool over = isMouseOver(0, ch_y - x / 2., TimeLine::screenLeft - x, ch_lead * .9);

				if (iter == 1)
				{
					ui::style::Base();
					ui::style::BaseButton();
					ui::style::button::zoom = false;
					ui::style::button::vAlign = ui::style::align_v::top;
					Button(channelIndex, cmdParamDesc[channelIndex].funcName, 0, ch_y - x / 2., TimeLine::screenLeft - x, ch_lead * .9);

					ui::style::box::rounded = .5;
					float bw = aspect * ch_h / 2.2;
					float bx = TimeLine::screenLeft - x * 2 - bw;

					ui::style::button::zoom = true;
					ui::style::button::hAlign = ui::style::align_h::center;
					ui::style::button::vAlign = ui::style::align_v::center;
					ui::style::box::rounded = .5;
					float sm_h = ch_h / 2.2;
					float slot_h = ch_h / 2.;
					float low = ch_y + ch_h / 2. + (slot_h - sm_h) / 2.;

					processSwitcher(channelIndex, "solo", bx, ch_y + (slot_h - sm_h) / 2., bw, sm_h, "S");
					processSwitcher(channelIndex, "mute", bx, low, bw, sm_h, "M");

					ui::style::button::inverted = false;
					ui::style::box::rounded = .15;

					processSlider(channelIndex, "vol", x, low, bw * 4, sm_h);
					processSlider(channelIndex, "pan", x + bw * 4.5, low, bw * 2, sm_h);

					ui::style::Base();
				}

				gapi.setScissors(rect{ (int)(TimeLine::screenLeft * dx11::width), (int)(top * dx11::height), dx11::width, (int)(bottom * dx11::height) });

				for (int clp = 0; clp <= tracker::track_desc.channel[ch].clipCounter; clp++)
				{
					int clipIndex = tracker::track_desc.channel[ch].clip[clp].cmdIndex;
					auto clp_desc = tracker::track_desc.channel[ch].clip[clp];
					float frame = SAMPLING_FREQ / FRAMES_PER_SECOND;
					float sWidth = TimeLine::TimeToScreen(frame * 60 * 60 * clp_desc.length * clp_desc.repeat / (editor::TimeLine::bpm * clp_desc.bpmScale));
					float h = ch_h;
					float clip_x = TimeLine::getScreenPos(frame * 60 * 60 * clp_desc.position / editor::TimeLine::bpm);
					float clip_y = clipYpos;
					float note_step = sWidth / clp_desc.length / clp_desc.repeat;
					bool over = drag.isFree() && isMouseOver(clip_x, clip_y, sWidth, h);

					if (ui::mousePos.x < TimeLine::screenLeft || ui::mousePos.x >= TimeLine::screenRight) over = false;

					if (iter == 0)
					{
						if (over) topUIElementIndex = clipIndex;
					}
					else
					{
						over = over && (clipIndex == topUIElementIndex);

						int posIndex = getParamIndexByStr(clipIndex, "pos");

						if (over && ui::lbDown && drag.isFree())
						{
							drag.set(clipIndex, posIndex, 0);
							storedParam[0] = cmdParamDesc[clipIndex].param[posIndex].value[0];
							currentCmd = clipIndex;
							currentClip = clipIndex;
						}

						if (ui::lbDown && drag.check(clipIndex, posIndex, 0))
						{
							cmdParamDesc[clipIndex].param[posIndex].value[0] = storedParam[0] + TimeLine::ScreenToTime(ui::mouseDelta.x) / (frame * 60 * 60 / editor::TimeLine::bpm);
							pLimits(clipIndex, posIndex);
						}

						float c = clp + 222;
						ui::style::box::r = .4 + .3 * sin(c * 12.123);
						ui::style::box::g = .4 + .3 * sin(c * 23.123);
						ui::style::box::b = .4 + .3 * sin(c * 44.123);
						clipStyleApply(clipIndex, over);
						ui::Box::Draw(clip_x, clip_y, sWidth, h / 3);
						for (int r = 0; r < clp_desc.repeat; r++)
						{
							ui::Box::Draw(clip_x + r * clp_desc.length * note_step, clip_y + h / 1.5, note_step * clp_desc.length, h / 3);
						}

					}

					// show all layers

					for (int r = 0; r < clp_desc.repeat; r++)
					{
						int pitchLayerIndex = tracker::track_desc.channel[ch].clip[clp].note[(int)layers::pitch].cmdIndex;

						for (int n = 1; n <= clp_desc.length; n++)
						{

							float h = ui::style::text::height * .58f / 1.f;
							float x = clip_x + (n - 1) * note_step + (r * clp_desc.length * note_step);
							float y = clip_y + h;

							bool over = clippingTest && drag.isFree() && isMouseOver(x, y, note_step, h);

							if (iter == 0)
							{
								if (over) topUIElementIndex = pitchLayerIndex;
								continue;
							}

							over = (pitchLayerIndex == topUIElementIndex) && over;

							if (ui::lbDown && over)
							{
								currentNote = n - 1;
								currentClip = clipIndex;
							}

							noteStyleApply(pitchLayerIndex, over);

							ui::style::BaseColor( (n - 1 == currentNote) && currentClip == clipIndex );
							//ui::style::BaseColor();
							auto pitchValue = tracker::track_desc.channel[ch].clip[clp].note[(int)layers::pitch].note_pitch[n - 1];
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
								storedParam[0] = pitchValue;
								currentCmd = pitchLayerIndex;
								currentParam = n;
							}

						}
					}

				}
			}
		}

	}
}