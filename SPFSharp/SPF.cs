using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace SPFSharp
{
	public static partial class SPF
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
