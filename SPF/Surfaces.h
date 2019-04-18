#pragma once
#include "Core.h"

struct Surface
{
	bool InUse;
	HardwareID GLID;
	HardwareID DepthGLID;
	ResourceIndex Texture;
	bool HasDepth;
};

constexpr uint32_t SurfacesCount = 20;

class Surfaces
{
private:
	Surface mSurfaces[SurfacesCount];

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