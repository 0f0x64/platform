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
		float progress_x = 0;
		float progress_y = 0;
		float progress_radial = 0;
		float signed_progress = 0;

	};

	enum class align_h { left, right, center };
	enum class align_v { top, bottom, center };
	

	namespace button {

		align_h hAlign = align_h::left;
		align_v vAlign = align_v::center;
		bool zoom = false;
		bool inverted = false;
		float inner = .1;

	};

	void BaseButton()
	{
		button::hAlign = align_h::left;
		button::vAlign = align_v::center;
		button::zoom = false;
		button::inverted = false;
		button::inner = .1;
	}

	void Base(bool inverted = false)
	{
		float white = .8f;
		float black = .2f;

		text::r = text::g = text::b = inverted ? black : white;
		text::a = 1.f;
		text::width = .025f*1.;
		text::height = .035f*1.;
		text::tracking = 5.f;

		box::r = box::g = box::b = inverted ? white/1.5: black;
		box::a = 1.f;
		box::width = .12f;
		box::height = text::height*.8f;
		box::rounded = .1f;
		box::soft = 5.f;
		box::edge = 15.f;
		box::outlineBrightness = 0.1f;
		box::progress_x = 0;
		box::progress_y = 0;
		box::progress_radial = 0;
		box::signed_progress = 0;
	}

	void BaseColor(bool inverted = false)
	{
		float white = .8f;
		float black = .2f;

		text::r = text::g = text::b = inverted ? black : white;
		text::a = 1.f;
		box::r = box::g = box::b = inverted ? white / 1.5 : black;
		box::a = 1.f;
	}
};