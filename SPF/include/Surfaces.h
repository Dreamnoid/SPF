#pragma once
#include "Core.h"

namespace SPF
{
	namespace Surfaces
	{
		DLLExport ResourceIndex Create(int w, int h, SurfaceFlags flags);
		DLLExport void Delete(ResourceIndex surface);
		DLLExport ResourceIndex GetTexture(ResourceIndex surface);
		DLLExport ResourceIndex GetDepthTexture(ResourceIndex surface);
		DLLExport void Clear(ResourceIndex surface);
	};
}