using System.Runtime.InteropServices;
using System.Security;

namespace SPFSharp
{
	internal static partial class Native
	{
		public static class Renderer
		{
			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_Begin(int surface, bool clear);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_SetBlending(int blending);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_FillRectangle(int x, int y, int w, int h, byte r, byte g, byte b, byte a);

            [SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            public static extern void SPF_DrawTexture(int texture,
                float aX, float aY,
                float bX, float bY,
                float cX, float cY,
                float dX, float dY,
                int sourceX, int sourceY, int sourceW, int sourceH,
                bool flipX, bool flipY,
                float aColorR, float aColorG, float aColorB, float aColorA,
                float bColorR, float bColorG, float bColorB, float bColorA,
                float cColorR, float cColorG, float cColorB, float cColorA,
                float dColorR, float dColorG, float dColorB, float dColorA,
                float overlayR, float overlayG, float ovelayB, float overlayA);

            [SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            public static extern void SPF_DrawLine(float fromX, float fromY, float toX, float toY, byte r, byte g, byte b, byte a);
        }
	}
}