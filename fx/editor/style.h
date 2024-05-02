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
		box::r = .2f;
		box::g = .2f;
		box::b = .2f;
		box::a = 1.f;
		box::width = .25f;
		box::height = .25f;
		box::rounded = .21f;
		box::soft = 5.1f;
		box::edge = 1.f;
		box::outlineBrightness = 0.f;

		text::r = .9f;
		text::g = .9f;
		text::b = .9f;
		text::a = 1.f;
		text::width = .035;
		text::height = .035;
		text::tracking = 0.1;
	}
};