using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SPFSharp
{
    public static class Program
    {
        const int WindowWidth = 640;
        const int WindowHeight = 480;

        const int SurfaceWidth = 320;
        const int SurfaceHeight = 240;

		public static void Main()
		{
			using (SPF.Open("Test", WindowWidth, WindowHeight))
			{
				//using (var surface = new SPF.Surface(SurfaceWidth, SurfaceHeight))
				//using (var music = new SPF.Music(SPF.IO.ReadFile("boss.ogg")))
				{
					//SPF.Audio.PlayMusic(music);

					using (var tex = new SPF.Texture(SPF.IO.ReadFile("test.png")))
					{
						while (SPF.BeginLoop(out float dt))
						{
							//SPF.Renderer.Begin(surface, true);
							//SPF.Renderer.FillRectangle(0, 0, SurfaceWidth, SurfaceHeight, new System.Numerics.Vector4(1, 0, 0, 1));

							//SPF.Renderer.Begin(null, true);

							//SPF.Renderer.FillRectangle(10, 10, WindowWidth-20, WindowHeight-20, new System.Numerics.Vector4(1, 1, 0, 1));
                            SPF.Renderer.DrawTexture(tex, 100, 100);
							SPF.Renderer.DrawTexture(tex, 120, 120);
							//SPF.Renderer.DrawTexture(surface.Texture, 10, 10);

							SPF.EndLoop();
						}
					}
				}
			}
		}
    }
}
