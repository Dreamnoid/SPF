using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SPFSharp
{
	public static class SPF
	{
		private static Dictionary<string, int> _loadedTextures = new Dictionary<string, int>();

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
			foreach (var texID in _loadedTextures.Values)
			{
				Native.DeleteTexture(texID);
			}
			Native.Close();
		}

		public static int GetTexture(string filename)
		{
			if (_loadedTextures.ContainsKey(filename) == false)
			{
				_loadedTextures[filename] = Native.LoadTexture(filename);
			}
			return _loadedTextures[filename];
		}

		public static void FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a)
		{
			Native.FillRectangle(x, y, w, h, r, g, b, a);
		}

		public static void DrawTexture(int tex, int x, int y, int w, int h, int clipx, int clipy, int clipw, int cliph, float r, float g, float b, float a)
		{
			Native.DrawTexture(tex, x, y, w, h, clipx, clipy, clipw, cliph, r, g, b, a);
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
	}
}
