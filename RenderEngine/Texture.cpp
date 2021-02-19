#include "Texture.h"
#include <windows.h>
#include <assert.h>

namespace RenderEngine {

#define SAFE_DELETE(p) if(p) delete p; p=nullptr;

	Texture::Texture()
	{
		CreateEmptyTexture(1024, 1024);
	}

	Texture::Texture(const char *filename)
	{
		LoadColourTexture(filename);
	}

	Texture::~Texture()
	{
		SAFE_DELETE(texColor);
		depth = nullptr;
	}

	void Texture::LoadColourTexture(const char *filename)
	{
		FILE *bmpFile = fopen(filename, "rb");

		if (bmpFile == nullptr)
		{
			std::cerr << "Open texture file fail" << std::endl;
			return;
		}

		BITMAPFILEHEADER fileHeader;
		fread(&fileHeader, sizeof(fileHeader), 1, bmpFile);

		if (fileHeader.bfType != 0x4D42)
		{
			fclose(bmpFile);
			std::cerr << "Texture file is not bmp file" << std::endl;
			return;
		}

		BITMAPINFOHEADER infoHeader;
		fread(&infoHeader, sizeof(infoHeader), 1, bmpFile);

		fseek(bmpFile, fileHeader.bfOffBits, SEEK_SET);

		width = infoHeader.biWidth;
		height = infoHeader.biHeight;
		int need = width * height * 4 + sizeof(void*)*(height);
		char *ptr = (char*)malloc(need);
		assert(ptr);
		char *tex;

		texColor = (UINT32 **)ptr;
		// The line number index of texture
		ptr += sizeof(void*) * height;

		tex = (char*)ptr;
		ptr += width * height * 4;

		for (int j = 0; j < height; ++j)
		{
			texColor[j] = (UINT32 *)(tex + width * 4 * j);
		}

		unsigned char B;
		unsigned char G;
		unsigned char R;
		unsigned char A = 255;

		for (int j = 0; j < height; ++j)
		{
			for (int i = 0; i < width; ++i)
			{
				fread(&B, 1, 1, bmpFile);
				fread(&G, 1, 1, bmpFile);
				fread(&R, 1, 1, bmpFile);
				if (infoHeader.biBitCount & 32)
					fread(&A, 1, 1, bmpFile);
				texColor[j][i] = (A << 24) | (R << 16) | (G << 8) | B;
			}
		}
		fclose(bmpFile);
	}

	void Texture::CreateEmptyTexture(int w, int h)
	{
		width = w;
		height = h;
		int need = width * height * 8 + sizeof(void*)*(height * 2);
		char *ptr = (char*)malloc(need);
		assert(ptr);
		char *tex, *dep;

		texColor = (UINT32 **)ptr;
		ptr += sizeof(void*) * height;
		tex = (char*)ptr;
		ptr += width * height * 4;

		for (int j = 0; j < height; ++j)
		{
			texColor[j] = (UINT32 *)(tex + width * 4 * j);
		}

		depth = (float **)(ptr);
		ptr += sizeof(void*) * height;
		dep = (char*)ptr;
		ptr += width * height * 4;
		for (int j = 0; j < height; ++j)
		{
			depth[j] = (float *)(dep + width * 4 * j);
		}

		ClearTextureColour();
		ClearTextureDepth();
	}

	void Texture::ClearTextureColour()
	{
		int buffer_size = width * height * 4;
		char* ptr = (char*)texColor;
		ptr += sizeof(void*) * height;
		memset(ptr, 0xff, buffer_size);

		/*
		for (int j = 0; j < height; ++j)
		{
			for (int i = 0; i < width; ++i)
			{
				Colour[j][i] = 0xffffffff;
			}
		}
		*/
	}

	void Texture::ClearTextureDepth()
	{
		for (int j = 0; j < height; ++j)
		{
			for (int i = 0; i < width; ++i)
			{
				depth[j][i] = 1.0f;
			}
		}
	}

	void Texture::SetColour(const Vector3& t, const Colour& c)
	{
		int x, y;
		float u = t.getX() * ((float)width - 1.0f);
		float v = t.getY() * ((float)height - 1.0f);
		x = (int)(u + 0.5f);
		y = (int)(v + 0.5f);
		// if crash, decomment this two line code
		//x = CLAMP(x, 0, width - 1);
		//y = CLAMP(y, 0, height - 1);
		texColor[y][x] = GetHEXColor(c) ;
	}

	Colour Texture::GetColour(const Vector3& t)
	{
		int x, y;
		float u = t.getX() * ((float)width - 1.0f);
		float v = t.getY() * ((float)height - 1.0f);
		x = (int)(u + 0.5f);
		y = (int)(v + 0.5f);
		// if crash, decomment this two line code
		//x = CLAMP(x, 0, width - 1);
		//y = CLAMP(y, 0, height - 1);
		UINT32 c = texColor[y][x];
		float a = ((c >> 24) & 0xff) / 255.0f;
		float r = ((c >> 16) & 0xff) / 255.0f;
		float g = ((c >> 8) & 0xff) / 255.0f;
		float b = (c & 0xff) / 255.0f;
		return Colour(r, g, b);
	}

	TextureArray::TextureArray()
	{

	}

	TextureArray::~TextureArray()
	{

	}

	void TextureArray::LoadTexture(const char *filename)
	{
		auto texure = std::make_shared<Texture>(filename);
		textures.push_back(texure);
	}

	void TextureArray::LoadEmptyTexture()
	{
		auto texure = std::make_shared<Texture>();
		textures.push_back(texure);
	}

	Colour TextureArray::GetColour(const Vector3& t, int index)
	{
		return textures[index]->GetColour(t);
	}

}
