#pragma once

typedef enum
{
	Key_Up = 0,
	Key_Down = 1,
	Key_Right = 2,
	Key_Left = 3,
	Key_Space = 4,
	Key_Escape = 5,
	Key_Delete = 6,
	Key_Z = 7,
	Key_X = 8,
	Key_C = 9,
} Key;

SDL_Scancode TranslateKey(Key key)
{
	switch (key)
	{
		case Key_Up: return SDL_SCANCODE_UP;
		case Key_Down: return SDL_SCANCODE_DOWN;
		case Key_Left: return SDL_SCANCODE_LEFT;
		case Key_Right: return SDL_SCANCODE_RIGHT;
		case Key_Space: return SDL_SCANCODE_SPACE;
		case Key_Escape: return SDL_SCANCODE_ESCAPE;
		case Key_Delete: return SDL_SCANCODE_DELETE;
		case Key_Z: return SDL_SCANCODE_Z;
		case Key_X: return SDL_SCANCODE_X;
		case Key_C: return SDL_SCANCODE_C;
		default: return SDL_SCANCODE_RETURN;
	}
}

DLLExport bool IsKeyDown(Key key)
{
	return Data.KeysDown[TranslateKey(key)];
}

DLLExport bool IsKeyPressed(Key key)
{
	SDL_Scancode code = TranslateKey(key);
	return !Data.KeysDownPreviousFrame[code] && Data.KeysDown[code];
}

DLLExport bool IsKeyReleased(Key key)
{
	SDL_Scancode code = TranslateKey(key);
	return Data.KeysDownPreviousFrame[code] && !Data.KeysDown[code];
}

void InitInput()
{
	Data.Controller = NULL;
	for (int i = 0; i < SDL_NumJoysticks(); ++i) 
	{
		if (SDL_IsGameController(i)) 
		{
			Data.Controller = SDL_GameControllerOpen(i);
			if (Data.Controller)
			{
				break;
			}
		}
	}
}

void UpdateInput()
{
	memcpy(Data.KeysDownPreviousFrame, Data.KeysDown, sizeof(Data.KeysDownPreviousFrame));
	memcpy(Data.ButtonsDownPreviousFrame, Data.ButtonsDown, sizeof(Data.ButtonsDownPreviousFrame));

	Data.PreviousMouseState = Data.CurrentMouseState;
	Data.CurrentMouseState = SDL_GetMouseState(&Data.MouseX, &Data.MouseY);
}

void QuitInput()
{
	if (Data.Controller)
	{
		SDL_GameControllerClose(Data.Controller);
	}
}

typedef enum
{
	Button_A = 0,
	Button_B = 1,
	Button_X = 2,
	Button_Y = 3,
	Button_Start = 4,
	Button_Select = 5,
	Button_DPadUp = 6,
	Button_DPadDown = 7,
	Button_DPadRight = 8,
	Button_DPadLeft = 9,
	Button_LeftShoulder = 10,
	Button_RightShoulder = 11,
} Button;

typedef enum
{
	MouseButton_Left = 0,
	MouseButton_Right = 1
} MouseButton;

SDL_GameControllerButton TranslateButton(Button button)
{
	switch (button)
	{
	case Button_A: return SDL_CONTROLLER_BUTTON_A;
	case Button_B: return SDL_CONTROLLER_BUTTON_B;
	case Button_X: return SDL_CONTROLLER_BUTTON_X;
	case Button_Y: return SDL_CONTROLLER_BUTTON_Y;
	case Button_Start: return SDL_CONTROLLER_BUTTON_START;
	case Button_Select: return SDL_CONTROLLER_BUTTON_BACK;
	case Button_DPadUp: return SDL_CONTROLLER_BUTTON_DPAD_UP;
	case Button_DPadDown: return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
	case Button_DPadRight: return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
	case Button_DPadLeft: return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
	case Button_LeftShoulder: return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	case Button_RightShoulder: return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
	default: return SDL_CONTROLLER_BUTTON_A;
	}
}

DLLExport bool IsControllerConnected()
{
	return Data.Controller;
}

DLLExport bool IsButtonDown(Button button)
{
	return Data.ButtonsDown[TranslateButton(button)];
}

DLLExport bool IsButtonPressed(Button button)
{
	SDL_GameControllerButton code = TranslateButton(button);
	return !Data.ButtonsDownPreviousFrame[code] && Data.ButtonsDown[code];
}

DLLExport bool IsButtonReleased(Button button)
{
	SDL_GameControllerButton code = TranslateButton(button);
	return Data.ButtonsDownPreviousFrame[code] && !Data.ButtonsDown[code];
}

DLLExport int GetMousePositionX()
{
	return Data.MouseX;
}

DLLExport int GetMousePositionY()
{
	return Data.MouseY;
}


unsigned int TranslateMouseButton(MouseButton button)
{
	switch (button)
	{
	case MouseButton_Left: return SDL_BUTTON_LEFT;
	case MouseButton_Right: return SDL_BUTTON_RIGHT;
	default: return SDL_BUTTON_LEFT;
	}
}

DLLExport bool IsMouseButtonDown(MouseButton button) 
{
	return (Data.CurrentMouseState & SDL_BUTTON(TranslateMouseButton(button)));
}

DLLExport bool IsMouseButtonPressed(MouseButton button) 
{
	auto mask = SDL_BUTTON(TranslateMouseButton(button));
	return ((Data.CurrentMouseState & mask) && !(Data.PreviousMouseState & mask));
}

DLLExport bool IsMouseButtonReleased(MouseButton button) 
{
	auto mask = SDL_BUTTON(TranslateMouseButton(button));
	return (!(Data.CurrentMouseState & mask) && (Data.PreviousMouseState & mask));
}
