namespace style {

	namespace text {

		float r, g, b, a;
		float width;
		float height;
		float tracking;
	};

	namespace box {

		float r, g, b, a;
		float width;
		float height;
		float rounded;
		float soft;
		float edge;
		float outlineBrightness;

	};

	void Base()
	{
		text::r = .9f;
		text::g = .9f;
		text::b = .9f;
		text::a = 1.f;
		text::width = .025f*1.5;
		text::height = .035f*1.5;
		text::tracking = 5.f;

		box::r = .2f;
		box::g = .2f;
		box::b = .2f;
		box::a = 1.f;
		box::width = .12f;
		box::height = text::height*.8f;
		box::rounded = .21f;
		box::soft = 5.1f;
		box::edge = 2.f;
		box::outlineBrightness = 0.1f;
	}
};