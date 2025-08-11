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

			return CreateResource(Resources.Surfaces, { true, fboID, colorTexture, depthTexture, flags, 0 });
		}

		void Attach(ResourceIndex surface, ResourceIndex texture)
		{
			Surface& surfaceData = Resources.Surfaces[surface];
			HardwareID textureID = Resources.Textures[texture].GLID;
			glBindTexture(GL_TEXTURE_2D, textureID);
			glBindFramebuffer(GL_FRAMEBUFFER, surfaceData.GLID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 + surfaceData.AdditionalColorTextures++, GL_TEXTURE_2D, textureID, 0);

			int colorAttachmentsCount = 1 + surfaceData.AdditionalColorTextures;
			static unsigned int attachments[32]; // 32 is the max offered by OpenGL
			for (int i = 0; i < colorAttachmentsCount; ++i)
			{
				attachments[i] = GL_COLOR_ATTACHMENT0 + i;
			}
			glDrawBuffers(colorAttachmentsCount, attachments);
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

		void CopyDepth(ResourceIndex target, ResourceIndex source)
		{
			const SPF::Surface& src = Resources.Surfaces[source];
			const SPF::Surface& tgt = Resources.Surfaces[target];
			const SPF::Texture& srcTex = Resources.Textures[src.DepthTexture];
			const SPF::Texture& tgtTex = Resources.Textures[tgt.DepthTexture];
			glBindFramebuffer(GL_READ_FRAMEBUFFER, src.GLID);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, tgt.GLID);
			glBlitFramebuffer(
				0, 0, srcTex.Width, srcTex.Height,
				0, 0, tgtTex.Width, tgtTex.Height,
				GL_DEPTH_BUFFER_BIT, GL_NEAREST
			);
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

	DLLExport void SPF_CopySurfaceDepth(int target, int source)
	{
		SPF::Surfaces::CopyDepth(target, source);
	}

	DLLExport void SPF_AttachToSurface(int surface, int texture)
	{
		SPF::Surfaces::Attach(surface, texture);
	}
}