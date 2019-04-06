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
		public class Texture
		{
			public UInt32 ID;
			public int Width;
			public int Height;
		}
		private static Dictionary<string, Texture> _loadedTextures = new Dictionary<string, Texture>();

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
			foreach (var tex in _loadedTextures.Values)
			{
				Native.DeleteTexture(tex.ID);
			}
			foreach (var sound in _loadedSounds.Values)
			{
				Native.DeleteSound(sound.ID);
			}
			Native.Close();
		}

		public static Texture GetTexture(string filename)
		{
			if (_loadedTextures.ContainsKey(filename) == false)
			{
				var tex = new Texture();

				var buffer = VirtualFileSystem.Read(filename);

				var cbuffer = Marshal.AllocHGlobal(buffer.Length);
				Marshal.Copy(buffer, 0, cbuffer, buffer.Length);

				tex.ID = Native.LoadTexture(cbuffer, buffer.Length);

				Marshal.FreeHGlobal(cbuffer);

				tex.Width = Native.GetTextureWidth(tex.ID);
				tex.Height = Native.GetTextureHeight(tex.ID);
				_loadedTextures[filename] = tex;
			}
			return _loadedTextures[filename];
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

		public static void SetTextureFiltering(Texture texture, bool filtering)
		{
			Native.SetTextureFiltering(texture.ID, filtering);
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

		public class Surface
		{
			public UInt32 ID;
			public Texture Texture;
			public int Width;
			public int Height;
		}

		public static Surface CreateSurface(int w, int h)
		{
			var id = Native.CreateSurface(w, h);
			return new Surface()
			{
				ID = id,
				Texture = new Texture() { ID = Native.GetSurfaceTexture(id), Width = w, Height = h },
				Width = w,
				Height = h
			};
		}

		public static void BeginSurface(Surface surface)
		{
			Native.BeginSurface(surface.ID);
		}

		public static void EndSurface()
		{
			Native.EndSurface();
		}

		public static void DeleteSurface(Surface surface)
		{
			Native.DeleteTexture(Native.GetSurfaceTexture(surface.ID));
			Native.DeleteSurface(surface.ID);
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

		public class Sound
		{
			public UInt32 ID;
		}
		private static Dictionary<string, Sound> _loadedSounds = new Dictionary<string, Sound>();

		public static Sound GetSound(string filename)
		{
			if (_loadedSounds.ContainsKey(filename) == false)
			{
				var sound = new Sound();

				var buffer = VirtualFileSystem.Read(filename);

				var cbuffer = Marshal.AllocHGlobal(buffer.Length);
				Marshal.Copy(buffer, 0, cbuffer, buffer.Length);

				sound.ID = Native.LoadSound(cbuffer, buffer.Length);

				Marshal.FreeHGlobal(cbuffer);

				_loadedSounds[filename] = sound;
			}
			return _loadedSounds[filename];
		}

		public static void PlaySound(Sound sound)
		{
			Native.PlaySound(sound.ID, false);
		}

		public static int PlaySound(Sound sound, bool looping)
		{
			return Native.PlaySound(sound.ID, looping);
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
			public UInt32 ID;
			public int Width;
			public int Height;

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
