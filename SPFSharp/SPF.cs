using System;
using System.Numerics;
using System.Runtime.InteropServices;

namespace SPFSharp
{
	public static partial class SPF
	{
		private static int _windowWidth, _windowHeight;

		public const Int32 InvalidResource = -1;

		public interface IResource : IDisposable
		{
			int ID { get; }
		}

		private class Instance : IDisposable
		{
			public Instance(string title, int w, int h, uint appID)
			{
				Native.Window.SPF_Open(title, w, h, appID);
				RefreshWindowSize();
			}

			public void Dispose()
			{
				VirtualFileSystem.CloseAll();
				Native.Window.SPF_Close();
			}
		}

		public static IDisposable Open(string title, int w, int h, uint appID = 0) => new Instance(title, w, h, appID);

		public static bool BeginLoop(out float dt)
		{
			var doesContinue = Native.Window.SPF_BeginLoop(out dt);
			Input.Update();
			RefreshWindowSize();
			return doesContinue;
		}

		public static void EndLoop() => Native.Window.SPF_EndLoop();

		public static void SaveScreenshot(string filename) => Native.Window.SPF_SaveScreenshot(filename);

		public static void UnlockAchievement(string id) => Native.SPF_UnlockAchievement(id);

		public class Surface : IResource
		{
			public Int32 ID { get; }
			public Texture Texture { get; }
			public int Width { get; }
			public int Height { get; }

			public Surface(int w, int h)
			{
				ID = Native.Textures.SPF_CreateEmptyTexture((uint)w, (uint)h);
				Width = w;
				Height = h;
				Texture = new Texture(ID, w, h);
			}

			public void Dispose() => Native.Textures.SPF_DeleteTexture(ID);
		}

		public static bool IsFullscreenRequested() => Native.Window.SPF_IsFullscreenRequested();

		public static void SetFullscreen(bool fullscreen)
		{
			Native.Window.SPF_SetFullscreen(fullscreen);
			RefreshWindowSize();
		}

		public static void SetVSync(bool enabled) => Native.Window.SPF_SetVSync(enabled);

		public static int GetWindowWidth() => _windowWidth;

		public static int GetWindowHeight() => _windowHeight;

		public static bool HasFocus() => Native.Window.SPF_HasFocus();

		public static void SetWindowSize(int w, int h)
		{
			Native.Window.SPF_SetWindowSize(w, h);
			RefreshWindowSize();
		}

		private static void RefreshWindowSize()
		{
			_windowWidth = Native.Window.SPF_GetWindowWidth();
			_windowHeight = Native.Window.SPF_GetWindowHeight();
		}

		public static void SetWindowTitle(string title) => Native.Window.SPF_SetWindowTitle(title);

		public static void ShowMessageBox(string title, string message, bool isError = false) => Native.Window.SPF_ShowMessageBox(title, message, isError);

		public class Image : IDisposable
		{
			public Int32 ID { get; private set; }
			public int Width { get; private set; }
			public int Height { get; private set; }

			public Image(byte[] buffer)
			{
				using (var cbuffer = new CBuffer(buffer))
				{
					ID = Native.Images.SPF_LoadImage(cbuffer.Pointer, cbuffer.Length);
				}
				Width = Native.Images.SPF_GetImageWidth(ID);
				Height = Native.Images.SPF_GetImageHeight(ID);
			}

			public struct RGBA
			{
				public byte R, G, B, A;
			}
			public RGBA GetPixel(int x, int y)
			{
				if (x < 0 || y < 0 || x >= Width || y >= Height) return new RGBA();

				UInt32 pixel = Native.Images.SPF_GetImagePixel(ID, x, y);
				byte r = (byte)((pixel & 0xFF000000) >> 24);
				byte g = (byte)((pixel & 0x00FF0000) >> 16);
				byte b = (byte)((pixel & 0x0000FF00) >> 8);
				byte a = (byte)(pixel & 0x0000000FF);
				return new RGBA() { R = r, G = g, B = b, A = a };
			}

			public void Dispose() => Native.Images.SPF_DeleteImage(ID);

			public static void Save(string filename, int width, int height, byte[] pixels)
			{
				using (var cbuffer = new CBuffer(pixels))
				{
					Native.Images.SPF_SaveImage(filename, width, height, cbuffer.Pointer);
				}
			}
		}

		public class Font
		{
			public readonly Texture Texture;
			public readonly Glyph[] Glyphes;
			public readonly int FontHeight;
			public readonly int FirstCharacter;
			public readonly int LastCharacter;

			internal Font(Texture texture, Glyph[] glyphes, int fontHeight, int firstCharacter, int lastCharacter)
            {
                Texture = texture;
                Glyphes = glyphes;
				FontHeight = fontHeight;
				FirstCharacter = firstCharacter;
				LastCharacter = lastCharacter;
            }
        }

		[StructLayout(LayoutKind.Sequential)]
		public struct Glyph
		{
			public int X, Y, Width, Height;
			public int Advance, OffsetX, OffsetY;
		}

		public static Font LoadFont(byte[] buffer, int width, int height, int fontHeight, int firstCharacter = 32, int lastCharacter = 256)
		{
			int charactersCount = lastCharacter - firstCharacter;
			using (var cbuffer = new CBuffer(buffer))
			{
                var glyphes = new Glyph[charactersCount];
                GCHandle gcGlyphes = GCHandle.Alloc(glyphes, GCHandleType.Pinned);
				var ptr = Marshal.UnsafeAddrOfPinnedArrayElement(glyphes, 0);
                var texID = Native.Font.SPF_LoadFont(cbuffer.Pointer, width, height, fontHeight, firstCharacter, lastCharacter, gcGlyphes.AddrOfPinnedObject());
                gcGlyphes.Free();

				if (texID < 0)
					throw new Exception("Could not render font");

                var texture = new Texture(texID, width, height);
                return new Font(texture, glyphes, fontHeight, firstCharacter, lastCharacter);
			}
		}

		public static void FatalError(string msg) => Native.SPF_FatalError(msg);
	}
}
