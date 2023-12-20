#pragma once
#include "Core.h"

namespace SPF
{
	namespace Textures
	{
		DLLExport ResourceIndex Create(unsigned int w, unsigned int h, void* pixels, bool depth, bool flipped);
		DLLExport ResourceIndex Load(unsigned char* buffer, int length);
		DLLExport void Delete(ResourceIndex texture);
		DLLExport int GetWidth(ResourceIndex texture);
		DLLExport int GetHeight(ResourceIndex texture);
		DLLExport void SetFiltering(ResourceIndex texture, bool filtering);
		DLLExport bool IsFlipped(ResourceIndex texture);
		DLLExport void GenerateMipmap(ResourceIndex texture);
	}
}