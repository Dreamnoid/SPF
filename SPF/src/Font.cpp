#include <vector>
#include <stdio.h>

#include <Core.h>
#include <Textures.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

namespace SPF
{
	struct Glyph
	{
		Rect Area;
		int Advance;
		int OffsetX;
		int OffsetY;
	};

	ResourceIndex LoadFont(const unsigned char* buffer, int width, int height, int fontSize, int firstCharacter, int lastCharacter, SPF::Glyph* glyphes)
	{
		const int charactersCount = lastCharacter - firstCharacter;
		unsigned char* alphaChannel = (unsigned char*)malloc(width * height * sizeof(unsigned char));
		std::vector<stbtt_bakedchar> characters(charactersCount);

		int result = stbtt_BakeFontBitmap(buffer, 0, (float)fontSize, alphaChannel, width, height, firstCharacter, charactersCount, characters.data());
		if (!result)
		{
			return InvalidResource;
		}

		for (int i = 0; i < charactersCount; ++i)
		{
			const stbtt_bakedchar& stb_char = characters[i];
			Glyph glyph = {};

			glyph.Area = { stb_char.x0, stb_char.y0, stb_char.x1 - stb_char.x0, stb_char.y1 - stb_char.y0 };
			glyph.Advance = (int)stb_char.xadvance;
			glyph.OffsetX = (int)stb_char.xoff;
			glyph.OffsetY = (int)stb_char.yoff;

			glyphes[i] = glyph;
		}

		// Expand A to RGBA
		unsigned char* rgbaChannel = (unsigned char*)malloc(width * height * 4 * sizeof(unsigned char));
		unsigned char* src = alphaChannel;
		unsigned char* dest = rgbaChannel;
		for (int i = 0; i < (width*height); ++i)
		{
			unsigned char alpha = *src++;
			if (fontSize <= 16)
			{
				alpha = std::min(alpha + (alpha / 3), 255);
			}
			*dest++ = 0xFF; // R
			*dest++ = 0xFF; // G
			*dest++ = 0xFF; // B
			*dest++ = alpha; // A
		}

		free(alphaChannel);

		ResourceIndex textureID = Textures::Create(width, height, (void*)rgbaChannel);
		free(rgbaChannel);

		return textureID;
	}
}

extern "C"
{
	DLLExport int SPF_LoadFont(unsigned char* buffer, int width, int height, int fontSize, int firstCharacter, int lastCharacter, SPF::Glyph* glyphes)
	{
		return SPF::LoadFont(buffer, width, height, fontSize, firstCharacter, lastCharacter, glyphes);
	}
}