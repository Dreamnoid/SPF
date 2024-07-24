#include <vector>
#include <Images.h>

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#include "stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define __STDC_LIB_EXT1__
#include "stb_image_write.h"

namespace SPF
{
	struct Image
	{
		bool InUse;
		void* Pixels;
		unsigned int Width;
		unsigned int Height;
	};

	struct
	{
		std::vector<Image> Images;
	} ImagesData;

	namespace Images
	{
		ResourceIndex Load(unsigned char* buffer, int length)
		{
			int w, h, bpp;
			stbi_uc* pixels = stbi_load_from_memory(buffer, length, &w, &h, &bpp, 4);

			for (ResourceIndex imgID = 0; imgID < ImagesData.Images.size(); ++imgID)
			{
				if (!ImagesData.Images[imgID].InUse)
				{
					ImagesData.Images[imgID] = { true,pixels,(unsigned int)w,(unsigned int)h };
					return imgID;
				}
			}
			ImagesData.Images.push_back({ true, pixels,(unsigned int)w,(unsigned int)h });
			return ImagesData.Images.size() - 1;
		}

		void Save(const char* filename, int width, int height, unsigned char* pixels)
		{
			stbi_write_png(filename, width, height, 4, (const void*)pixels, width * 4);
		}

		void Delete(ResourceIndex image)
		{
			stbi_image_free(ImagesData.Images[image].Pixels);
			ImagesData.Images[image].InUse = 0;
		}

		int GetWidth(ResourceIndex image)
		{
			return ImagesData.Images[image].Width;
		}

		int GetHeight(ResourceIndex image)
		{
			return ImagesData.Images[image].Height;
		}

		unsigned int GetPixel(ResourceIndex image, int x, int y)
		{
			int index = (y * ImagesData.Images[image].Width + x) * 4;
			stbi_uc* pixels = (stbi_uc*)ImagesData.Images[image].Pixels;
			unsigned int r = pixels[index + 0];
			unsigned int g = pixels[index + 1];
			unsigned int b = pixels[index + 2];
			unsigned int a = pixels[index + 3];
			return (r << 24) | (g << 16) | (b << 8) | a;
		}
	}
}

extern "C"
{
	DLLExport SPF::ResourceIndex SPF_LoadImage(unsigned char* buffer, int length)
	{
		return SPF::Images::Load(buffer, length);
	}

	DLLExport void SPF_SaveImage(const char* filename, int width, int height, unsigned char* pixels)
	{
		SPF::Images::Save(filename, width, height, pixels);
	}

	DLLExport void SPF_DeleteImage(SPF::ResourceIndex image)
	{
		SPF::Images::Delete(image);
	}

	DLLExport int SPF_GetImageWidth(SPF::ResourceIndex image)
	{
		return SPF::Images::GetWidth(image);
	}

	DLLExport int SPF_GetImageHeight(SPF::ResourceIndex image)
	{
		return SPF::Images::GetHeight(image);
	}

	DLLExport unsigned int SPF_GetImagePixel(SPF::ResourceIndex image, int x, int y)
	{
		return SPF::Images::GetPixel(image, x, y);
	}
}