#include <Core.h>
#include <SDL3/SDL.h>
#include <cstdio>
#include "Resources.h"

namespace SPF
{
	struct
	{
		SDL_Renderer* Renderer = nullptr;
	} RendererData;

	namespace Renderer
	{
		void Init(SDL_Renderer* renderer)
		{
			RendererData.Renderer = renderer;
			SDL_SetRenderDrawBlendMode(RendererData.Renderer, SDL_BLENDMODE_BLEND);
		}

		void Begin(ResourceIndex surface, bool clear)
		{
			if (surface >= 0)
			{
				SDL_SetRenderTarget(RendererData.Renderer, (SDL_Texture*)Resources.Textures[surface].Pointer);
			}
			else
			{
				SDL_SetRenderTarget(RendererData.Renderer, nullptr);
			}

			if (clear)
			{
				SDL_SetRenderDrawColor(RendererData.Renderer, 0, 0, 0, 255);
				SDL_RenderClear(RendererData.Renderer);
			}
		}

		static SDL_FRect ToSDL(const Rect& rect)
		{
			return SDL_FRect { (float)rect.X, (float)rect.Y, (float)rect.Width, (float)rect.Height };
		}

		static SDL_FPoint ToSDL(const Vector2& vec)
		{
			return SDL_FPoint{ (float)vec.X, (float)vec.Y };
		}

		static SDL_FColor ToSDL(const RGBA& color)
		{
			return SDL_FColor{ color.R, color.G, color.B, color.A };
		}

		static void SetSDLColor(const Color& color)
		{
			SDL_SetRenderDrawColor(RendererData.Renderer, color.R, color.G, color.B, color.A);
		}

		void DetermineUV(SDL_FRect clip, int texWidth, int texHeight, bool flipX, bool flipY, Vector2& uv1, Vector2& uv2)
		{
			uv1.X = clip.x / (float)texWidth;
			uv2.X = (clip.x + clip.w) / (float)texWidth;
			uv1.Y = clip.y / (float)texHeight;
			uv2.Y = (clip.y + clip.h) / (float)texHeight;

			if (flipX)
			{
				float t = uv1.X;
				uv1.X = uv2.X;
				uv2.X = t;
			}

			if (flipY)
			{
				float t = uv1.Y;
				uv1.Y = uv2.Y;
				uv2.Y = t;
			}
		}

		void DrawTexture(
			HardwareID texture,
			Vector2 a, Vector2 b, Vector2 c, Vector2 d,
			Rect source,
			bool flipX, bool flipY,
			RGBA colorA, RGBA colorB, RGBA colorC, RGBA colorD,
			RGBA overlay)
		{
			const SPF::Texture& texInfo = Resources.Textures[texture];
			
			Vector2 uv1, uv2;
			DetermineUV(ToSDL(source), texInfo.Width, texInfo.Height, flipX, flipY, uv1, uv2);

			SDL_Vertex vertices[4]
			{
				{ ToSDL(a), ToSDL(colorA), { uv1.X, uv1.Y } },
				{ ToSDL(b), ToSDL(colorB), { uv2.X, uv1.Y } },
				{ ToSDL(c), ToSDL(colorC), { uv2.X, uv2.Y } },
				{ ToSDL(d), ToSDL(colorD), { uv1.X, uv2.Y } }
			};
			int indices[6] { 0, 1, 2, 0, 2, 3 };
			SDL_RenderGeometry(RendererData.Renderer, (SDL_Texture*)texInfo.Pointer, vertices, 4, indices, 6);

			// TODO: overlay = lazy create a copy of the texture with only alpha and draw it modulated
		}

		void FillRectangle(Rect dest, Color color)
		{
			SDL_FRect fillRect = ToSDL(dest);
			SetSDLColor(color);
			SDL_RenderFillRect(RendererData.Renderer, &fillRect);
		}

		void SetBlending(BlendMode blending)
		{
			const static SDL_BlendMode blendModes[4] =
			{
				SDL_BLENDMODE_NONE,
				SDL_BLENDMODE_BLEND,
				SDL_BLENDMODE_ADD,
				SDL_BLENDMODE_MUL
			};
			SDL_SetRenderDrawBlendMode(RendererData.Renderer, blendModes[(int)blending]);
		}

		void DrawLine(Vector2 from, Vector2 to, Color color)
		{
			SetSDLColor(color);
			SDL_RenderLine(RendererData.Renderer, from.X, from.Y, to.X, to.Y);
		}
	}
}

extern "C"
{
	DLLExport void SPF_Begin(int surface, int clear)
	{
		SPF::Renderer::Begin((SPF::ResourceIndex)surface, (bool)clear);
	}

	DLLExport void SPF_SetBlending(int blending)
	{
		SPF::Renderer::SetBlending((SPF::BlendMode)blending);
	}

	DLLExport void SPF_DrawTexture(int texture,
		float aX, float aY,
		float bX, float bY,
		float cX, float cY,
		float dX, float dY,
		int sourceX, int sourceY, int sourceW, int sourceH,
		bool flipX, bool flipY, 
		float aColorR, float aColorG, float aColorB, float aColorA,
		float bColorR, float bColorG, float bColorB, float bColorA,
		float cColorR, float cColorG, float cColorB, float cColorA,
		float dColorR, float dColorG, float dColorB, float dColorA,
		float overlayR, float overlayG, float ovelayB, float overlayA)
	{
		SPF::Renderer::DrawTexture((SPF::HardwareID)texture,
			{ aX, aY }, { bX, bY }, { cX, cY }, { dX, dY },
			{ sourceX, sourceY, sourceW, sourceH },
			flipX, flipY,
			{ aColorR, aColorG, aColorB, aColorA },
			{ bColorR, bColorG, bColorB, bColorA },
			{ cColorR, cColorG, cColorB, cColorA },
			{ dColorR, dColorG, dColorB, dColorA },
			{ overlayR, overlayG, ovelayB, overlayA });
	}

	DLLExport void SPF_FillRectangle(int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		SPF::Renderer::FillRectangle({ x, y, w, h }, { r, g, b, a });
	}
}