using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SPFSharp
{
    public static class Program
    {
        const int WindowWidth = 320;
        const int WindowHeight = 240;

        const int SurfaceWidth = 320;
        const int SurfaceHeight = 240;

		public static void Main()
		{
			SPF.Open("Test", WindowWidth, WindowHeight);
			SPF.AddArchive("D:/Test/Test.zip");

			var tex = SPF.GetTexture("test.png");
			var surface = SPF.CreateSurface(SurfaceWidth, SurfaceHeight);

			////SPF.SetFullscreen(true);
			var sound = SPF.GetSound("tada.wav");

			float x = 0, y = 0;

			float dt;
			while (SPF.BeginLoop(out dt))
			{
				if (SPF.IsKeyDown(SPF.Key.Down) || SPF.IsButtonDown(SPF.Button.DPadDown))
				{
					y += 100 * dt;
				}
				else if (SPF.IsKeyDown(SPF.Key.Up) || SPF.IsButtonDown(SPF.Button.DPadUp))
				{
					y -= 100 * dt;
				}
				if (SPF.IsKeyDown(SPF.Key.Left) || SPF.IsButtonDown(SPF.Button.DPadLeft))
				{
					x -= 100 * dt;
				}
				else if (SPF.IsKeyDown(SPF.Key.Right) || SPF.IsButtonDown(SPF.Button.DPadRight))
				{
					x += 100 * dt;
				}

				if (SPF.IsKeyPressed(SPF.Key.Space) || SPF.IsButtonPressed(SPF.Button.A))
				{
					SPF.PlaySound(sound);
				}
				if (SPF.IsKeyPressed(SPF.Key.Escape))
				{
					break;
				}

				SPF.BeginSurface(surface);

				SPF.FillRectangle(20, 20, 100, 100, 0.5f, 0.1f, 0.2f, 1f);
				SPF.FillRectangle(110, 110, 50, 20, 0.2f, 0.8f, 0.2f, 1f);
				SPF.DrawTexture(tex, (int)x, (int)y, 100, 100, 0, 0, tex.Width, tex.Height, false, false, 1, 1, 1, 0.5f);

				SPF.EndSurface();
				SPF.DrawTexture(surface.Texture, 0, 0, WindowWidth, WindowHeight, 0, 0, SurfaceWidth, SurfaceHeight, false, false, 1, 1, 1, 1);

				SPF.EndLoop();
			}
			SPF.Close();
		}
    }
}
