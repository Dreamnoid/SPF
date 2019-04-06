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

			using (var img = new SPF.Image(System.IO.File.ReadAllBytes("test.png")))
			{
				var rgba = img.GetPixel(93, 45);
				var pixel = Native.GetImagePixel(img.ID, 93, 45);
				Console.WriteLine(pixel + " = " + rgba.R + ";" + rgba.G + ";" + rgba.B + ";" + rgba.A);
			}
			Console.ReadKey();

			SPF.Close();
		}
    }
}
