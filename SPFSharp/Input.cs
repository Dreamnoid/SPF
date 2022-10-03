using System.Numerics;
using System.Runtime.InteropServices;

namespace SPFSharp
{
    public static partial class SPF
	{
		public enum Key : int
		{
			Up = 0,
			Down = 1,
			Right = 2,
			Left = 3,
			Space = 4,
			Escape = 5,
			Delete = 6,
			Control = 10,
			Shift = 11,
			Alt = 12,
			Num0 = 13,
			Num1 = 14,
			Num2 = 15,
			Num3 = 16,
			Num4 = 17,
			Num5 = 18,
			Num6 = 19,
			Num7 = 20,
			Num8 = 21,
			Num9 = 22,
			Return = 23,
			A,
			B,
			C,
			D,
			E,
			F,
			G,
			H,
			I,
			J,
			K,
			L,
			M,
			N,
			O,
			P,
			Q,
			R,
			S,
			T,
			U,
			V,
			W,
			X,
			Y,
			Z,
			F1,
			F2,
			F3,
			F4,
			F5,
			F6,
			F7,
			F8,
			F9,
			F10,
			F11,
			F12,
			Tab,
			PrintScreen
		}

		public enum Button : int
		{
			A = 0,
			B = 1,
			X = 2,
			Y = 3,
			Start = 4,
			Select = 5,
			DPadUp = 6,
			DPadDown = 7,
			DPadRight = 8,
			DPadLeft = 9,
			LeftShoulder = 10,
			RightShoulder = 11
		}

		public enum MouseButton : int
		{
			Left = 0,
			Right = 1,
			Middle = 2
		}

		public static class Input
		{
            private static int _mousePositionX, _mousePositionY;

            internal static void Update()
            {
                _mousePositionX = Native.Input.SPF_GetMousePositionX();
                _mousePositionY = Native.Input.SPF_GetMousePositionY();
            }

            public static bool IsKeyDown(Key key) => Native.Input.SPF_IsKeyDown((int)key);

			public static bool IsKeyPressed(Key key) => Native.Input.SPF_IsKeyPressed((int)key);

			public static bool IsKeyReleased(Key key) => Native.Input.SPF_IsKeyReleased((int)key);

			public static bool IsButtonDown(Button button) => Native.Input.SPF_IsButtonDown((int)button);

			public static bool IsButtonPressed(Button button) => Native.Input.SPF_IsButtonPressed((int)button);

			public static bool IsButtonReleased(Button button) => Native.Input.SPF_IsButtonReleased((int)button);
			
			public static int GetMousePositionX() => _mousePositionX;

			public static int GetMousePositionY() => _mousePositionY;

            public static int GetMouseDeltaX() => Native.Input.SPF_GetMouseDeltaX();

			public static int GetMouseDeltaY() => Native.Input.SPF_GetMouseDeltaY();

			public static bool IsMouseButtonDown(MouseButton button) => Native.Input.SPF_IsMouseButtonDown((int)button);

			public static bool IsMouseButtonPressed(MouseButton button) => Native.Input.SPF_IsMouseButtonPressed((int)button);

			public static bool IsMouseButtonReleased(MouseButton button) => Native.Input.SPF_IsMouseButtonReleased((int)button);

            public static int GetMouseWheel() => Native.Input.SPF_GetMouseWheel();

			public static bool IsControllerConnected() => Native.Input.SPF_IsControllerConnected();

			public static float Deadzone = 0.05f;

			public static Vector2 GetLeftThumbstick()
			{
				var direction = new Vector2(GetLeftThumbstickX(), GetLeftThumbstickY());
				if (direction.LengthSquared() < Deadzone)
				{
					direction = Vector2.Zero;
				}
				return direction;
			}

			public static Vector2 GetRightThumbstick()
			{
				var direction = new Vector2(GetRightThumbstickX(), GetRightThumbstickY());
				if (direction.LengthSquared() < Deadzone)
				{
					direction = Vector2.Zero;
				}
				return direction;
			}

			public static float GetLeftThumbstickX() => Native.Input.SPF_GetLeftThumbstickX();

			public static float GetLeftThumbstickY() => Native.Input.SPF_GetLeftThumbstickY();

			public static float GetRightThumbstickX() => Native.Input.SPF_GetRightThumbstickX();

			public static float GetRightThumbstickY() => Native.Input.SPF_GetRightThumbstickY();

			public static void SetRelativeMouseState(bool state) => Native.Input.SPF_SetRelativeMouseState(state);

			public static void StartTextInput() => Native.Input.SPF_StartTextInput();

			public static void StopTextInput() => Native.Input.SPF_StopTextInput();

			public static string GetTextInput() => Marshal.PtrToStringAnsi(Native.Input.SPF_GetTextInput());

			public static void Rumble(float duration, float lowIntensity = 0.5f, float highIntensity = 0.5f) 
				=> Native.Input.SPF_Rumble(duration, lowIntensity, highIntensity);
		}
	}
}
