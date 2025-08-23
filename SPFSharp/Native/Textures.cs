using System;
using System.Runtime.InteropServices;
using System.Security;

namespace SPFSharp
{
	internal static partial class Native
	{
		public static class Textures
		{
			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern Int32 SPF_LoadTexture(IntPtr buffer, int length);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern Int32 SPF_LoadCubemap(IntPtr buffer, int length);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int SPF_CreateEmptyTexture(uint w, uint h, int flags);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_DeleteTexture(Int32 texture);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int SPF_GetTextureWidth(Int32 texture);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int SPF_GetTextureHeight(Int32 texture);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_SetTextureFiltering(Int32 texture, bool filtering);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_GenerateTextureMipmaps(Int32 texture);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern bool SPF_IsTextureFlipped(Int32 texture);
		}
	}
}