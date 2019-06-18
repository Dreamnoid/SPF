#include <Images.h>

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include "stb_image.h"

namespace SPF
{
	Images mImages;

	ResourceIndex Images::LoadImage(unsigned char* buffer, int length)
	{
		int w, h, bpp;
		stbi_uc* pixels = stbi_load_from_memory(buffer, length, &w, &h, &bpp, 4);

		for (ResourceIndex imgID = 0; imgID < mImages.size(); ++imgID)
		{
			if (!mImages[imgID].InUse)
			{
				mImages[imgID] = { true,pixels,(unsigned int)w,(unsigned int)h };
				return imgID;
			}
		}
		mImages.push_back({ true, pixels,(unsigned int)w,(unsigned int)h });
		return mImages.size() - 1;
	}

	void Images::DeleteImage(ResourceIndex image)
	{
		stbi_image_free(mImages[image].Pixels);
		mImages[image].InUse = 0;
	}

	int Images::GetImageWidth(ResourceIndex image)
	{
		return mImages[image].Width;
	}

	int Images::GetImageHeight(ResourceIndex image)
	{
		return mImages[image].Height;
	}

	unsigned int Images::GetImagePixel(ResourceIndex image, int x, int y)
	{
		int index = (y * mImages[image].Width + x) * 4;
		stbi_uc* pixels = (stbi_uc*)mImages[image].Pixels;
		unsigned int r = pixels[index + 0];
		unsigned int g = pixels[index + 1];
		unsigned int b = pixels[index + 2];
		unsigned int a = pixels[index + 3];
		return (r << 24) | (g << 16) | (b << 8) | a;
	}
}

extern "C"
{
	DLLExport SPF::ResourceIndex LoadImage(unsigned char* buffer, int length)
	{
		return SPF::mImages.LoadImage(buffer, length);
	}

	DLLExport void DeleteImage(SPF::ResourceIndex image)
	{
		SPF::mImages.DeleteImage(image);
	}

	DLLExport int GetImageWidth(SPF::ResourceIndex image)
	{
		return SPF::mImages.GetImageWidth(image);
	}

	DLLExport int GetImageHeight(SPF::ResourceIndex image)
	{
		return SPF::mImages.GetImageHeight(image);
	}

	DLLExport unsigned int GetImagePixel(SPF::ResourceIndex image, int x, int y)
	{
		return SPF::mImages.GetImagePixel(image, x, y);
	}
}