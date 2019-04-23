#include "Surfaces.h"
#include "gl4.h"
#include <cstdio>
#include "Textures.h"

Surfaces mSurfaces;

ResourceIndex Surfaces::Create(int w, int h, bool depth)
{
	ResourceIndex texture = mTextures.Create(w, h, nullptr, true);

	GLuint ids[1];
	glGenFramebuffers(1, ids);
	GLuint fboID = ids[0];

	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextures.Get(texture).GLID, 0);

	GLuint depthID;
	if (depth)
	{
		glGenRenderbuffers(1, ids);
		depthID = ids[0];
		glBindRenderbuffer(GL_RENDERBUFFER, depthID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthID);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	for (ResourceIndex surfaceID = 0; surfaceID < SurfacesCount; ++surfaceID)
	{
		if (!mSurfaces[surfaceID].InUse)
		{
			mSurfaces[surfaceID].GLID = fboID;
			mSurfaces[surfaceID].Texture = texture;
			mSurfaces[surfaceID].InUse = true;
			mSurfaces[surfaceID].HasDepth = depth;
			mSurfaces[surfaceID].DepthGLID = depthID;
			return surfaceID;
		}
	}
	FatalError("All surface slots are used");
}

void Surfaces::Delete(ResourceIndex surface)
{
	mSurfaces[surface].InUse = false;
	GLuint ids[1];
	if (mSurfaces[surface].HasDepth)
	{
		ids[0] = mSurfaces[surface].DepthGLID;
		glDeleteRenderBuffers(1, ids);
	}
	ids[0] = mSurfaces[surface].GLID;
	glDeleteFramebuffers(1, ids);
	mTextures.Delete(mSurfaces[surface].Texture);
}

ResourceIndex Surfaces::GetTexture(ResourceIndex surface)
{
	return mSurfaces[surface].Texture;
}

void Surfaces::Clear(ResourceIndex surface)
{
	glBindFramebuffer(GL_FRAMEBUFFER, mSurfaces[surface].GLID);
	if (mSurfaces[surface].HasDepth)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

extern "C"
{

	DLLExport ResourceIndex CreateSurface(int w, int h, int depth)
	{
		return mSurfaces.Create(w, h, depth);
	}

	DLLExport void ClearSurface(ResourceIndex surface)
	{
		mSurfaces.Clear(surface);
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