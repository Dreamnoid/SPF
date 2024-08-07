#pragma once
#include "Core.h"

namespace SPF
{
	namespace Images
	{
		DLLExport ResourceIndex Load(unsigned char* buffer, int length);
		DLLExport void Save(const char* filename, int width, int height, unsigned char* pixels);
		DLLExport void Delete(ResourceIndex image);
		DLLExport int GetWidth(ResourceIndex image);
		DLLExport int GetHeight(ResourceIndex image);
		DLLExport unsigned int GetPixel(ResourceIndex image, int x, int y);
	};
}