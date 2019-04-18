#pragma once
#include "Core.h"
#undef LoadImage

struct Image
{
	bool InUse;
	void* Pixels;
	unsigned int Width;
	unsigned int Height;
};

constexpr uint32_t ImagesCount = 20;

class Images
{
private:
	Image mImages[ImagesCount];

public:
	ResourceIndex LoadImage(unsigned char* buffer, int length);
	void DeleteImage(ResourceIndex image);
	int GetImageWidth(ResourceIndex image);
	int GetImageHeight(ResourceIndex image);
	unsigned int GetImagePixel(ResourceIndex image, int x, int y);

};

extern Images mImages;