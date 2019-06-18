#include "Input.h"
#include <SDL.h>
#include <cstring>

Input mInput;

int TranslateKey(Key key)
{
	switch (key)
	{
	case Key::Up: return SDL_SCANCODE_UP;
	case Key::Down: return SDL_SCANCODE_DOWN;
	case Key::Left: return SDL_SCANCODE_LEFT;
	case Key::Right: return SDL_SCANCODE_RIGHT;
	case Key::Space: return SDL_SCANCODE_SPACE;
	case Key::Escape: return SDL_SCANCODE_ESCAPE;
	case Key::Delete: return SDL_SCANCODE_DELETE;
	case Key::Z: return SDL_SCANCODE_Z;
	case Key::X: return SDL_SCANCODE_X;
	case Key::C: return SDL_SCANCODE_C;
	case Key::Num0: return SDL_SCANCODE_KP_0;
	case Key::Num1: return SDL_SCANCODE_KP_1;
	case Key::Num2: return SDL_SCANCODE_KP_2;
	case Key::Num3: return SDL_SCANCODE_KP_3;
	case Key::Num4: return SDL_SCANCODE_KP_4;
	case Key::Num5: return SDL_SCANCODE_KP_5;
	case Key::Num6: return SDL_SCANCODE_KP_6;
	case Key::Num7: return SDL_SCANCODE_KP_7;
	case Key::Num8: return SDL_SCANCODE_KP_8;
	case Key::Num9: return SDL_SCANCODE_KP_9;
	case Key::Control: return (SDL_NUM_SCANCODES + 0);
	case Key::Shift: return (SDL_NUM_SCANCODES + 1);
	case Key::Alt: return (SDL_NUM_SCANCODES + 2);
	default: return SDL_SCANCODE_RETURN;
	}
}


SDL_GameControllerButton TranslateButton(Button button)
{
	switch (button)
	{
	case Button::A: return SDL_CONTROLLER_BUTTON_A;
	case Button::B: return SDL_CONTROLLER_BUTTON_B;
	case Button::X: return SDL_CONTROLLER_BUTTON_X;
	case Button::Y: return SDL_CONTROLLER_BUTTON_Y;
	case Button::Start: return SDL_CONTROLLER_BUTTON_START;
	case Button::Select: return SDL_CONTROLLER_BUTTON_BACK;
	case Button::DPadUp: return SDL_CONTROLLER_BUTTON_DPAD_UP;
	case Button::DPadDown: return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
	case Button::DPadRight: return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
	case Button::DPadLeft: return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
	case Button::LeftShoulder: return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	case Button::RightShoulder: return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
	default: return SDL_CONTROLLER_BUTTON_A;
	}
}

unsigned int TranslateMouseButton(MouseButton button)
{
	switch (button)
	{
	case MouseButton::Left: return SDL_BUTTON_LEFT;
	case MouseButton::Right: return SDL_BUTTON_RIGHT;
	default: return SDL_BUTTON_LEFT;
	}
}

void Input::SearchGamepad()
{
	mController = nullptr;
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		if (SDL_IsGameController(i))
		{
			mController = SDL_GameControllerOpen(i);
			if (mController)
			{
				break;
			}
		}
	}
}

void Input::Init()
{
	SDL_GameControllerEventState(SDL_ENABLE);
	SearchGamepad();
	mRelativeMode = false;
}

void Input::Update()
{
	memcpy(mKeysDownPreviousFrame, mKeysDown, sizeof(mKeysDownPreviousFrame));
	memcpy(mButtonsDownPreviousFrame, mButtonsDown, sizeof(mButtonsDownPreviousFrame));

	int previousMouseX = mMouseX;
	int previousMouseY = mMouseY;
	mPreviousMouseState = mCurrentMouseState;
	mCurrentMouseState = SDL_GetMouseState(&mMouseX, &mMouseY);
	if (mRelativeMode)
	{
		SDL_GetRelativeMouseState(&mMouseDeltaX, &mMouseDeltaY);
	}
	else
	{
		mMouseDeltaX = mMouseX - previousMouseX;
		mMouseDeltaY = mMouseY - previousMouseY;
	}

	SDL_Keymod modifiers = SDL_GetModState();
	mKeysDown[TranslateKey(Key::Control)] = ((modifiers & KMOD_CTRL) != 0);
	mKeysDown[TranslateKey(Key::Shift)] = ((modifiers & KMOD_SHIFT) != 0);
	mKeysDown[TranslateKey(Key::Alt)] = ((modifiers & KMOD_ALT) != 0);
}

void Input::HandleEvent(const SDL_Event& evt)
{
	if (evt.type == SDL_KEYDOWN)
	{
		mKeysDown[evt.key.keysym.scancode] = 1;
	}
	if (evt.type == SDL_KEYUP)
	{
		mKeysDown[evt.key.keysym.scancode] = 0;
	}
	if (evt.type == SDL_CONTROLLERBUTTONDOWN)
	{
		mButtonsDown[evt.cbutton.button] = 1;
	}
	if (evt.type == SDL_CONTROLLERBUTTONUP)
	{
		mButtonsDown[evt.cbutton.button] = 0;
	}
	if (evt.type == SDL_CONTROLLERDEVICEADDED || evt.type == SDL_CONTROLLERDEVICEREMOVED)
	{
		SearchGamepad();
		if (evt.type == SDL_CONTROLLERDEVICEREMOVED)
		{
			memset(&mButtonsDown, 0, sizeof(mButtonsDown)); // All buttons released
		}
	}
}

void Input::Dispose ()
{
	if (mController)
	{
		SDL_GameControllerClose(mController);
	}
}

bool Input::IsKeyDown(Key key)
{
	return mKeysDown[TranslateKey(key)];
}

bool Input::IsKeyPressed(Key key)
{
	int code = TranslateKey(key);
	return !mKeysDownPreviousFrame[code] && mKeysDown[code];
}

bool Input::IsKeyReleased(Key key)
{
	int code = TranslateKey(key);
	return mKeysDownPreviousFrame[code] && !mKeysDown[code];
}

bool Input::IsControllerConnected()
{
	return mController;
}

bool Input::IsButtonDown(Button button)
{
	return mButtonsDown[TranslateButton(button)];
}

bool Input::IsButtonPressed(Button button)
{
	SDL_GameControllerButton code = TranslateButton(button);
	return !mButtonsDownPreviousFrame[code] && mButtonsDown[code];
}

bool Input::IsButtonReleased(Button button)
{
	SDL_GameControllerButton code = TranslateButton(button);
	return mButtonsDownPreviousFrame[code] && !mButtonsDown[code];
}

int Input::GetMousePositionX() const
{
	return mMouseX;
}

int Input::GetMousePositionY() const
{
	return mMouseY;
}

int Input::GetMouseDeltaX() const
{
	return mMouseDeltaX;
}

int Input::GetMouseDeltaY() const
{
	return mMouseDeltaY;
}

bool Input::IsMouseButtonDown(MouseButton button)
{
	return (mCurrentMouseState & SDL_BUTTON(TranslateMouseButton(button)));
}

bool Input::IsMouseButtonPressed(MouseButton button)
{
	auto mask = SDL_BUTTON(TranslateMouseButton(button));
	return ((mCurrentMouseState & mask) && !(mPreviousMouseState & mask));
}

bool Input::IsMouseButtonReleased(MouseButton button)
{
	auto mask = SDL_BUTTON(TranslateMouseButton(button));
	return (!(mCurrentMouseState & mask) && (mPreviousMouseState & mask));
}

constexpr float ThumbstickDeadzoneRatio = 0.1f;

float NormalizeThumbstick(Sint16 rawValue)
{
	float normalizedValue = rawValue / 32767;
	/*if (fabs(normalizedValue) <= ThumbstickDeadzoneRatio)
	{
		normalizedValue = 0.f;
	}*/
	return normalizedValue;
}

float Input::GetLeftThumbstickX()
{
	if (mController)
	{
		return NormalizeThumbstick(SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_LEFTX));
	}
	else return 0.f;
}

float Input::GetLeftThumbstickY()
{
	if (mController)
	{
		return NormalizeThumbstick(SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_LEFTY));
	}
	else return 0.f;
}

void Input::SetRelativeMouseState(bool state)
{
	mRelativeMode = state;
	SDL_SetRelativeMouseMode(state ? SDL_TRUE : SDL_FALSE);
}

extern "C"
{

	DLLExport int IsKeyDown(Key key)
	{
		return mInput.IsKeyDown(key) ? 1 : 0;
	}

	DLLExport int IsKeyPressed(Key key)
	{
		return mInput.IsKeyPressed(key) ? 1 : 0;
	}

	DLLExport int IsKeyReleased(Key key)
	{
		return mInput.IsKeyReleased(key) ? 1 : 0;
	}

	DLLExport int IsControllerConnected()
	{
		return mInput.IsControllerConnected() ? 1 : 0;
	}

	DLLExport int IsButtonDown(Button button)
	{
		return mInput.IsButtonDown(button) ? 1 : 0;
	}

	DLLExport int IsButtonPressed(Button button)
	{
		return mInput.IsButtonPressed(button) ? 1 : 0;
	}

	DLLExport int IsButtonReleased(Button button)
	{
		return mInput.IsButtonReleased(button) ? 1 : 0;
	}

	DLLExport int GetMousePositionX()
	{
		return mInput.GetMousePositionX();
	}

	DLLExport int GetMousePositionY()
	{
		return mInput.GetMousePositionY();
	}

	DLLExport int GetMouseDeltaX()
	{
		return mInput.GetMouseDeltaX();
	}

	DLLExport int GetMouseDeltaY()
	{
		return mInput.GetMouseDeltaY();
	}

	DLLExport int IsMouseButtonDown(MouseButton button)
	{
		return mInput.IsMouseButtonDown(button) ? 1 : 0;
	}

	DLLExport int IsMouseButtonPressed(MouseButton button)
	{
		return mInput.IsMouseButtonPressed(button) ? 1 : 0;
	}

	DLLExport int IsMouseButtonReleased(MouseButton button)
	{
		return mInput.IsMouseButtonReleased(button) ? 1 : 0;
	}

	DLLExport float GetLeftThumbstickX()
	{
		return mInput.GetLeftThumbstickX();
	}

	DLLExport float GetLeftThumbstickY()
	{
		return mInput.GetLeftThumbstickY();
	}

	DLLExport void SetRelativeMouseState(bool state)
	{
		mInput.SetRelativeMouseState(state);
	}
}
