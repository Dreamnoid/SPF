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

		public class Shader : IResource
		{
			public Int32 ID { get; }

			public Shader(ShaderBuilder shader)
			{
				ID = Native.SPF_CreateShader(shader.ToString());
			}

			public void Dispose() => Native.SPF_DeleteShader(ID);
		}

		public class Instance : IDisposable
		{
			public Instance(string title, int w, int h)
			{
				Native.Window.SPF_Open(title, w, h);
				RefreshWindowSize();
			}

			public void Dispose()
			{
				VirtualFileSystem.CloseAll();
				Native.Window.SPF_Close();
			}
		}

		public static Instance Open(string title, int w, int h)
		{
			return new Instance(title, w, h);
		}

		public static bool BeginLoop(out float dt)
		{
			var doesContinue = Native.Window.SPF_BeginLoop(out dt);
			Input.Update();
			RefreshWindowSize();
			return doesContinue;
		}

		public static void EndLoop() => Native.Window.SPF_EndLoop();

		public static void SaveScreenshot(string filename) => Native.Window.SPF_SaveScreenshot(filename);

		public class Surface : IResource
		{
			public Int32 ID { get; }
			public Texture Texture { get; }
			public Texture DepthTexture { get; }
			public int Width { get; }
			public int Height { get; }
			public bool HasDepth { get; }

			public Surface(int w, int h, bool depth = false)
			{
				ID = Native.Surfaces.SPF_CreateSurface(w, h, depth);
				Texture = new Texture(Native.Surfaces.SPF_GetSurfaceTexture(ID), w, h, true);
				Width = w;
				Height = h;
				HasDepth = depth;
				if (depth)
				{
					DepthTexture = new Texture(Native.Surfaces.SPF_GetSurfaceDepthTexture(ID), w, h, true);
				}
			}

			public void Clear() => Native.Surfaces.SPF_ClearSurface(ID);

			public void Dispose()
			{
				Native.Textures.SPF_DeleteTexture(Native.Surfaces.SPF_GetSurfaceTexture(ID));
				Native.Surfaces.SPF_DeleteSurface(ID);
			}

			public void Attach(Texture texture) => Native.Surfaces.SPF_AttachToSurface(ID, texture.ID);

			public void CopyDepthFrom(Surface source) => Native.Surfaces.SPF_CopySurfaceDepth(ID, source.ID);
		}

		public static void SetFullscreen(bool fullscreen)
		{
			Native.Window.SPF_SetFullscreen(fullscreen);
			RefreshWindowSize();
		}

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

		public static void GetDesktopSize(out int w, out int h) => Native.Window.SPF_GetDesktopSize(out w, out h);

		public static void ShowMessageBox(string title, string message, bool isError = false) => Native.Window.SPF_ShowMessageBox(title, message, isError);

		public static string OpenFileDialog(string defaultFolder, string filter) => Native.Window.SPF_OpenFileDialog(defaultFolder, filter).MarshalToString();

		public static string SaveFileDialog(string defaultFolder, string filter) => Native.Window.SPF_SaveFileDialog(defaultFolder, filter).MarshalToString();

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

		public class Mesh : IDisposable
		{
			public Int32 ID { get; }
			public int VerticesCount { get; }

			public Mesh(Vertex[] vertices) : this(vertices, false) { }

			protected Mesh(Vertex[] vertices, bool dynamic)
			{
				ID = Native.SPF_LoadMesh(vertices, vertices.Length, dynamic);
				VerticesCount = vertices.Length;
			}

			public void Dispose()
			{
				Native.SPF_DeleteMesh(ID);
			}
		}

		public class DynamicMesh : Mesh
		{
			public Vertex[] Vertices { get; }

			public DynamicMesh(Vertex[] vertices) : base(vertices, true)
			{
				Vertices = vertices;
			}

			public void Update()
			{
				Native.SPF_UpdateMesh(ID, Vertices, VerticesCount);
			}
		}

		public static void FatalError(string msg)
		{
			Native.SPF_FatalError(msg);
		}
	}
}
