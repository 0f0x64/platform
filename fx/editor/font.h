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

	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)	return false;

	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)	return false;

	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;
	if (bpp != 32) return false;

	imageSize = width * height * 4;

	targaImage = new unsigned char[imageSize];
	if (!targaImage) return false;

	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize) return false;

	fclose(filePtr);

	m_targaData = new unsigned char[imageSize];
	index = 0;
	k = (width * height * 4) - (width * 4);

	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			*(unsigned char*)(m_targaData + index + 0) = targaImage[k + 2];  // Red.
			*(unsigned char*)(m_targaData + index + 1) = targaImage[k + 1];  // Green.
			*(unsigned char*)(m_targaData + index + 2) = targaImage[k + 0];  // Blue
			*(unsigned char*)(m_targaData + index + 3) = targaImage[k + 3];  // Alpha

			k += 4;
			index += 4;
		}

		k -= (width * 8);
	}

	delete[] targaImage;
	targaImage = 0;

	return true;
}


float kerningTable[255];

void LoadFont()
{
	int th, tw;

	LoadTarga("..\\ui_font\\font.tga", th, tw);

	int rowPitch = (tw * 4) * sizeof(unsigned char);
	context->UpdateSubresource(dx11::Textures::texture[fontTextureIndex].pTexture, 0, NULL, m_targaData, rowPitch, 0);

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
				if (pixel > 0)
				{
					lineMax = x;
					break;
				}
			}
			rowMax = max(rowMax, lineMax);
		}

		kerningTable[letter] = float(rowMax) / float(fontW);

	}

	kerningTable[0] = 0.5f / float(fontW);

}