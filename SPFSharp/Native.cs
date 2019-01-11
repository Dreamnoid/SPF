using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace SPFSharp
{
    internal static class Native
    {
        const string DLLName = "SPF.dll";

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Open(string title, int w, int h);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool BeginLoop(out float dt);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void EndLoop();

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Close();

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int LoadTexture(string filename);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void DeleteTexture(int texture);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern uint CompileShader(string fragmentCode, string vertexCode);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void DeleteShader(uint id);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void DrawTexture(int tex, int x, int y, int w, int h, float r, float g, float b, float a);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool IsKeyDown(int key);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool IsKeyPressed(int key);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool IsKeyReleased(int key);

        public static int KeyUp = 0;
        public static int KeyDown = 1;
        public static int KeyRight = 2;
        public static int KeyLeft = 3;
        public static int KeySpace = 4;
        public static int KeyEscape = 5;

    }
}
