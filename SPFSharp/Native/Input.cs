using System;
using System.Runtime.InteropServices;
using System.Security;

namespace SPFSharp
{
    internal static partial class Native
    {
		public static class Input
		{
			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern bool SPF_IsButtonDown(int button);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern bool SPF_IsButtonPressed(int button);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern bool SPF_IsButtonReleased(int button);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern bool SPF_IsControllerConnected();

            [SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            public static extern bool SPF_IsCurrentInputLost();

            [SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern bool SPF_IsUsingController();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int SPF_GetControllerModel();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int SPF_GetMousePositionX();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int SPF_GetMousePositionY();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int SPF_GetMouseDeltaX();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int SPF_GetMouseDeltaY();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern bool SPF_IsMouseButtonDown(int button);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern bool SPF_IsMouseButtonPressed(int button);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern bool SPF_IsMouseButtonReleased(int button);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int SPF_GetMouseWheel();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern float SPF_GetLeftThumbstickX();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern float SPF_GetLeftThumbstickY();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern float SPF_GetRightThumbstickX();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern float SPF_GetRightThumbstickY();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern bool SPF_IsKeyDown(int key);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern bool SPF_IsKeyPressed(int key);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern bool SPF_IsKeyReleased(int key);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_Rumble(float duration, float lowIntensity, float highIntensity);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int SPF_LocalizeKey(int key);
		}
	}
}
