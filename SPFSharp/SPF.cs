using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace SPFSharp
{
	public static class SPF
	{
		public class Texture : IDisposable
		{
			public UInt32 ID { get; private set; }
			public int Width { get; private set; }
			public int Height { get; private set; }

			internal Texture(UInt32 id, int w, int h)
			{
				ID = id;
				Width = w;
				Height = h;
			}

			public Texture(byte[] buffer)
			{
				var cbuffer = Marshal.AllocHGlobal(buffer.Length);
				Marshal.Copy(buffer, 0, cbuffer, buffer.Length);

				ID = Native.LoadTexture(cbuffer, buffer.Length);

				Marshal.FreeHGlobal(cbuffer);

				Width = Native.GetTextureWidth(ID);
				Height = Native.GetTextureHeight(ID);
			}

			public void Dispose()
			{
				Native.DeleteTexture(ID);
			}

			public void SetFiltering(bool filtering)
			{
				Native.SetTextureFiltering(ID, filtering);
			}
		}

		public static void Open(string title, int w, int h)
		{
			Native.Open(title, w, h);
		}

		public static bool BeginLoop(out float dt)
		{
			return Native.BeginLoop(out dt);
		}

		public static void EndLoop()
		{
			Native.EndLoop();
		}

		public static void Close()
		{
			VirtualFileSystem.CloseAll();
			Native.Close();
		}


		public static void FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a)
		{
			Native.FillRectangle(x, y, w, h, r, g, b, a);
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

		public static bool IsKeyDown(Key key)
		{
			return Native.IsKeyDown((int)key);
		}

		public static bool IsKeyPressed(Key key)
		{
			return Native.IsKeyPressed((int)key);
		}

		public static bool IsKeyReleased(Key key)
		{
			return Native.IsKeyReleased((int)key);
		}

		public enum Key : int
		{
			Up = 0,
			Down = 1,
			Right = 2,
			Left = 3,
			Space = 4,
			Escape = 5,
			Delete = 6,
			Z = 7,
			X = 8,
			C = 9
		}

		public class Surface : IDisposable
		{
			public UInt32 ID { get; private set; }
			public Texture Texture { get; private set; }
			public int Width { get; private set; }
			public int Height { get; private set; }

			public Surface(int w, int h)
			{
				ID = Native.CreateSurface(w, h);
				Texture = new Texture(Native.GetSurfaceTexture(ID), w, h);
				Width = w;
				Height = h;
			}
			
			public void Dispose()
			{
				Native.DeleteTexture(Native.GetSurfaceTexture(ID));
				Native.DeleteSurface(ID);
			}

			public void Begin()
			{
				Native.BeginSurface(ID);
			}

			public void End()
			{
				Native.EndSurface();
			}
		}

		public enum BlendMode : int
		{
			Alpha = 0,
			Additive = 1
		}

		public static void SetBlending(BlendMode blendMode)
		{
			Native.SetBlending((int)blendMode);
		}

		public static void SetFullscreen(bool fullscreen)
		{
			Native.SetFullscreen(fullscreen);
		}

		public static int GetWindowWidth()
		{
			return Native.GetWindowWidth();
		}

		public static int GetWindowHeight()
		{
			return Native.GetWindowHeight();
		}

		public class Sound : IDisposable
		{
			public UInt32 ID { get; private set; }

			public Sound(byte[] buffer)
			{
				var cbuffer = Marshal.AllocHGlobal(buffer.Length);
				Marshal.Copy(buffer, 0, cbuffer, buffer.Length);

				ID = Native.LoadSound(cbuffer, buffer.Length);

				Marshal.FreeHGlobal(cbuffer);
			}

			public void Dispose()
			{
				Native.DeleteSound(ID);
			}

			public int Play(bool looping = false)
			{
				return Native.PlaySound(ID, looping);
			}
		}

		public static void StopChannel(int channel)
		{
			Native.StopChannel(channel);
		}

		public enum Button : int
		{
			A = 0,
			B = 1,
			X = 2,
			Y = 3,
			Start = 4,
			Select = 5,
			DPadUp = 6,
			DPadDown = 7,
			DPadRight = 8,
			DPadLeft = 9,
			LeftShoulder = 10,
			RightShoulder = 11
		}

		public static bool IsButtonDown(Button button)
		{
			return Native.IsButtonDown((int)button);
		}

		public static bool IsButtonPressed(Button button)
		{
			return Native.IsButtonPressed((int)button);
		}

		public static bool IsButtonReleased(Button button)
		{
			return Native.IsButtonReleased((int)button);
		}

		public static void AddArchive(string filename)
		{
			VirtualFileSystem.AddArchive(filename);
		}

		public static byte[] ReadFile(string filename)
		{
			return VirtualFileSystem.Read(filename);
		}

		public static string[] ListArchivedFiles()
		{
			return VirtualFileSystem.ListArchivedFiles();
		}

		public static float GetVolume()
		{
			return Native.GetVolume();
		}

		public static void SetVolume(float volume)
		{
			Native.SetVolume(volume);
		}

		public static bool IsControllerConnected()
		{
			return Native.IsControllerConnected();
		}

		public class Image : IDisposable
		{
			public UInt32 ID { get; private set; }
			public int Width { get; private set; }
			public int Height { get; private set; }

			public Image(byte[] buffer)
			{
				var cbuffer = Marshal.AllocHGlobal(buffer.Length);
				Marshal.Copy(buffer, 0, cbuffer, buffer.Length);
				ID = Native.LoadImage(cbuffer, buffer.Length);
				Marshal.FreeHGlobal(cbuffer);

				Width = Native.GetImageWidth(ID);
				Height = Native.GetImageHeight(ID);
			}

			public struct RGBA
			{
				public byte R, G, B, A;
			}
			public RGBA GetPixel(int x, int y)
			{
				if (x < 0 || y < 0 || x >= Width || y >= Height) return new RGBA();

				UInt32 pixel = Native.GetImagePixel(ID, x, y);
				byte r = (byte)((pixel & 0xFF000000) >> 24);
				byte g = (byte)((pixel & 0x00FF0000) >> 16);
				byte b = (byte)((pixel & 0x0000FF00) >> 8);
				byte a = (byte)(pixel & 0x0000000FF);
				return new RGBA() { R = r, G = g, B = b, A = a };
			}

			public void Dispose()
			{
				Native.DeleteImage(ID);
			}
		}

	}
}
