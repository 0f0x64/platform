namespace paramEdit {

	float yPos=0.05;
	float yPosLast;

	float  storedParam = 0;
	int currentParam;

	void rbDown()
	{
		yPosLast = yPos;
	}

	void lbDown()
	{
		storedParam = cmdParamDesc[currentCmd].params[currentParam][0];
	}
	
	void ShowStack()
	{
		if (editor::ui::rbDown)
		{
			yPos = yPosLast + editor::ui::mouseDelta.y;
		}

		ui::style::Base();


		float top = ui::style::text::height;
		float bottom = 1.f - ui::style::text::height*2.5f;

		float lead = ui::style::text::height;

		gapi.setScissors(0, top, 1, bottom);

		yPos = max(yPos, top - cmdCounter * lead + lead);
		yPos = min(yPos, bottom - lead);

		float inside = ui::style::text::width * .5f * aspect / 2.f;
		float x = ui::style::text::width / 2.f; 
		float y = yPos;
		ui::Box::Setup();

		for (int i = 0; i < cmdCounter; i++)
		{
			auto w = ui::Text::getTextLen(cmdParamDesc[i].funcName, ui::style::text::width)+inside*2.f;

			if (editor::ui::mousePos.x > x && editor::ui::mousePos.x < x + w &&
				editor::ui::mousePos.y > y - ui::style::text::height * .1 &&
				editor::ui::mousePos.y < y + ui::style::text::height * .9)
			{
				ui::style::box::outlineBrightness = 1.f;

				if (ui::lbDown) {
					currentCmd = i;
					currentParam = 0;
				}
			}
			else
			{
				ui::style::box::outlineBrightness = 0.1f;
			}

			float sel = currentCmd == i ? 1.f : 0.f;
			ui::style::box::r = lerp(0.2f, .8f, sel);
			ui::style::box::g = lerp(0.2f, .8f, sel);
			ui::style::box::b = lerp(0.2f, .8f, sel);

			ui::Box::Draw(x, y- ui::style::text::height*.1f,w,ui::style::text::height*.8f);
			y += lead;
		}


		y = yPos;
		ui::Text::Setup();
		for (int i = 0; i < cmdCounter; i++)
		{
			float sel = currentCmd == i ? 1.f : 0.f;
			ui::style::text::r = lerp(1.f, .0f, sel);
			ui::style::text::g = lerp(1.f, .0f, sel);
			ui::style::text::b = lerp(1.f, .0f, sel);

			ui::Text::Draw(cmdParamDesc[i].funcName, x + inside, y);
			y += lead;
		}

		ui::style::Base();
		x += .12;
		for (int i = 0; i < cmdParamDesc[currentCmd].pCount; i++)
		{
			auto w = ui::Text::getTextLen(cmdParamDesc[currentCmd].paramName[i],ui::style::text::width);

			float y = yPos + float(currentCmd + i) * lead;

			if (editor::ui::mousePos.x > x && editor::ui::mousePos.x < x + w &&
				editor::ui::mousePos.y > y &&
				editor::ui::mousePos.y < y + ui::style::text::height * .9)
			{
				if (ui::lbDown)
				{
					currentParam = i;
				}
			}

			float sel = currentParam == i ? 1 : 0;
			ui::style::text::r = lerp(.5, 1, sel);
			ui::style::text::g = lerp(.5, 1, sel);
			ui::style::text::b = lerp(.5, 1, sel);

			ui::Text::Draw(cmdParamDesc[currentCmd].paramName[i], x, yPos + float(currentCmd + i) * lead);


			if ((strcmp(cmdParamDesc[currentCmd].paramType[i], "int") == 0) ||
				(strcmp(cmdParamDesc[currentCmd].paramType[i], "float") == 0))
			{
				char val[32];
				_gcvt(cmdParamDesc[currentCmd].params[i][0], 5, val);
				ui::Text::Draw(val, x + .22, yPos + float(currentCmd + i) * lead);
			}

			if ((strcmp(cmdParamDesc[currentCmd].paramType[i], "position") == 0) ||
				(strcmp(cmdParamDesc[currentCmd].paramType[i], "size") == 0))
			{

				if (ui::lbDown)
				{
					cmdParamDesc[currentCmd].params[i][0] = storedParam + ui::mouseDelta.x;
				}

				char val[32];
				char vstr[132];
				_gcvt(cmdParamDesc[currentCmd].params[i][0], 5, val);
				strcpy(vstr, val);
				_gcvt(cmdParamDesc[currentCmd].params[i][1], 5, val);
				strcat(vstr, " : "); strcat(vstr, val);
				_gcvt(cmdParamDesc[currentCmd].params[i][2], 5, val);
				strcat(vstr, " : "); strcat(vstr, val);

				ui::Text::Draw(vstr, x + .22, yPos + float(currentCmd + i) * lead);
			}


		}


		gapi.setScissors(0, 0, 1, 1);
	}

}
