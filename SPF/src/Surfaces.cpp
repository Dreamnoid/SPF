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
			ResourceIndex texture = Textures::Create(w, h, nullptr, false, true);

			GLuint ids[1];
			glGenFramebuffers(1, ids);
			GLuint fboID = ids[0];

			glBindFramebuffer(GL_FRAMEBUFFER, fboID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Resources.Textures[texture].GLID, 0);

			ResourceIndex depthTexture = -1;
			if (depth)
			{
				depthTexture = Textures::Create(w, h, nullptr, true, true);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Resources.Textures[depthTexture].GLID, 0);
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			return CreateResource(Resources.Surfaces, { true, fboID, texture, depthTexture, depth });
		}

		void Delete(ResourceIndex surface)
		{
			GLuint ids[1];
			ids[0] = Resources.Surfaces[surface].GLID;
			glDeleteFramebuffers(1, ids);
			if (Resources.Surfaces[surface].HasDepth)
			{
				Textures::Delete(Resources.Surfaces[surface].DepthTexture);
			}
			Textures::Delete(Resources.Surfaces[surface].Texture);
			DeleteResource(Resources.Surfaces, surface);
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

		ResourceIndex GetDepthTexture(ResourceIndex surface)
		{
			return Resources.Surfaces[surface].DepthTexture;
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

	DLLExport int SPF_GetSurfaceDepthTexture(int surface)
	{
		return SPF::Surfaces::GetDepthTexture(surface);
	}
}