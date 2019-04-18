#include "Textures.h"
#include "gl4.h"
#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include "stb_image.h"

Textures mTextures;

ResourceIndex Textures::Create(int w, int h, GLvoid* pixels, bool flipped)
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

	ResourceIndex texID = 0;
	for (ResourceIndex texID = 0; texID < TexturesCount; ++texID)
	{
		if (!mTextures[texID].InUse)
		{
			mTextures[texID].GLID = id;
			mTextures[texID].Width = w;
			mTextures[texID].Height = h;
			mTextures[texID].InUse = 1;
			mTextures[texID].Flipped = flipped;
			return texID;
		}
	}
	FatalError("All texture slots are used");
}

void Textures::SetFiltering(ResourceIndex texture, bool filtering)
{
	glBindTexture(GL_TEXTURE_2D, mTextures[texture].GLID);
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

ResourceIndex Textures::Load(unsigned char* buffer, int length)
{
	int w, h, bpp;
	stbi_uc* pixels = stbi_load_from_memory(buffer, length, &w, &h, &bpp, 4);
	ResourceIndex index = Create(w, h, (GLvoid*)pixels, 0);
	stbi_image_free((void*)pixels);
	return index;
}

void Textures::Delete(ResourceIndex texture)
{
	mTextures[texture].InUse = 0;
	GLuint ids[1];
	ids[0] = mTextures[texture].GLID;
	glDeleteTextures(1, ids);
}

int Textures::GetWidth(ResourceIndex texture)
{
	return mTextures[texture].Width;
}

int Textures::GetHeight(ResourceIndex texture)
{
	return mTextures[texture].Height;
}

extern "C"
{
	DLLExport void SetTextureFiltering(ResourceIndex texture, int filtering)
	{
		return mTextures.SetFiltering(texture, filtering);
	}

	DLLExport ResourceIndex LoadTexture(unsigned char* buffer, int length)
	{
		return mTextures.Load(buffer, length);
	}

	DLLExport void DeleteTexture(ResourceIndex texture)
	{
		mTextures.Delete(texture);
	}

	DLLExport int GetTextureWidth(ResourceIndex texture)
	{
		return mTextures.GetWidth(texture);
	}

	DLLExport int GetTextureHeight(ResourceIndex texture)
	{
		return mTextures.GetHeight(texture);
	}
}