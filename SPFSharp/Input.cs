﻿using System;
using System.Collections.Generic;
using System.Linq;
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
			public static bool IsKeyDown(Key key)
			{
				return Native.IsKeyDown((int)key);
			}

			public static bool IsKeyPressed(Key key)
			{
				return Native.IsKeyPressed((int)key);
			}

			public static bool IsKeyReleased(Key key)
			{
				return Native.IsKeyReleased((int)key);
			}

			public static bool IsButtonDown(Button button)
			{
				return Native.IsButtonDown((int)button);
			}

			public static bool IsButtonPressed(Button button)
			{
				return Native.IsButtonPressed((int)button);
			}

			public static bool IsButtonReleased(Button button)
			{
				return Native.IsButtonReleased((int)button);
			}

			public static int GetMousePositionX()
			{
				return Native.GetMousePositionX();
			}

			public static int GetMousePositionY()
			{
				return Native.GetMousePositionY();
			}

			public static int GetMouseDeltaX()
			{
				return Native.GetMouseDeltaX();
			}

			public static int GetMouseDeltaY()
			{
				return Native.GetMouseDeltaY();
			}

			public static bool IsMouseButtonDown(MouseButton button)
			{
				return Native.IsMouseButtonDown((int)button);
			}

			public static bool IsMouseButtonPressed(MouseButton button)
			{
				return Native.IsMouseButtonPressed((int)button);
			}

			public static bool IsMouseButtonReleased(MouseButton button)
			{
				return Native.IsMouseButtonReleased((int)button);
			}

			public static bool IsControllerConnected()
			{
				return Native.IsControllerConnected();
			}

			public static float GetLeftThumbstickX()
			{
				return Native.GetLeftThumbstickX();
			}

			public static float GetLeftThumbstickY()
			{
				return Native.GetLeftThumbstickY();
			}

			public static void SetRelativeMouseState(bool state)
			{
				Native.SetRelativeMouseState(state);
			}
		}
	}
}
