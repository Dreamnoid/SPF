#include <Textures.h>
#include "GL4.h"
#include <cstdio>
#include <vector>
#include "Resources.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include "stb_image.h"

namespace SPF
{
	namespace Textures
	{
		ResourceIndex Create(unsigned int w, unsigned int h, void* pixels, bool depth, bool flipped)
		{
			GLuint ids[1];
			glGenTextures(1, ids);
			GLuint id = ids[0];

			GLint previousId;
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousId);

			glBindTexture(GL_TEXTURE_2D, id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			if (depth)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, w, h, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, pixels);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			}

			glBindTexture(GL_TEXTURE_2D, previousId);

			return CreateResource(Resources.Textures, { true, id, w, h, flipped, depth, false });
		}

		void GenerateMipmap(ResourceIndex texture)
		{
			Texture& textureData = Resources.Textures[texture];
			if (!textureData.HasMipmap)
			{
				GLint previousId;
				glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousId);

				glBindTexture(GL_TEXTURE_2D, textureData.GLID);
				glGenerateMipmap(GL_TEXTURE_2D);

				glBindTexture(GL_TEXTURE_2D, previousId);

				textureData.HasMipmap = true;
			}
		}

		void SetFiltering(ResourceIndex texture, bool filtering)
		{
			GLint previousId;
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousId);

			Texture& textureData = Resources.Textures[texture];
			glBindTexture(GL_TEXTURE_2D, textureData.GLID);
			if (filtering)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureData.HasMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureData.HasMipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}

			glBindTexture(GL_TEXTURE_2D, previousId);
		}

		ResourceIndex Load(unsigned char* buffer, int length)
		{
			int w, h, bpp;
			stbi_uc* pixels = stbi_load_from_memory(buffer, length, &w, &h, &bpp, 4);
			ResourceIndex index = Create(w, h, (GLvoid*)pixels, false, false);
			stbi_image_free((void*)pixels);
			return index;
		}

		void Delete(ResourceIndex texture)
		{
			GLuint ids[1];
			ids[0] = Resources.Textures[texture].GLID;
			glDeleteTextures(1, ids);
			DeleteResource(Resources.Textures, texture);
		}

		int GetWidth(ResourceIndex texture)
		{
			return Resources.Textures[texture].Width;
		}

		int GetHeight(ResourceIndex texture)
		{
			return Resources.Textures[texture].Height;
		}

		bool IsFlipped(ResourceIndex texture)
		{
			return Resources.Textures[texture].Flipped;
		}
	}
}

extern "C"
{
	DLLExport void SPF_SetTextureFiltering(int texture, int filtering)
	{
		return SPF::Textures::SetFiltering(texture, filtering);
	}

	DLLExport int SPF_LoadTexture(unsigned char* buffer, int length)
	{
		return SPF::Textures::Load(buffer, length);
	}

	DLLExport void SPF_DeleteTexture(int texture)
	{
		SPF::Textures::Delete(texture);
	}

	DLLExport int SPF_GetTextureWidth(int texture)
	{
		return SPF::Textures::GetWidth(texture);
	}

	DLLExport int SPF_GetTextureHeight(int texture)
	{
		return SPF::Textures::GetHeight(texture);
	}

	DLLExport int SPF_IsTextureFlipped(int texture)
	{
		return SPF::Textures::IsFlipped(texture);
	}
}
