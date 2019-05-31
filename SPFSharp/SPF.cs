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
			public Int32 ID { get; private set; }
			public int Width { get; private set; }
			public int Height { get; private set; }

			internal Texture(Int32 id, int w, int h)
			{
				ID = id;
				Width = w;
				Height = h;
			}

			public Texture(byte[] buffer)
			{
				using (var cbuffer = new CBuffer(buffer))
				{
					ID = Native.LoadTexture(cbuffer.Pointer, cbuffer.Length);
				}

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
			public Int32 ID { get; private set; }
			public Texture Texture { get; private set; }
			public int Width { get; private set; }
			public int Height { get; private set; }
			public bool HasDepth { get; private set; }

			public Surface(int w, int h, bool depth = false)
			{
				ID = Native.CreateSurface(w, h, depth);
				Texture = new Texture(Native.GetSurfaceTexture(ID), w, h);
				Width = w;
				Height = h;
				HasDepth = depth;
			}

			public void Clear()
			{
				Native.ClearSurface(ID);
			}

			public void Dispose()
			{
				Native.DeleteTexture(Native.GetSurfaceTexture(ID));
				Native.DeleteSurface(ID);
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

		public static void SetWindowSize(int w, int h)
		{
			Native.SetWindowSize(w, h);
		}

		public class Image : IDisposable
		{
			public Int32 ID { get; private set; }
			public int Width { get; private set; }
			public int Height { get; private set; }

			public Image(byte[] buffer)
			{
				using (var cbuffer = new CBuffer(buffer))
				{
					ID = Native.LoadImage(cbuffer.Pointer, cbuffer.Length);
				}
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

		[StructLayout(LayoutKind.Sequential)]
		public struct Vertex
		{
			public float X, Y, Z;
			public float U, V, BU, BV;
			public float R, G, B, A;
			public float OverlayR, OverlayG, OverlayB, OverlayA;
		}

		public class Mesh : IDisposable
		{
			public Int32 ID { get; private set; }
			public int VerticesCount { get; private set; }

			public Mesh(Vertex[] vertices)
			{
				ID = Native.LoadMesh(vertices, vertices.Length);
				VerticesCount = vertices.Length;
			}

			public void Dispose()
			{
				Native.DeleteMesh(ID);
			}
		}
	}
}
