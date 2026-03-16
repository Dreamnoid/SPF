using SPFSharp;

namespace Sample
{
    public static class Program
    {
        const int WindowWidth = 1280;
        const int WindowHeight = 720;

		public static void Main()
		{
			using (SPF.Open("Test", WindowWidth, WindowHeight))
			{
				using (var surface = new SPF.Surface(320, 180))
				//using (var music = new SPF.Music(SPF.IO.ReadFile("boss.ogg")))
				{
					//SPF.Audio.PlayMusic(music);

					using (var tex = new SPF.Texture(SPF.IO.ReadFile("test.png")))
					{
						tex.SetFiltering(false);
						while (SPF.BeginLoop(out float dt))
						{
							SPF.Renderer.Begin(surface, true);
							SPF.Renderer.FillRectangle(0, 0, surface.Width, surface.Height, new System.Numerics.Vector4(1, 0, 0, 1));

                            SPF.Renderer.FillRectangle(10, 10, 24, 24, new System.Numerics.Vector4(1f, 0, 0, 1f));

							//SPF.Renderer.FillRectangle(10, 10, WindowWidth-20, WindowHeight-20, new System.Numerics.Vector4(1, 1, 0, 1));
                            SPF.Renderer.DrawTexture(tex, 100, 100);
							//SPF.Renderer.DrawTexture(tex, 120, 120);
							//SPF.Renderer.DrawTexture(surface.Texture, 10, 10);

							SPF.Renderer.DrawTexture(tex, 120, 120, (int)(tex.Width * 1.5f), (int)(tex.Height * 1.2f), 0, 0, tex.Width, tex.Height, false, false, System.Numerics.Vector4.One);

							SPF.Renderer.Begin(null, true);
							SPF.Renderer.DrawTexture(surface.Texture, 32, 32);

							SPF.EndLoop();
						}
					}
				}
			}
		}
    }
}
