﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SPFSharp
{
	public static partial class SPF
	{
		public enum BlendMode : int
		{
			Alpha = 0,
			Additive = 1
		}

		public static class Renderer
		{
			public static void SetBlending(BlendMode blendMode)
			{
				Native.SetBlending((int)blendMode);
			}

			public static void FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a)
			{
				Native.FillRectangle(x, y, w, h, r, g, b, a);
			}

			public static void DrawRectangle(int x, int y, int w, int h, float r, float g, float b, float a)
			{
				Native.FillRectangle(x, y, w, 1, r, g, b, a);
				Native.FillRectangle(x, y + h - 1, w, 1, r, g, b, a);
				Native.FillRectangle(x, y + 1, 1, h - 2, r, g, b, a);
				Native.FillRectangle(x + w - 1, y + 1, 1, h - 2, r, g, b, a);
			}

			public static void DrawTexture(Texture tex, int x, int y)
			{
				Native.DrawTexture(tex.ID, x, y, tex.Width, tex.Height, 0, 0, tex.Width, tex.Height, false, false, 1, 1, 1, 1, 0, 0, 0, 0);
			}

			public static void DrawTexture(Texture tex,
				int x, int y, int w, int h,
				int clipx, int clipy, int clipw, int cliph,
				bool flipx, bool flipy,
				float r, float g, float b, float a)
			{
				Native.DrawTexture(tex.ID, x, y, w, h, clipx, clipy, clipw, cliph, flipx, flipy, r, g, b, a, 0, 0, 0, 0);
			}

			public static void DrawTexture(Texture tex,
				int x, int y, int w, int h,
				int clipx, int clipy, int clipw, int cliph,
				bool flipx, bool flipy,
				float r, float g, float b, float a,
				float overlayR, float overlayG, float overlayB, float overlayA)
			{
				Native.DrawTexture(tex.ID, x, y, w, h, clipx, clipy, clipw, cliph, flipx, flipy, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
			}

			public static void DrawTexturedQuad(Texture tex,
				float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Dx, float Dy,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				float r, float g, float b, float a,
				float overlayR, float overlayG, float overlayB, float overlayA)
			{
				Native.DrawTexturedQuad(tex.ID, Ax, Ay, Bx, By, Cx, Cy, Dx, Dy, srcx, srcy, srcw, srch, flipX, flipY, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
			}

		}
	}
}