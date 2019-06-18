#include <Surfaces.h>
#include "gl4.h"
#include <cstdio>
#include <vector>
#include <Textures.h>
#include "Resources.h"

namespace SPF
{
	namespace Surfaces
	{
		ResourceIndex Create(int w, int h, bool depth)
		{
			ResourceIndex texture = Textures::Create(w, h, nullptr, true);

			GLuint ids[1];
			glGenFramebuffers(1, ids);
			GLuint fboID = ids[0];

			glBindFramebuffer(GL_FRAMEBUFFER, fboID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Resources.Textures[texture].GLID, 0);

			GLuint depthID = 0;
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

			for (ResourceIndex surfaceID = 0; surfaceID < Resources.Surfaces.size(); ++surfaceID)
			{
				if (!Resources.Surfaces[surfaceID].InUse)
				{
					Resources.Surfaces[surfaceID] = { true,fboID,depthID,texture,depth };
					return surfaceID;
				}
			}
			Resources.Surfaces.push_back({ true,fboID,depthID,texture,depth });
			return Resources.Surfaces.size() - 1;
		}

		void Delete(ResourceIndex surface)
		{
			Resources.Surfaces[surface].InUse = false;
			GLuint ids[1];
			if (Resources.Surfaces[surface].HasDepth)
			{
				ids[0] = Resources.Surfaces[surface].DepthGLID;
				glDeleteRenderBuffers(1, ids);
			}
			ids[0] = Resources.Surfaces[surface].GLID;
			glDeleteFramebuffers(1, ids);
			Textures::Delete(Resources.Surfaces[surface].Texture);
		}

		void Clear(ResourceIndex surface)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, Resources.Surfaces[surface].GLID);
			if (Resources.Surfaces[surface].HasDepth)
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
			else
			{
				glClear(GL_COLOR_BUFFER_BIT);
			}
		}

		ResourceIndex GetTexture(ResourceIndex surface)
		{
			return Resources.Surfaces[surface].Texture;
		}
	}
}

extern "C"
{
	DLLExport int SPF_CreateSurface(int w, int h, int depth)
	{
		return SPF::Surfaces::Create(w, h, depth);
	}

	DLLExport void SPF_ClearSurface(int surface)
	{
		SPF::Surfaces::Clear(surface);
	}

	DLLExport void SPF_DeleteSurface(int surface)
	{
		SPF::Surfaces::Delete(surface);
	}

	DLLExport int SPF_GetSurfaceTexture(int surface)
	{
		return SPF::Surfaces::GetTexture(surface);
	}
}