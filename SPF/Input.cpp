#include "Input.h"
#include <SDL.h>
#include <cstring>

Input mInput;

SDL_Scancode TranslateKey(Key key)
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
}

void Input::Update()
{
	memcpy(mKeysDownPreviousFrame, mKeysDown, sizeof(mKeysDownPreviousFrame));
	memcpy(mButtonsDownPreviousFrame, mButtonsDown, sizeof(mButtonsDownPreviousFrame));

	mPreviousMouseState = mCurrentMouseState;
	mCurrentMouseState = SDL_GetMouseState(&mMouseX, &mMouseY);
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
	SDL_Scancode code = TranslateKey(key);
	return !mKeysDownPreviousFrame[code] && mKeysDown[code];
}

bool Input::IsKeyReleased(Key key)
{
	SDL_Scancode code = TranslateKey(key);
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

int Input::GetMousePositionX()
{
	return mMouseX;
}

int Input::GetMousePositionY()
{
	return mMouseY;
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
}