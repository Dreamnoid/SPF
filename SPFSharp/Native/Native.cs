using System;
using System.Runtime.InteropServices;
using System.Security;

namespace SPFSharp
{
	internal static partial class Native
	{
#if LINUX
		const string DLLName = "SPF3.so";
#else
		const string DLLName = "SPF3.dll";
#endif

		[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_FatalError(string msg);

        [SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SPF_UnlockAchievement(string id);
    }
}
