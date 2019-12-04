using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

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
			Z = 7,
			X = 8,
			C = 9,
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
			Right = 1
		}

		public static class Input
		{
            private static int _mousePositionX, _mousePositionY;

            internal static void Update()
            {
                _mousePositionX = Native.SPF_GetMousePositionX();
                _mousePositionY = Native.SPF_GetMousePositionY();
            }

            public static bool IsKeyDown(Key key)
			{
				return Native.SPF_IsKeyDown((int)key);
			}

			public static bool IsKeyPressed(Key key)
			{
				return Native.SPF_IsKeyPressed((int)key);
			}

			public static bool IsKeyReleased(Key key)
			{
				return Native.SPF_IsKeyReleased((int)key);
			}

			public static bool IsButtonDown(Button button)
			{
				return Native.SPF_IsButtonDown((int)button);
			}

			public static bool IsButtonPressed(Button button)
			{
				return Native.SPF_IsButtonPressed((int)button);
			}

			public static bool IsButtonReleased(Button button)
			{
				return Native.SPF_IsButtonReleased((int)button);
			}

			public static int GetMousePositionX()
            {
                return _mousePositionX;
            }

			public static int GetMousePositionY()
            {
                return _mousePositionY;
            }

            public static int GetMouseDeltaX()
			{
				return Native.SPF_GetMouseDeltaX();
			}

			public static int GetMouseDeltaY()
			{
				return Native.SPF_GetMouseDeltaY();
			}

			public static bool IsMouseButtonDown(MouseButton button)
			{
				return Native.SPF_IsMouseButtonDown((int)button);
			}

			public static bool IsMouseButtonPressed(MouseButton button)
			{
				return Native.SPF_IsMouseButtonPressed((int)button);
			}

			public static bool IsMouseButtonReleased(MouseButton button)
			{
				return Native.SPF_IsMouseButtonReleased((int)button);
			}

            public static int GetMouseWheel() => Native.SPF_GetMouseWheel();

			public static bool IsControllerConnected()
			{
				return Native.SPF_IsControllerConnected();
			}

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

			public static float GetLeftThumbstickX()
			{
				return Native.SPF_GetLeftThumbstickX();
			}

			public static float GetLeftThumbstickY()
			{
				return Native.SPF_GetLeftThumbstickY();
			}

			public static float GetRightThumbstickX()
			{
				return Native.SPF_GetRightThumbstickX();
			}

			public static float GetRightThumbstickY()
			{
				return Native.SPF_GetRightThumbstickY();
			}

			public static void SetRelativeMouseState(bool state)
			{
				Native.SPF_SetRelativeMouseState(state);
			}

			public static void StartTextInput() => Native.SPF_StartTextInput();

			public static void StopTextInput() => Native.SPF_StopTextInput();

			public static string GetTextInput() => Marshal.PtrToStringAnsi(Native.SPF_GetTextInput());
		}
	}
}
