using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SPFSharp
{
    public static class Program
    {
        public static void Main()
        {
            Native.Open("Test", 320, 240);
            var tex = Native.LoadTexture("D:/Test/test.png");

            float x = 0, y = 0;

            float dt;
            while (Native.BeginLoop(out dt))
            {
                if (Native.IsKeyDown(Native.KeyDown))
                {
                    y += 100 * dt;
                }
                else if (Native.IsKeyDown(Native.KeyUp))
                {
                    y -= 100 * dt;
                }
                if (Native.IsKeyDown(Native.KeyLeft))
                {
                    x -= 100 * dt;
                }
                else if (Native.IsKeyDown(Native.KeyRight))
                {
                    x += 100 * dt;
                }

                Native.FillRectangle(20, 20, 100, 100, 0.5f, 0.1f, 0.2f, 1f);
                Native.FillRectangle(110, 110, 50, 20, 0.2f, 0.8f, 0.2f, 1f);
                Native.DrawTexture(tex, (int)x, (int)y, 100, 100, 1, 1, 1, 0.5f);

                Native.EndLoop();
            }
            Native.DeleteTexture(tex);
            Native.Close();
        }
    }
}
