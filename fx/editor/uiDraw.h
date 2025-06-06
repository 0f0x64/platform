namespace ui 
{
	bool dblClk = false;
	int cursorID = 0;
	bool lbDown = false;
	bool rbDown = false;
	bool mbDown = false;
	bool LeftDown = false;
	bool RightDown = false;
	
	typedef struct {
		float x;
		float y;
	} point2df;

	typedef struct {
		float x;
		float y;
		float z;
	} point3df;

	point2df mousePos;
	point2df mouseLastPos;
	point2df mouseDelta;
	float mouseAngle;
	float mouseLastAngle;
	float mouseAngleDelta;

	point2df GetCusorPos()
	{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(hWnd, &p);

		RECT r;
		GetClientRect(hWnd, &r);

		editor::ui::point2df pos;
		pos.x = p.x/(float)(r.right - r.left);
		pos.y = p.y/(float)(r.bottom - r.top);

		return pos;
	}

	void SetCusorPos(point2df pos)
	{
		POINT p;
		RECT r;
		GetClientRect(hWnd, &r);
		p.x = (int)(pos.x * (float)(r.right - r.left));
		p.y = (int)(pos.y * (float)(r.bottom - r.top));
		ClientToScreen(hWnd, &p);
		SetCursorPos(p.x, p.y);
	}

	using namespace dx11;

	const int fontTextureIndex = 254;
	const int float4ArraySize = 255;

	#include "font.h"
	#include "style.h"

	namespace Line
	{
		void LineNullDrawer(int vertexCount, int instances)
		{
			context->DrawInstanced(vertexCount, instances, 0, 0);
		}

		void Setup()
		{
			InputAssembler::IA(topology::lineList);
			Blend::Set(blendmode::alpha);
			Rasterizer::Cull(cullmode::off);
			Depth::Depth(depthmode::off);

		}

		float4 buffer[4000];

		void Draw(int count, float r = 1, float g = 1, float b = 1, float a = 1)
		{
			ps::lineDrawer_ps.params.color = { r,g,b,a };
			ps::lineDrawer_ps.set();

			int pCounter = 0;

			for (int i = 0; i < count; i++)
			{
				vs::lineDrawer.params.position[pCounter] = buffer[i * 2]; pCounter++;
				vs::lineDrawer.params.position[pCounter] = buffer[i * 2 + 1]; pCounter++;

				if (pCounter > float4ArraySize - 2 || i == count - 1)
				{
					vs::lineDrawer.set();
					LineNullDrawer(pCounter, 1);
					pCounter = 0;
				}
			}
		}

		void Draw3d(int count, float r = 1, float g = 1, float b = 1, float a = 1)
		{
			ps::lineDrawerUV_ps.params.color = { r,g,b,a };
			ps::lineDrawerUV_ps.set();

			int pCounter = 0;

			vs::lineDrawer3d.params.model = ConstBuf::camera.view[0];

			for (int i = 0; i < count; i++)
			{
				vs::lineDrawer3d.params.position[pCounter] = buffer[i * 2]; pCounter++;
				vs::lineDrawer3d.params.position[pCounter] = buffer[i * 2 + 1]; pCounter++;

				if (pCounter > float4ArraySize - 2 || i == count - 1)
				{
					vs::lineDrawer3d.set();
					LineNullDrawer(pCounter, 1);
					pCounter = 0;
				}
			}
		}

		void StoreLine(int counter, float x, float y, float x1, float y1)
		{
			ui::Line::buffer[counter * 2] = { x,y,0,1 };
			ui::Line::buffer[counter * 2 + 1] = { x1,y1,0,1 };
		}
	}

	namespace Box
	{

		void Setup()
		{
			InputAssembler::IA(topology::triList);
			Blend::Set(blendmode::alpha, blendop::add);
			Rasterizer::Cull(cullmode::off);
			Depth::Depth(depthmode::off);
		}

		void Draw(float x, float y, float w = style::box::width, float h = style::box::height)
		{
			using namespace style;

			int pCounter = 0;

			vs::box.params = {
				.pos_size = {x, y, w, h}
			};

			vs::box.set();

			ps::box_ps.params._aspect = aspect;
			ps::box_ps.params.rad = box::rounded;
			ps::box_ps.params.r = box::r;
			ps::box_ps.params.g = box::g;
			ps::box_ps.params.b = box::b;
			ps::box_ps.params.a = box::a;
			ps::box_ps.params.soft = box::soft;
			ps::box_ps.params.edge = box::edge;
			ps::box_ps.params.outlineBrightness = box::outlineBrightness;
			ps::box_ps.params.progress = ui::style::box::progress;
			ps::box_ps.params.signed_progress = (float)( ui::style::box::signed_progress ? 1 : 0);
			ps::box_ps.params.slider_type = (float)(ui::style::box::slider_type);

			ps::box_ps.set();

			Draw::NullDrawer(1, 1);
		}

	}

	namespace Text
	{

		float getLetterOffset(char a, float w)
		{
			return w * ( kerningTable[a - 32] + style::text::tracking * kerningTable[0])/2.f;
		}

		float getTextLen(const char* str, float w, int strEnd = -1)
		{
			float offset = 0;

			if (strEnd < 0) strEnd = strlen(str);
			strEnd = min((unsigned int)strEnd, strlen(str));

			for (unsigned int i = 0; i < (unsigned int)strEnd; i++)
			{
				offset += getLetterOffset(str[i], w * aspect);
			}

			return offset;
		}

		void Setup()
		{
			InputAssembler::IA(topology::triList);
			Blend::Set(blendmode::alpha, blendop::add);
			Rasterizer::Cull(cullmode::off);
			Depth::Depth(depthmode::off);

			ps::letter_ps.samplers.s1Filter = filter::linear;
			ps::letter_ps.samplers.s1AddressU = addr::clamp;
			ps::letter_ps.samplers.s1AddressV = addr::clamp;
			ps::letter_ps.textures.tex = (texture)fontTextureIndex;

		}

		void Draw(const char* str, float x, float y, float w = style::text::width, float h = style::text::height, bool centered = false)
		{
			if (strlen(str) == 0)
			{
				return;
			}

			vs::letter.params.width = w * aspect;
			vs::letter.params.height = h;

			float offset = 0.f;

			for (unsigned int i = 0; i < strlen(str); i++)
			{
				vs::letter.params.pos_size[i] = { x + offset, y, (float)(str[i] - 32), 0};
				offset += getLetterOffset(str[i], w * aspect);
			}

			if (centered)
			{
				for (unsigned int i = 0; i < strlen(str); i++)
				{
					vs::letter.params.pos_size[i].x -=offset/2.;
				}
			}

			vs::letter.set();

			ps::letter_ps.params.r = style::text::r;
			ps::letter_ps.params.g = style::text::g;
			ps::letter_ps.params.b = style::text::b;
			ps::letter_ps.params.a = style::text::a;

			ps::letter_ps.set();
			Draw::NullDrawer(1, strlen(str));

		}
	}

	void ShowTextWithButton(const char* str, float x, float y)
	{
		style::Base();
		float ofs = kerningTable[0] * 2.f;
		float boxW = Text::getTextLen(str, ui::style::text::width) + ofs * 2.f;
		Box::Draw(x, y, boxW);
		Text::Draw(str, x + ofs, y);
	}

	void ShowButtonWithText(const char* str, float x, float y, float w, float h)
	{
		style::Base();
		Box::Draw(x, y, w, h);
		float textW = Text::getTextLen(str,w);
		float x1 = x + w / 2.f - textW / 2;
		float y1 = y - h / 2.f + style::text::height / 2;
		Text::Draw(str, x1, y1);
	}

	void CreateFontTexture()
	{
		Textures::Create(fontTextureIndex, Textures::tType::flat, Textures::tFormat::u8, XMFLOAT2(32, 3072), false, false);
	}

	void Init()
	{
		CreateFontTexture();
		LoadFont();
		CalcKerning();
	}

	void Draw()
	{

	}

}

