namespace ui 
{
	using namespace dx11;

	#include <stdio.h>

	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

	unsigned char* m_targaData;

	bool LoadTarga(const char* filename, int& height, int& width)
	{
		int error, bpp, imageSize, index, i, j, k;
		FILE* filePtr;
		unsigned int count;
		TargaHeader targaFileHeader;
		unsigned char* targaImage;


		// Open the targa file for reading in binary.
		error = fopen_s(&filePtr, filename, "rb");
		if (error != 0)
		{
			return false;
		}

		// Read in the file header.
		count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
		if (count != 1)
		{
			return false;
		}

		// Get the important information from the header.
		height = (int)targaFileHeader.height;
		width = (int)targaFileHeader.width;
		bpp = (int)targaFileHeader.bpp;

		// Check that it is 32 bit and not 24 bit.
		if (bpp != 32)
		{
			return false;
		}

		// Calculate the size of the 32 bit image data.
		imageSize = width * height * 4;

		// Allocate memory for the targa image data.
		targaImage = new unsigned char[imageSize];
		if (!targaImage)
		{
			return false;
		}

		// Read in the targa image data.
		count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
		if (count != imageSize)
		{
			return false;
		}

		// Close the file.
		error = fclose(filePtr);
		if (error != 0)
		{
			return false;
		}

		// Allocate memory for the targa destination data.
		m_targaData = new unsigned char[imageSize];
		if (!m_targaData)
		{
			return false;
		}

		// Initialize the index into the targa destination data array.
		index = 0;

		// Initialize the index into the targa image data.
		k = (width * height * 4) - (width * 4);

		// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
		for (j = 0; j < height; j++)
		{
			for (i = 0; i < width; i++)
			{
				*(unsigned char*)(m_targaData + index + 0) = targaImage[k + 2];  // Red.
				*(unsigned char*)(m_targaData + index + 1) = targaImage[k + 1];  // Green.
				*(unsigned char*)(m_targaData + index + 2) = targaImage[k + 0];  // Blue
				*(unsigned char*)(m_targaData + index + 3) = 1;// targaImage[k + 3];  // Alpha

				// Increment the indexes into the targa data.
				k += 4;
				index += 4;
			}

			// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
			k -= (width * 8);
		}

		// Release the targa image data now that it was copied into the destination array.
		delete[] targaImage;
		targaImage = 0;

		return true;
	}

	char* image;
	float kerningTable[255];

	void LoadTGA()
	{
		int th, tw;
		bool Loaded = false;

		Loaded = LoadTarga("..\\ui_font\\font.tga", th, tw);
		int sizeX = tw;
		int sizeY = th;

		int rowPitch = (tw * 4) * sizeof(unsigned char);

		dx11::context->UpdateSubresource(dx11::Textures::texture[tex::font].pTexture, 0, NULL, m_targaData, rowPitch, 0);

	}


	void CalcKerning()
	{
		int fontW = 32;
		int fontH = 32;
		int letters = 96;

		for (int letter = 0; letter < letters; letter++)
		{
			int yOfs = letter * fontH;
			int w = fontW - 1;

			int rowMax = 0;
			for (int y = 0; y < fontH; y++)
			{
				int lineMax = 0;
				for (int x = w; x >= 0; x--)
				{
					unsigned char pixel = *(unsigned char*)(m_targaData + ((yOfs + y) * fontW + x) * 4);
					if (pixel > 0) lineMax = max(lineMax, x);
				}
				rowMax = max(rowMax, lineMax);
			}

			kerningTable[letter] = float(rowMax) / float(fontW);

		}

		kerningTable[0] = 0.5f / float(fontW);

	}
	int pCounter = 0;

	void SetPoint(float x, float y, float z = 0)
	{
		ConstBuf::float4array[pCounter].x = x;
		ConstBuf::float4array[pCounter].y = y;
		ConstBuf::float4array[pCounter].z = z;
		ConstBuf::float4array[pCounter].w = 1;
		pCounter++;

	}

	void SetBox(float x, float y, float w, float h)
	{
		ConstBuf::float4array[pCounter].x = x;
		ConstBuf::float4array[pCounter].y = y;
		ConstBuf::float4array[pCounter].z = w;
		ConstBuf::float4array[pCounter].w = h;
		pCounter++;

	}

	void ShowLine()
	{
		api.setIA(topology::lineList);
		api.blend(blendmode::off);
		api.cull(cullmode::off);
		api.depth(depthmode::off);

		pCounter = 0;
		for (int i = 0; i < 128; i++)
		{
			float x = ((i / 128.f) - .5f) * 2;
			SetPoint(x, 0);
			SetPoint(x, 0.1);
		}
		ConstBuf::Update(6, ConstBuf::float4array);

		vs::lineDrawer.set();
		ps::colorFill.params.r = 1;
		ps::colorFill.params.g = 1;
		ps::colorFill.params.b = 1;
		ps::colorFill.params.a = 1;
		ps::colorFill.set();
		api.drawLine(128, 1);

	}



	namespace style {

		namespace text {

			float r, g, b, a;
			float width;
			float height;
			float tracking;
			bool  forceWidth;
			float fWidthValue;
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

		namespace button {

			float inside;
		};

		void Base()
		{
			box::r = .2f;
			box::g = .2f;
			box::b = .2f;
			box::a = 1.f;
			box::width = .25f;
			box::height = .25f;
			box::rounded = .25f;
			box::soft = 100.f;
			box::edge = 10.f;
			box::outlineBrightness = 0.f;

			text::r = .9f;
			text::g = .9f;
			text::b = .9f;
			text::a = 1.f;
			text::width = .25;
			text::height = .25;
			text::tracking = 0.5;
			text::forceWidth = false;
			text::fWidthValue = kerningTable[0] * text::width;

			button::inside = .9;
		}
	};

	void ShowBox(float x, float y, float w = style::box::width, float h = style::box::height)
	{
		api.setIA(topology::triList);
		api.blend(blendmode::alpha, blendop::add);
		api.cull(cullmode::off);
		api.depth(depthmode::off);

		using namespace style;

		pCounter = 0;
		SetBox(x, y, w, h);

		ConstBuf::Update(6, ConstBuf::float4array);
		ConstBuf::SetToVertex(6);

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

	void ShowText(const char* str, float x, float y, float w = style::text::width, float h = style::text::height)
	{
		api.setIA(topology::triList);
		api.blend(blendmode::alpha, blendop::add);
		api.cull(cullmode::off);
		api.depth(depthmode::off);

		using namespace style;

		vs::letter.params.width = w * aspect;
		vs::letter.params.height = h;

		pCounter = 0;

		float offset = 0;

		for (unsigned int i = 0; i < strlen(str); i++)
		{
			SetBox(x + offset, y, (float)(str[i] - 32), 0);
			offset += getLetterOffset(str[i]);
		}

		ConstBuf::Update(6, ConstBuf::float4array);
		ConstBuf::SetToVertex(6);



		vs::letter.set();

		ps::letter_ps.samplers.s1Filter = filter::linear;
		ps::letter_ps.samplers.s1AddressU = addr::clamp;
		ps::letter_ps.samplers.s1AddressV = addr::clamp;

		ps::letter_ps.textures.tex = tex::font;

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
		float ofs = kerningTable[0] * 2.;
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
		LoadTGA();
		CalcKerning();
	}

	void Draw()
	{
		ShowButtonWithText("bla Bla-bla", 0, 0, .5, .5);
	}

}
