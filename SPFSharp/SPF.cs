using System;
using System.Collections.Generic;
using System.Linq;
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
			foreach (var tex in _loadedTextures.Values)
			{
				Native.DeleteTexture(tex.ID);
			}
			Native.Close();
		}

		public static Texture GetTexture(string filename)
		{
			if (_loadedTextures.ContainsKey(filename) == false)
			{
                var tex = new Texture();
                tex.ID = Native.LoadTexture(filename);
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
            Native.DrawTexture(tex.ID, x, y, tex.Width, tex.Height, 0, 0, tex.Width, tex.Height, false, false, 1, 1, 1, 1);
        }

        public static void DrawTexture(Texture tex, int x, int y, int w, int h, int clipx, int clipy, int clipw, int cliph, bool flipx, bool flipy, float r, float g, float b, float a)
		{
            Native.DrawTexture(tex.ID, x, y, w, h, clipx, clipy, clipw, cliph, flipx, flipy, r, g, b, a);
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
			Escape = 5
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

    }
}
