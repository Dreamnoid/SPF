using System;
using System.Runtime.InteropServices;
using System.Security;

namespace SPFSharp
{
	internal static partial class Native
	{
		public static class Window
		{
			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_Open(string title, int w, int h);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern bool SPF_BeginLoop(out float dt);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_EndLoop();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_Close();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_SetFullscreen(bool fullscreen);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_SetWindowSize(int w, int h);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_SetWindowTitle(string title);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int SPF_GetWindowWidth();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int SPF_GetWindowHeight();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern bool SPF_HasFocus();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_SaveScreenshot(string filename);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_SetClipboard(string text);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr SPF_GetClipboard();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_GetDesktopSize(out int w, out int h);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_ShowMessageBox(string title, string message, bool isError);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr SPF_OpenFileDialog(string defaultFolder, string filter);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr SPF_SaveFileDialog(string defaultFolder, string filter);
		}
	}
}