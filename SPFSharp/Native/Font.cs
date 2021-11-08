using System;
using System.Runtime.InteropServices;
using System.Security;

namespace SPFSharp
{
    internal static partial class Native
	{
		public static class Font
		{
			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern Int32 SPF_LoadFont(IntPtr buffer, int width, int height, int fontSize, int firstCharacter, int lastCharacter, IntPtr glyphes);
		}
	}
}