#pragma once
#include "Core.h"
#include <vector>
#undef LoadImage

struct Image
{
	bool InUse;
	void* Pixels;
	unsigned int Width;
	unsigned int Height;
};

class Images
{
private:
	std::vector<Image> mImages;

public:
	Images() : mImages(20) {}
	ResourceIndex LoadImage(unsigned char* buffer, int length);
	void DeleteImage(ResourceIndex image);
	int GetImageWidth(ResourceIndex image);
	int GetImageHeight(ResourceIndex image);
	unsigned int GetImagePixel(ResourceIndex image, int x, int y);

};

extern Images mImages;