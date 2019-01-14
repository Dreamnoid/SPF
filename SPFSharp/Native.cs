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
        public static extern UInt32 LoadTexture(string filename);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void DeleteTexture(UInt32 texture);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetTextureWidth(UInt32 texture);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetTextureHeight(UInt32 texture);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void DrawTexture(UInt32 tex, int x, int y, int w, int h, int srcx, int srcy, int srcw, int srch, bool flipx, bool flipy, float r, float g, float b, float a);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool IsKeyDown(int key);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool IsKeyPressed(int key);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool IsKeyReleased(int key);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 CreateSurface(int w, int h);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void BeginSurface(UInt32 surface);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void EndSurface();

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void DeleteSurface(UInt32 surface);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 GetSurfaceTexture(UInt32 surface);
    }
}
