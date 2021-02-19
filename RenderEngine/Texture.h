#pragma once

#include <vector>
#include "Vector.h"
#include "Colour.h"


namespace RenderEngine {


	class Texture
	{
	public:
		Texture();
		explicit Texture(const char *filename);
		~Texture();
		void LoadColourTexture(const char *filename);
		void CreateEmptyTexture(int w, int h);
		void ClearTextureColour();
		void ClearTextureDepth();
		int GetWidth() const { return width; }
		int GetHeight() const { return height; }
		void SetColour(const Vector3& t, const Colour& c);
		Colour GetColour(const Vector3& t);
		void SetDepth(int x, int y, float z) { depth[x][y] = z; }
		float GetDepth(int x, int y) { return depth[x][y]; }
		inline unsigned int GetHEXColor(Colour color) {
			unsigned int r = (unsigned int)(color.r * 255.f);
			unsigned int g = (unsigned int)(color.g * 255.f);
			unsigned int b = (unsigned int)(color.b * 255.f);
			unsigned int hexColor = (r << 16) | (g << 8) | (b);
			return hexColor;
		}

	private:
		int width;
		int height;
		UINT32 **texColor = nullptr;
		float **depth = nullptr;
	};

	class TextureArray
	{
	public:
		TextureArray();
		~TextureArray();

		void LoadTexture(const char *filename);
		void LoadEmptyTexture();
		Colour GetColour(const Vector3& t, int index);
		size_t GetSize() const { return textures.size(); }
		auto GetTexture(int index) const { return textures[index]; }
		std::vector<std::shared_ptr<Texture>>& GetArrayRef() { return textures; }
		

	private:
		std::vector<std::shared_ptr<Texture>> textures;
	};
}