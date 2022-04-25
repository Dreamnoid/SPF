using System;
using System.Runtime.InteropServices;
using System.Security;

namespace SPFSharp
{
	internal static partial class Native
	{
#if LINUX
		const string DLLName = "SPF.so";
#else
		const string DLLName = "SPF.dll";
#endif

		[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_FatalError(string msg);

        [SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 SPF_LoadMesh([In] SPF.Vertex[] vertices, int count);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DeleteMesh(Int32 mesh);

		[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_CreateShader(string sourceCode);

		[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DeleteShader(int id);
	}
}
