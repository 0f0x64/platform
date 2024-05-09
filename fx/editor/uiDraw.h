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

	namespace ConstBuffer
	{
		ID3D11Buffer* float4arrayBuf;

		XMFLOAT4 float4array[float4ArraySize];

		void Create()
		{
			ConstBuf::Create(float4arrayBuf, sizeof(float4array));
		}

		void f4arrayUpdateAndSet()
		{
			context->UpdateSubresource(float4arrayBuf, 0, NULL, float4array, 0, 0);
			context->VSSetConstantBuffers(6, 1, &float4arrayBuf);
		}

		int pCounter = 0;

		void SetFloat4Const(float x, float y, float z = 0, float w = 1)
		{
			float4array[pCounter].x = x;
			float4array[pCounter].y = y;
			float4array[pCounter].z = z;
			float4array[pCounter].w = w;
			pCounter++;
		}
	}

	namespace Line
	{
		void LineNullDrawer(int vertexCount, int instances)
		{
			ConstBuf::Update(0, ConstBuf::drawerV);
			ConstBuf::ConstToVertex(0);
			ConstBuf::Update(1, ConstBuf::drawerP);
			ConstBuf::ConstToPixel(1);

			ConstBuffer::f4arrayUpdateAndSet();

			context->DrawInstanced(vertexCount, instances, 0, 0);
		}

		void Setup()
		{
			gapi.setIA(topology::lineList);
			gapi.blend(blendmode::alpha);
			gapi.cull(cullmode::off);
			gapi.depth(depthmode::off);
		}

		typedef struct {
			float x;
			float y;
			float z;
			float w;
		} line4;

		line4 buffer[2048];

		void Draw(int count, float r = 1, float g = 1, float b = 1, float a = 1)
		{
			ps::lineDrawer_ps.params.r = r;
			ps::lineDrawer_ps.params.g = g;
			ps::lineDrawer_ps.params.b = b;
			ps::lineDrawer_ps.params.a = a;
			ps::lineDrawer_ps.set();

			ConstBuffer::pCounter = 0;

			for (int i = 0; i < count; i++)
			{
				ConstBuffer::SetFloat4Const(buffer[i*2].x, buffer[i*2].y, buffer[i*2].z);
				ConstBuffer::SetFloat4Const(buffer[i*2+1].x, buffer[i*2+1].y, buffer[i*2+1].z);

				if (ConstBuffer::pCounter > float4ArraySize - 2 || i == count - 1)
				{
					vs::lineDrawer.set();
					LineNullDrawer(ConstBuffer::pCounter, 1);
					ConstBuffer::pCounter = 0;
				}
			}
		}

		void Draw3d(int count, float r = 1, float g = 1, float b = 1, float a = 1)
		{
			ps::lineDrawer_ps.params.r = r;
			ps::lineDrawer_ps.params.g = g;
			ps::lineDrawer_ps.params.b = b;
			ps::lineDrawer_ps.params.a = a;
			ps::lineDrawer_ps.set();

			ConstBuffer::pCounter = 0;

			for (int i = 0; i < count; i++)
			{
				ConstBuffer::SetFloat4Const(buffer[i * 2].x, buffer[i * 2].y, buffer[i * 2].z);
				ConstBuffer::SetFloat4Const(buffer[i * 2 + 1].x, buffer[i * 2 + 1].y, buffer[i * 2 + 1].z);

				if (ConstBuffer::pCounter > float4ArraySize - 2 || i == count - 1)
				{
					vs::lineDrawer3d.set();
					LineNullDrawer(ConstBuffer::pCounter, 1);
					ConstBuffer::pCounter = 0;
				}
			}
		}
	}

	namespace Box
	{

		void Setup()
		{
			gapi.setIA(topology::triList);
			gapi.blend(blendmode::alpha, blendop::add);
			gapi.cull(cullmode::off);
			gapi.depth(depthmode::off);
		}

		void Draw(float x, float y, float w = style::box::width, float h = style::box::height)
		{
			using namespace style;

			ConstBuffer::pCounter = 0;
			ConstBuffer::SetFloat4Const(x, y, w, h);
			ConstBuffer::f4arrayUpdateAndSet();

			vs::box.set();

			ps::box_ps.params.aspect = aspect;
			ps::box_ps.params.rad = box::rounded;
			ps::box_ps.params.r = box::r;
			ps::box_ps.params.g = box::g;
			ps::box_ps.params.b = box::b;
			ps::box_ps.params.a = box::a;
			ps::box_ps.params.soft = box::soft;
			ps::box_ps.params.edge = box::edge;
			ps::box_ps.params.outlineBrightness = box::outlineBrightness;
			ps::box_ps.set();

			gapi.draw(1, 1);

		}

	}

	namespace Text
	{

		float getLetterOffset(char a, float w)
		{
			return w * ( kerningTable[a - 32] + style::text::tracking * kerningTable[0])/2.f;
		}

		float getTextLen(const char* str, float w)
		{
			float offset = 0;
			for (unsigned int i = 0; i < strlen(str); i++)
			{
				offset += getLetterOffset(str[i], w * aspect);
			}
			return offset;
		}

		void Setup()
		{
			gapi.setIA(topology::triList);
			gapi.blend(blendmode::alpha, blendop::add);
			gapi.cull(cullmode::off);
			gapi.depth(depthmode::off);

			ps::letter_ps.samplers.s1Filter = filter::linear;
			ps::letter_ps.samplers.s1AddressU = addr::clamp;
			ps::letter_ps.samplers.s1AddressV = addr::clamp;
			ps::letter_ps.textures.tex = (texture)fontTextureIndex;

		}

		void Draw(const char* str, float x, float y, float w = style::text::width, float h = style::text::height)
		{

			vs::letter.params.width = w * aspect;
			vs::letter.params.height = h;

			ConstBuffer::pCounter = 0;

			float offset = 0.f;

			for (unsigned int i = 0; i < strlen(str); i++)
			{
				ConstBuffer::SetFloat4Const(x + offset, y, (float)(str[i] - 32), 0);
				offset += getLetterOffset(str[i], w * aspect);
			}

			ConstBuffer::f4arrayUpdateAndSet();

			vs::letter.set();

			ps::letter_ps.params.r = style::text::r;
			ps::letter_ps.params.g = style::text::g;
			ps::letter_ps.params.b = style::text::b;
			ps::letter_ps.params.a = style::text::a;

			ps::letter_ps.set();

			gapi.draw(1, strlen(str));

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
		ConstBuffer::Create();
	}

	void Draw()
	{

	}

}

