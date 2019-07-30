#include <Textures.h>
#include "gl4.h"
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
		ResourceIndex Create(unsigned int w, unsigned int h, void* pixels, bool flipped)
		{
			GLuint ids[1];
			glGenTextures(1, ids);
			GLuint id = ids[0];

			glBindTexture(GL_TEXTURE_2D, id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

			for (ResourceIndex texID = 0; texID < Resources.Textures.size(); ++texID)
			{
				if (!Resources.Textures[texID].InUse)
				{
					Resources.Textures[texID] = { true,id,w,h,flipped };
					return texID;
				}
			}

			Resources.Textures.push_back({ true,id,w,h,flipped });
			return Resources.Textures.size() - 1;
		}

		void SetFiltering(ResourceIndex texture, bool filtering)
		{
			glBindTexture(GL_TEXTURE_2D, Resources.Textures[texture].GLID);
			if (filtering)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
		}

		ResourceIndex Load(unsigned char* buffer, int length)
		{
			int w, h, bpp;
			stbi_uc* pixels = stbi_load_from_memory(buffer, length, &w, &h, &bpp, 4);
			ResourceIndex index = Create(w, h, (GLvoid*)pixels, 0);
			stbi_image_free((void*)pixels);
			return index;
		}

		void Delete(ResourceIndex texture)
		{
			Resources.Textures[texture].InUse = 0;
			GLuint ids[1];
			ids[0] = Resources.Textures[texture].GLID;
			glDeleteTextures(1, ids);
		}

		int GetWidth(ResourceIndex texture)
		{
			return Resources.Textures[texture].Width;
		}

		int GetHeight(ResourceIndex texture)
		{
			return Resources.Textures[texture].Height;
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
}