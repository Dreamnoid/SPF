#include "Surfaces.h"
#include "gl4.h"
#include <cstdio>
#include "Textures.h"

Surfaces mSurfaces;

ResourceIndex Surfaces::Create(int w, int h)
{
	ResourceIndex texture = mTextures.Create(w, h, NULL, 1);

	GLuint ids[1];
	glGenFramebuffers(1, ids);
	GLuint fboID = ids[0];
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextures.Get(texture).GLID, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	for (ResourceIndex surfaceID = 0; surfaceID < SurfacesCount; ++surfaceID)
	{
		if (!mSurfaces[surfaceID].InUse)
		{
			mSurfaces[surfaceID].GLID = fboID;
			mSurfaces[surfaceID].Texture = texture;
			mSurfaces[surfaceID].InUse = true;
			return surfaceID;
		}
	}
	FatalError("All surface slots are used");
}

void Surfaces::Delete(ResourceIndex surface)
{
	mSurfaces[surface].InUse = false;
	GLuint ids[1];
	ids[0] = mSurfaces[surface].GLID;
	glDeleteFramebuffers(1, ids);
}

ResourceIndex Surfaces::GetTexture(ResourceIndex surface)
{
	return mSurfaces[surface].Texture;
}

extern "C"
{

	DLLExport ResourceIndex CreateSurface(int w, int h)
	{
		return mSurfaces.Create(w, h);
	}

	DLLExport void DeleteSurface(ResourceIndex surface)
	{
		mSurfaces.Delete(surface);
	}

	DLLExport ResourceIndex GetSurfaceTexture(ResourceIndex surface)
	{
		return mSurfaces.GetTexture(surface);
	}

}