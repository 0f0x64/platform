namespace paramEdit {

	void ShowStack()
	{
		ui::text::Setup();
		float x = 0, y = 0;
		for (int i = 0; i < cFunc; i++)
		{
			ui::text::Draw(callList[i].funcName, x, y);
			y += ui::style::text::height;
		}
	}

}
