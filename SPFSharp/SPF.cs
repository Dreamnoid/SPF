using System;
using System.Numerics;
using System.Runtime.InteropServices;

namespace SPFSharp
{
	public static partial class SPF
    {
        private static int _windowWidth, _windowHeight;

		public const Int32 InvalidResource = -1;

		public class Texture : IDisposable
		{
			public Int32 ID { get; }
			public int Width { get; }
			public int Height { get; }

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

		public class Shader : IDisposable
		{
			public Int32 ID { get; }

			public Shader(ShaderBuilder shader)
			{
				ID = Native.SPF_CreateShader(shader.ToString());
			}

			public void Dispose()
			{
				Native.SPF_DeleteShader(ID);
			}
		}

		public class Instance : IDisposable
		{
			public Instance(string title, int w, int h)
			{
				Native.SPF_Open(title, w, h);
                SPF.RefreshWindowSize();
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
			var doesContinue = Native.SPF_BeginLoop(out dt);
            Input.Update();
            RefreshWindowSize();
            return doesContinue;
        }

		public static void EndLoop()
		{
			Native.SPF_EndLoop();
		}

		public class Surface : IDisposable
		{
			public Int32 ID { get; }
			public Texture Texture { get; }
			public Texture DepthTexture { get; }
			public int Width { get; }
			public int Height { get; }
			public bool HasDepth { get; }

			public Surface(int w, int h, bool depth = false)
			{
				ID = Native.SPF_CreateSurface(w, h, depth);
				Texture = new Texture(Native.SPF_GetSurfaceTexture(ID), w, h);
				Width = w;
				Height = h;
				HasDepth = depth;
				if (depth)
				{
					DepthTexture = new Texture(Native.SPF_GetSurfaceDepthTexture(ID), w, h);
				}
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
            RefreshWindowSize();
		}

		public static int GetWindowWidth()
        {
            return _windowWidth;
        }

		public static int GetWindowHeight()
        {
            return _windowHeight;
        }

		public static bool HasFocus() => Native.SPF_HasFocus();

		public static void SetWindowSize(int w, int h)
		{
			Native.SPF_SetWindowSize(w, h);
            RefreshWindowSize();
		}

        private static void RefreshWindowSize()
        {
            _windowWidth = Native.SPF_GetWindowWidth();
            _windowHeight = Native.SPF_GetWindowHeight();
        }

		public static void SetWindowTitle(string title) => Native.SPF_SetWindowTitle(title);

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
			public float NormalX, NormalY, NormalZ;
			public float U, V, BU, BV;
			public float R, G, B, A;
			public float OverlayR, OverlayG, OverlayB, OverlayA;

			public static Vertex Create(Vector3 position, Vector2 uv) => Create(position, Vector3.Zero, uv, Vector2.Zero, Vector4.One, Vector4.Zero);

			public static Vertex Create(Vector3 position, Vector2 uv, Vector4 color) => Create(position, Vector3.Zero, uv, Vector2.Zero, color, Vector4.Zero);

			public static Vertex Create(Vector3 position, Vector3 normal, Vector2 uv, Vector2 billboard, Vector4 color, Vector4 overlay)
			{
				return new Vertex()
				{
					X = position.X,
					Y = position.Y,
					Z = position.Z,
					NormalX = normal.X,
					NormalY = normal.Y,
					NormalZ = normal.Z,
					U = uv.X,
					V = uv.Y,
					BU = billboard.X,
					BV = billboard.Y,
					R = color.X,
					G = color.Y,
					B = color.Z,
					A = color.W,
					OverlayR = overlay.X,
					OverlayG = overlay.Y,
					OverlayB = overlay.Z,
					OverlayA = overlay.W
				};
			}
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

		public static void FatalError(string msg)
		{
			Native.SPF_FatalError(msg);
		}
	}
}
