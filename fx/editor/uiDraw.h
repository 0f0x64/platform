namespace ui 
{
	using namespace dx11;

	ID3D11Buffer* f4arrayBuf;
	#define float4ArraySize 4000
	XMFLOAT4 float4array[float4ArraySize];

	void CreateConstBuf()
	{
		ConstBuf::CreateCB(f4arrayBuf, sizeof(float4array));
	}

	void f4arrayUpdateAndSet()
	{
		context->UpdateSubresource(f4arrayBuf, 0, NULL, float4array, 0, 0);
		context->VSSetConstantBuffers(6, 1, &f4arrayBuf);
	}

	void LineNullDrawer(int vertexCount, int instances)
	{
		ConstBuf::Update(0, ConstBuf::drawerV);
		ConstBuf::SetToVertex(0);
		ConstBuf::Update(1, ConstBuf::drawerP);
		ConstBuf::SetToPixel(1);

		f4arrayUpdateAndSet();

		context->DrawInstanced(vertexCount, instances, 0, 0);
	}

	#include "font.h"
	#include "style.h"

	int pCounter = 0;

	void SetFloat4Const(float x, float y, float w = 0, float h = 0)
	{
		float4array[pCounter].x = x;
		float4array[pCounter].y = y;
		float4array[pCounter].z = w;
		float4array[pCounter].w = h;
		pCounter++;
	}

	void SetupLineDrawer()
	{
		api.setIA(topology::lineList);
		api.blend(blendmode::off);
		api.cull(cullmode::off);
		api.depth(depthmode::off);
	}

	typedef struct {
		float x;
		float y;
		float x1;
		float y1;
	} line2;
	
	line2 lineBuffer[2048];

	void ShowLine(int count, float r = 1, float g = 1, float b = 1, float a = 1)
	{
		ps::colorFill.params.r = r;
		ps::colorFill.params.g = g;
		ps::colorFill.params.b = b;
		ps::colorFill.params.a = a;
		ps::colorFill.set();

		pCounter = 0;

		for (int i = 0; i < count; i++)
		{
			SetFloat4Const(lineBuffer[i].x, lineBuffer[i].y);
			SetFloat4Const(lineBuffer[i].x1, lineBuffer[i].y1);

			if (pCounter > float4ArraySize-2 || i == count - 1)
			{
				vs::lineDrawer.set();
				LineNullDrawer(pCounter, 1);
				pCounter = 0;
			}
		}
	}

	void SetupBoxDrawer()
	{
		api.setIA(topology::triList);
		api.blend(blendmode::alpha, blendop::add);
		api.cull(cullmode::off);
		api.depth(depthmode::off);
	}

	void ShowBox(float x, float y, float w = style::box::width, float h = style::box::height)
	{
		using namespace style;

		pCounter = 0;
		SetFloat4Const(x, y, w, h);

		f4arrayUpdateAndSet();
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

		api.draw(1, pCounter);

	}

	float getLetterOffset(char a)
	{
		return vs::letter.params.width * kerningTable[a - 32] + style::text::tracking * kerningTable[0];
	}

	float getTextLen(const char* str)
	{
		float offset = 0;
		for (unsigned int i = 0; i < strlen(str); i++)
		{
			offset += getLetterOffset(str[i]);
		}
		return offset;
	}

	void SetupTextDrawer()
	{
		api.setIA(topology::triList);
		api.blend(blendmode::alpha, blendop::add);
		api.cull(cullmode::off);
		api.depth(depthmode::off);

		ps::letter_ps.samplers.s1Filter = filter::linear;
		ps::letter_ps.samplers.s1AddressU = addr::clamp;
		ps::letter_ps.samplers.s1AddressV = addr::clamp;
		ps::letter_ps.textures.tex = tex::font;

	}

	void ShowText(const char* str, float x, float y, float w = style::text::width, float h = style::text::height)
	{
		using namespace style;

		vs::letter.params.width = w * aspect;
		vs::letter.params.height = h;

		pCounter = 0;

		float offset = 0;

		for (unsigned int i = 0; i < strlen(str); i++)
		{
			SetFloat4Const(x + offset, y, (float)(str[i] - 32), 0);
			offset += getLetterOffset(str[i])/2.f;
		}

		f4arrayUpdateAndSet();

		vs::letter.set();

		ps::letter_ps.params.r = text::r;
		ps::letter_ps.params.g = text::g;
		ps::letter_ps.params.b = text::b;
		ps::letter_ps.params.a = text::a;

		ps::letter_ps.set();

		api.draw(1, strlen(str));

	}

	void ShowTextWithButton(const char* str, float x, float y)
	{
		style::Base();
		float ofs = kerningTable[0] * 2.f;
		float boxW = getTextLen(str) + ofs * 2.f;
		ShowBox(x, y, boxW);
		ShowText(str, x + ofs, y);
	}

	void ShowButtonWithText(const char* str, float x, float y, float w, float h)
	{
		style::Base();
		ShowBox(x, y, w, h);
		float textW = getTextLen(str);
		float x1 = x + w / 2.f - textW / 2;
		float y1 = y - h / 2.f + style::text::height / 2;
		ShowText(str, x1, y1);
	}

	void Init()
	{
		LoadFont();
		CalcKerning();
		CreateConstBuf();
	}

	void Draw()
	{

	}

}
