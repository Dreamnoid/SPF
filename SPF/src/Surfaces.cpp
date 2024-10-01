#include <Surfaces.h>
#include "GL4.h"
#include <cstdio>
#include <vector>
#include <Textures.h>
#include "Resources.h"

namespace SPF
{
	namespace Surfaces
	{
		ResourceIndex Create(int w, int h, SurfaceFlags flags)
		{
			GLuint ids[1];
			glGenFramebuffers(1, ids);
			GLuint fboID = ids[0];

			glBindFramebuffer(GL_FRAMEBUFFER, fboID);

			ResourceIndex colorTexture = InvalidResource;
			ResourceIndex depthTexture = InvalidResource;

			if (HasFlag(flags, SurfaceFlags::Color))
			{
				colorTexture = Textures::Create(w, h, nullptr, TextureFlags::Flipped);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Resources.Textures[colorTexture].GLID, 0);
			}

			if (HasFlag(flags, SurfaceFlags::Depth))
			{
				if (HasFlag(flags, SurfaceFlags::Stencil))
				{
					depthTexture = Textures::Create(w, h, nullptr, TextureFlags::Flipped | TextureFlags::Depth | TextureFlags::Stencil);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, Resources.Textures[depthTexture].GLID, 0);
				}
				else
				{
					depthTexture = Textures::Create(w, h, nullptr, TextureFlags::Flipped | TextureFlags::Depth);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Resources.Textures[depthTexture].GLID, 0);
				}
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			return CreateResource(Resources.Surfaces, { true, fboID, colorTexture, depthTexture, flags });
		}

		void Delete(ResourceIndex surface)
		{
			GLuint ids[1];
			ids[0] = Resources.Surfaces[surface].GLID;
			glDeleteFramebuffers(1, ids);
			if (Resources.Surfaces[surface].DepthTexture > InvalidResource)
			{
				Textures::Delete(Resources.Surfaces[surface].DepthTexture);
			}
			if (Resources.Surfaces[surface].ColorTexture > InvalidResource)
			{
				Textures::Delete(Resources.Surfaces[surface].ColorTexture);
			}
			DeleteResource(Resources.Surfaces, surface);
		}

		void Clear(ResourceIndex surface)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, Resources.Surfaces[surface].GLID);
			GLbitfield mask = GL_NONE;
			if (HasFlag(Resources.Surfaces[surface].Flags, SurfaceFlags::Color))
			{
				mask |= GL_COLOR_BUFFER_BIT;
			}
			if (HasFlag(Resources.Surfaces[surface].Flags, SurfaceFlags::Depth))
			{
				mask |= GL_DEPTH_BUFFER_BIT;
			}
			if (HasFlag(Resources.Surfaces[surface].Flags, SurfaceFlags::Stencil))
			{
				mask |= GL_STENCIL_BUFFER_BIT;
			}
			glClear(mask);
		}

		ResourceIndex GetTexture(ResourceIndex surface)
		{
			return Resources.Surfaces[surface].ColorTexture;
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
		return SPF::Surfaces::Create(w, h, depth ? SPF::SurfaceFlags::All : SPF::SurfaceFlags::Color);
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