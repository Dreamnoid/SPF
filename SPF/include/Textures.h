#pragma once
#include "Core.h"
#include <vector>

namespace SPF
{
	struct Texture
	{
		bool InUse;
		HardwareID GLID;
		unsigned int Width;
		unsigned int Height;
		bool Flipped;
	};

	class Textures
	{
	private:
		std::vector<Texture> mTextures;

	public:
		Textures() : mTextures(200) {}
		ResourceIndex Create(unsigned int w, unsigned int h, void* pixels, bool flipped);
		ResourceIndex Load(unsigned char* buffer, int length);
		void Delete(ResourceIndex texture);
		int GetWidth(ResourceIndex texture);
		int GetHeight(ResourceIndex texture);
		void SetFiltering(ResourceIndex texture, bool filtering);

		inline const Texture& Get(ResourceIndex texture) const
		{
			return mTextures[texture];
		}
	};

	extern Textures mTextures;
}