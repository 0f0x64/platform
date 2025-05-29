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
		float progress = 0;
		float signed_progress = 0;
		float slider_type = 0;
	};

	enum class align_h { left, right, center };
	enum class align_v { top, bottom, center };
	

	namespace button {

		align_h hAlign = align_h::left;
		align_v vAlign = align_v::center;
		bool zoom = false;
		bool inverted = false;
		bool selected = false;
		float inner = .1;

	};

	void BaseButton()
	{
		button::hAlign = align_h::left;
		button::vAlign = align_v::center;
		button::zoom = false;
		button::inverted = false;
		button::selected = false;
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

		box::r = box::g = box::b = inverted ? white/1.5f: black;
		box::a = 1.f;
		box::width = .12f;
		box::height = text::height*.8f;
		box::rounded = .1f;
		box::soft = 5.f;
		box::edge = 15.f;
		box::outlineBrightness = 0.1f;
		box::progress = 0;
		box::signed_progress = 0;
		box::slider_type = 0;
	}

	void BaseColor(bool inverted = false, bool selected = false)
	{
		float white = .8f;
		float black = .2f;

		text::r = text::g = text::b = inverted ? black : white;
		text::a = 1.f;
		box::r = box::g = box::b = inverted ? white / 1.5f : black;
		box::a = 1.f;

		if (selected)
		{
			box::r = box::g = box::b = black;
		}
	}
};