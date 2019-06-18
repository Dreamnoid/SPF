#pragma once
#include "Core.h"

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

	constexpr int ModifiersCount = 3;
	constexpr int SDL_NumScanCodes = 512; // TODO: dynamic allocation of these arrays to keep the SDL constants?
	constexpr int SDL_ControllerButtonMax = 15;

	class Input
	{
	private:
		OpaquePointer mController;
		int mKeysDownPreviousFrame[SDL_NumScanCodes + ModifiersCount];
		int mKeysDown[SDL_NumScanCodes + ModifiersCount];

		int mButtonsDownPreviousFrame[SDL_ControllerButtonMax];
		int mButtonsDown[SDL_ControllerButtonMax];

		int mMouseX;
		int mMouseY;
		int mMouseDeltaX;
		int mMouseDeltaY;
		unsigned int mCurrentMouseState;
		unsigned int mPreviousMouseState;
		bool mRelativeMode;

		void SearchGamepad();

	public:
		void Init();
		void Update();
		void HandleEvent(const OpaquePointer evtPtr);
		void Dispose();

		bool IsKeyDown(Key key);
		bool IsKeyPressed(Key key);
		bool IsKeyReleased(Key key);
		bool IsControllerConnected();
		bool IsButtonDown(Button button);
		bool IsButtonPressed(Button button);
		bool IsButtonReleased(Button button);
		int GetMousePositionX() const;
		int GetMousePositionY() const;
		int GetMouseDeltaX() const;
		int GetMouseDeltaY() const;
		bool IsMouseButtonDown(MouseButton button);
		bool IsMouseButtonPressed(MouseButton button);
		bool IsMouseButtonReleased(MouseButton button);
		void SetRelativeMouseState(bool state);

		float GetLeftThumbstickX();
		float GetLeftThumbstickY();
	};

	extern Input mInput;
}