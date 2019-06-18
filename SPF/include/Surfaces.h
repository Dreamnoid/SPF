#pragma once
#include <vector>
#include "Core.h"

namespace SPF
{
	struct Surface
	{
		bool InUse;
		HardwareID GLID;
		HardwareID DepthGLID;
		ResourceIndex Texture;
		bool HasDepth;
	};

	class Surfaces
	{
	private:
		std::vector<Surface> mSurfaces;

	public:
		ResourceIndex Create(int w, int h, bool depth);
		void Delete(ResourceIndex surface);
		ResourceIndex GetTexture(ResourceIndex surface);
		void Clear(ResourceIndex surface);

		inline const Surface& Get(ResourceIndex surface) const
		{
			return mSurfaces[surface];
		}

	};

	extern Surfaces mSurfaces;
}