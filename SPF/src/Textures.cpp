#include <Core.h>
#include <SDL3/SDL.h>
#include <cstdio>
#include <vector>
#include "Resources.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include "stb_image.h"

namespace SPF
{
	struct
	{
		SDL_Renderer* Renderer = nullptr;
	} TexturesData;

	namespace Textures
	{
		void Init(SDL_Renderer* renderer)
		{
			TexturesData.Renderer = renderer;
		}

		ResourceIndex Create(unsigned int w, unsigned int h, void* pixels)
		{
			SDL_TextureAccess access = pixels != nullptr ? SDL_TEXTUREACCESS_STATIC : SDL_TEXTUREACCESS_TARGET;

			SDL_Texture* texture = SDL_CreateTexture(TexturesData.Renderer, SDL_PIXELFORMAT_ABGR8888, access, w, h);
			if (!texture)
			{
				FatalError(SDL_GetError());
			}

			if (pixels != nullptr && !SDL_UpdateTexture(texture, nullptr, pixels, w * 4))
			{
				FatalError(SDL_GetError());
			}

			SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

			SDL_Texture* mask = nullptr;
			if (pixels != nullptr)
			{
				mask = SDL_CreateTexture(TexturesData.Renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, w, h);
				uint8_t* maskPixels = (uint8_t*)SDL_malloc(w * h * 4);
				SDL_memset(maskPixels, 255, w * h * 4);
				for (unsigned int i = 0; i < w * h; ++i)
				{
					maskPixels[i * 4 + 3] = ((uint8_t*)pixels)[i * 4 + 3];
				}
				if (!SDL_UpdateTexture(mask, nullptr, maskPixels, w * 4))
				{
					FatalError(SDL_GetError());
				}
				SDL_SetTextureScaleMode(mask, SDL_SCALEMODE_NEAREST);
			}

			return CreateResource(Resources.Textures, { true, texture, mask, w, h });
		}

		void SetFiltering(ResourceIndex texture, bool filtering)
		{
			const Texture& texInfo = Resources.Textures[texture];
			SDL_SetTextureScaleMode((SDL_Texture*)texInfo.Pointer, filtering ? SDL_SCALEMODE_LINEAR : SDL_SCALEMODE_NEAREST);
			if (texInfo.MaskPointer != nullptr)
			{
				SDL_SetTextureScaleMode((SDL_Texture*)texInfo.MaskPointer, filtering ? SDL_SCALEMODE_LINEAR : SDL_SCALEMODE_NEAREST);
			}
		}

		ResourceIndex Load(unsigned char* buffer, int length)
		{
			int w, h, bpp;
			stbi_uc* pixels = stbi_load_from_memory(buffer, length, &w, &h, &bpp, 4);
			ResourceIndex index = Create(w, h, pixels);
			stbi_image_free((void*)pixels);
			return index;
		}

		void Delete(ResourceIndex texture)
		{
			const Texture& texInfo = Resources.Textures[texture];
			SDL_DestroyTexture((SDL_Texture*)texInfo.Pointer);
			if (texInfo.MaskPointer != nullptr)
			{
				SDL_DestroyTexture((SDL_Texture*)texInfo.MaskPointer);
			}
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

	DLLExport int SPF_CreateEmptyTexture(unsigned int w, unsigned int h)
	{
		return SPF::Textures::Create(w, h, nullptr);
	}

	DLLExport int SPF_CreateWhiteTexture()
	{
		static unsigned int pixels[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
		return SPF::Textures::Create(2, 2, pixels);
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
