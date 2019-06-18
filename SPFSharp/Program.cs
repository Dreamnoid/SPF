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
			using (SPF.Open("Test", WindowWidth, WindowHeight))
			{
				using (var music = new SPF.Music(SPF.IO.ReadFile("boss.ogg")))
				{
					SPF.Audio.PlayMusic(music);

					using (var tex = new SPF.Texture(SPF.IO.ReadFile("test.png")))
					{
						while (SPF.BeginLoop(out float dt))
						{
							SPF.Renderer.DrawTexture(tex, 0, 0);
							SPF.EndLoop();
						}
					}
				}
			}
		}
    }
}
