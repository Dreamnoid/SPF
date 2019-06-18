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
					ID = Native.SPF_LoadTexture(cbuffer.Pointer, cbuffer.Length);
				}

				Width = Native.SPF_GetTextureWidth(ID);
				Height = Native.SPF_GetTextureHeight(ID);
			}

			public void Dispose()
			{
				Native.SPF_DeleteTexture(ID);
			}

			public void SetFiltering(bool filtering)
			{
				Native.SPF_SetTextureFiltering(ID, filtering);
			}
		}

		public class Instance : IDisposable
		{
			public Instance(string title, int w, int h)
			{
				Native.SPF_Open(title, w, h);
			}

			public void Dispose()
			{
				VirtualFileSystem.CloseAll();
				Native.SPF_Close();
			}
		}

		public static Instance Open(string title, int w, int h)
		{
			return new Instance(title, w, h);
		}

		public static bool BeginLoop(out float dt)
		{
			return Native.SPF_BeginLoop(out dt);
		}

		public static void EndLoop()
		{
			Native.SPF_EndLoop();
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
				ID = Native.SPF_CreateSurface(w, h, depth);
				Texture = new Texture(Native.SPF_GetSurfaceTexture(ID), w, h);
				Width = w;
				Height = h;
				HasDepth = depth;
			}

			public void Clear()
			{
				Native.SPF_ClearSurface(ID);
			}

			public void Dispose()
			{
				Native.SPF_DeleteTexture(Native.SPF_GetSurfaceTexture(ID));
				Native.SPF_DeleteSurface(ID);
			}
		}

		public static void SetFullscreen(bool fullscreen)
		{
			Native.SPF_SetFullscreen(fullscreen);
		}

		public static int GetWindowWidth()
		{
			return Native.SPF_GetWindowWidth();
		}

		public static int GetWindowHeight()
		{
			return Native.SPF_GetWindowHeight();
		}

		public static void SetWindowSize(int w, int h)
		{
			Native.SPF_SetWindowSize(w, h);
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
					ID = Native.SPF_LoadImage(cbuffer.Pointer, cbuffer.Length);
				}
				Width = Native.SPF_GetImageWidth(ID);
				Height = Native.SPF_GetImageHeight(ID);
			}

			public struct RGBA
			{
				public byte R, G, B, A;
			}
			public RGBA GetPixel(int x, int y)
			{
				if (x < 0 || y < 0 || x >= Width || y >= Height) return new RGBA();

				UInt32 pixel = Native.SPF_GetImagePixel(ID, x, y);
				byte r = (byte)((pixel & 0xFF000000) >> 24);
				byte g = (byte)((pixel & 0x00FF0000) >> 16);
				byte b = (byte)((pixel & 0x0000FF00) >> 8);
				byte a = (byte)(pixel & 0x0000000FF);
				return new RGBA() { R = r, G = g, B = b, A = a };
			}

			public void Dispose()
			{
				Native.SPF_DeleteImage(ID);
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
				ID = Native.SPF_LoadMesh(vertices, vertices.Length);
				VerticesCount = vertices.Length;
			}

			public void Dispose()
			{
				Native.SPF_DeleteMesh(ID);
			}
		}
	}
}
