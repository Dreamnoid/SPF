#include "Images.h"

//#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include "stb_image.h"

Images mImages;

ResourceIndex Images::LoadImage(unsigned char* buffer, int length)
{
	int w, h, bpp;
	stbi_uc* pixels = stbi_load_from_memory(buffer, length, &w, &h, &bpp, 4);

	ResourceIndex imgID = 0;
	for (ResourceIndex texID = 0; imgID < ImagesCount; ++imgID)
	{
		if (!mImages[imgID].InUse)
		{
			mImages[imgID].Width = w;
			mImages[imgID].Height = h;
			mImages[imgID].InUse = 1;
			mImages[imgID].Pixels = pixels;
			return imgID;
		}
	}
	FatalError("All image slots are used");
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

extern "C"
{
	DLLExport ResourceIndex LoadImage(unsigned char* buffer, int length)
	{
		return mImages.LoadImage(buffer, length);
	}

	DLLExport void DeleteImage(ResourceIndex image)
	{
		mImages.DeleteImage(image);
	}

	DLLExport int GetImageWidth(ResourceIndex image)
	{
		return mImages.GetImageWidth(image);
	}

	DLLExport int GetImageHeight(ResourceIndex image)
	{
		return mImages.GetImageHeight(image);
	}

	DLLExport unsigned int GetImagePixel(ResourceIndex image, int x, int y)
	{
		return mImages.GetImagePixel(image, x, y);
	}
}