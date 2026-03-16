using System.Numerics;

namespace SPFSharp
{
	public static partial class SPF
	{
		public enum BlendMode : int
		{
			None = 0,
			Alpha = 1,
			Additive = 2,
			Multiply = 3
		}

		public static class Renderer
		{
			private static int Resolve(IResource resource) => (resource != null) ? resource.ID : -1;

			private static Surface _currentSurface = null;

			public static void Begin() => Begin(null);

			public static void Begin(Surface surface, bool clear = false)
			{
				_currentSurface = surface;
				Native.Renderer.SPF_Begin(Resolve(surface), clear);
			}

            public static void FillRectangle(int x, int y, int w, int h, byte r, byte g, byte b, byte a)
                => Native.Renderer.SPF_FillRectangle(x, y, w, h, r, g, b, a);

            public static void FillRectangle(int x, int y, int w, int h, in Vector4 color)
				=> Native.Renderer.SPF_FillRectangle(x, y, w, h, (byte)(color.X * 255), (byte)(color.Y * 255), (byte)(color.Z * 255), (byte)(color.W * 255));

			public static void DrawRectangle(int x, int y, int w, int h, Vector4 color, int lineWidth = 1)
			{
				FillRectangle(x, y, w, lineWidth, color);
				FillRectangle(x, y + h - lineWidth, w, lineWidth, color);
				FillRectangle(x, y + lineWidth, lineWidth, h - (lineWidth * 2), color);
				FillRectangle(x + w - lineWidth, y + lineWidth, lineWidth, h - (lineWidth * 2), color);
			}

			public static void DrawTexture(Texture tex, int x, int y)
				=> DrawTexture(tex,
					x, y, tex.Width, tex.Height,
					0, 0, tex.Width, tex.Height,
					false, false,
					Vector4.One, Vector4.Zero);

			public static void DrawTexture(Texture tex,
				int x, int y, int w, int h,
				int clipx, int clipy, int clipw, int cliph,
				bool flipx, bool flipy,
				Vector4 color) 
				=> DrawTexture(tex,
					x, y, w, h, 
					clipx, clipy, clipw, cliph,
					flipx, flipy,
					color, Vector4.Zero);

			public static void DrawTexture(Texture tex,
				int x, int y, int w, int h,
				int clipx, int clipy, int clipw, int cliph,
				bool flipx, bool flipy,
				Vector4 color,
				Vector4 overlay)
			{
				DrawTexturedQuad(tex, 
					new Vector2(x, y), 
					new Vector2(x + w, y), 
					new Vector2(x + w, y + h), 
					new Vector2(x, y + h), 
					clipx, clipy, clipw, cliph, 
					flipx, flipy, 
					color, overlay);
			}

			public static void DrawTexturedQuad(Texture tex,
				in Vector2 a,
				in Vector2 b,
				in Vector2 c,
				in Vector2 d,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				in Vector4 color,
				in Vector4 overlay)
			{
				DrawTexturedQuad(
					tex,
					new Vector3(a.X, a.Y, 0f),
					new Vector3(b.X, b.Y, 0f),
					new Vector3(c.X, c.Y, 0f),
					new Vector3(d.X, d.Y, 0f),
					srcx, srcy, srcw, srch,
					flipX, flipY,
					color, color, color, color,
					overlay);
			}

			public static void DrawTexturedQuad(Texture tex,
				in Vector2 a,
				in Vector2 b,
				in Vector2 c,
				in Vector2 d,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				in Vector4 aColor, in Vector4 bColor, in Vector4 cColor, in Vector4 dColor,
				in Vector4 overlay)
			{
				DrawTexturedQuad(
					tex,
					new Vector3(a.X, a.Y, 0f),
					new Vector3(b.X, b.Y, 0f),
					new Vector3(c.X, c.Y, 0f),
					new Vector3(d.X, d.Y, 0f),
					srcx, srcy, srcw, srch,
					flipX, flipY,
					aColor, bColor, cColor, dColor,
					overlay);
			}

			public static void DrawTexturedQuad(Texture tex,
				in Vector3 a,
				in Vector3 b,
				in Vector3 c,
				in Vector3 d,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				in Vector4 color,
				in Vector4 overlay)
			{
				DrawTexturedQuad(
					tex,
					a, b, c, d,
					srcx, srcy, srcw, srch,
					flipX, flipY,
					color, color, color, color,
					overlay);
			}

			public static void DrawTexturedQuad(Texture tex,
				in Vector3 a,
				in Vector3 b,
				in Vector3 c,
				in Vector3 d,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				in Vector4 aColor, in Vector4 bColor, in Vector4 cColor, in Vector4 dColor,
				in Vector4 overlay)
			{
				Native.Renderer.SPF_DrawTexture(tex.ID,
					a.X, a.Y,
					b.X, b.Y,
					c.X, c.Y,
					d.X, d.Y,
					srcx, srcy, srcw, srch,
					flipX, flipY,
					aColor.X, aColor.Y, aColor.Z, aColor.W,
					bColor.X, bColor.Y, bColor.Z, bColor.W,
					cColor.X, cColor.Y, cColor.Z, cColor.W,
                    dColor.X, dColor.Y, dColor.Z, dColor.W,
                    overlay.X, overlay.Y, overlay.Z, overlay.W);
			}

			public static void SetBlending(BlendMode blendMode = BlendMode.Alpha)
				=> Native.Renderer.SPF_SetBlending((int)blendMode);
		}
	}
}
