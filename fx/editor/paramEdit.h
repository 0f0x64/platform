namespace paramEdit {

	float yPos=0;
	float yPosLast;

	void rbDown()
	{
		yPosLast = yPos;
	}

	void ShowStack()
	{

		if (editor::ui::rbDown)
		{
			yPos = yPosLast + editor::ui::mouseDelta.y;
		}

		float top = ui::style::text::height;
		float bottom = 1.f - ui::style::text::height*2.5f;
		gapi.setScissors(0, top, 1, bottom);
		float lead = ui::style::text::height;

		yPos = max (yPos, top - cFunc * lead+lead);
		yPos = min(yPos, bottom - lead);

		ui::style::Base();
		ui::style::text::width *= .8f;
		float x = ui::style::text::width/2.f; 

		float y = yPos;
		ui::Box::Setup();
		for (int i = 0; i < cFunc; i++)
		{
			auto w = ui::Text::getTextLen(callList[i].funcName)+ ui::style::text::width * .2;

			if (editor::ui::mousePos.x > x && editor::ui::mousePos.x < x + w &&
				editor::ui::mousePos.y > y - ui::style::text::height * .1 &&
				editor::ui::mousePos.y < y + ui::style::text::height * .9)
			{
				ui::style::box::outlineBrightness = 111.f;
			}
			else
			{
				ui::style::box::outlineBrightness = 0.f;
			}

			ui::Box::Draw(x, y- ui::style::text::height*.1,w,ui::style::text::height*.8);
			y += lead;
		}


		y = yPos;
		ui::Text::Setup();
		for (int i = 0; i < cFunc; i++)
		{
			ui::Text::Draw(callList[i].funcName, x + ui::style::text::width * .1, y);
			y += lead;
		}

		gapi.setScissors(0, 0, 1, 1);
	}

}
