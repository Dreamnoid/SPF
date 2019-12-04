#pragma once
#include <Core.h>

namespace SPF
{
	enum class Key : int
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
	};

	enum class Button : int
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
		RightShoulder = 11,
	};

	enum class MouseButton : int
	{
		Left = 0,
		Right = 1
	};

	namespace Input
	{
		void Init(const OpaquePointer window);
		void Update();
		void HandleEvent(const OpaquePointer evtPtr);
		void Dispose();

		DLLExport bool IsKeyDown(Key key);
		DLLExport bool IsKeyPressed(Key key);
		DLLExport bool IsKeyReleased(Key key);

		DLLExport bool IsControllerConnected();
		DLLExport bool IsButtonDown(Button button);
		DLLExport bool IsButtonPressed(Button button);
		DLLExport bool IsButtonReleased(Button button);
		DLLExport float GetLeftThumbstickX();
		DLLExport float GetLeftThumbstickY();
		DLLExport float GetRightThumbstickX();
		DLLExport float GetRightThumbstickY();

		DLLExport int GetMousePositionX();
		DLLExport int GetMousePositionY();
		DLLExport int GetMouseDeltaX();
		DLLExport int GetMouseDeltaY();
		DLLExport bool IsMouseButtonDown(MouseButton button);
		DLLExport bool IsMouseButtonPressed(MouseButton button);
		DLLExport bool IsMouseButtonReleased(MouseButton button);
		DLLExport void SetRelativeMouseState(bool state);
		DLLExport int GetMouseWheel();

		DLLExport void StartTextInput();
		DLLExport void StopTextInput();
		DLLExport const char* GetTextInput();
	}
}