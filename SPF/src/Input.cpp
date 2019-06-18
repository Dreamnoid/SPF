#include <Input.h>
#include <SDL.h>
#include <cstring>

namespace SPF
{
	constexpr int ModifiersCount = 3;

	struct
	{
		SDL_GameController* Controller;
		int KeysDownPreviousFrame[SDL_NUM_SCANCODES + ModifiersCount];
		int KeysDown[SDL_NUM_SCANCODES + ModifiersCount];

		int ButtonsDownPreviousFrame[SDL_CONTROLLER_BUTTON_MAX];
		int ButtonsDown[SDL_CONTROLLER_BUTTON_MAX];

		int MouseX;
		int MouseY;
		int MouseDeltaX;
		int MouseDeltaY;
		unsigned int CurrentMouseState;
		unsigned int PreviousMouseState;
		bool RelativeMode = false;
	} InputData;

	namespace Input
	{

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

		void SearchGamepad()
		{
			InputData.Controller = nullptr;
			for (int i = 0; i < SDL_NumJoysticks(); ++i)
			{
				if (SDL_IsGameController(i))
				{
					InputData.Controller = SDL_GameControllerOpen(i);
					if (InputData.Controller)
					{
						break;
					}
				}
			}
		}

		void Init()
		{
			SDL_GameControllerEventState(SDL_ENABLE);
			SearchGamepad();
		}

		void Update()
		{
			memcpy(InputData.KeysDownPreviousFrame, InputData.KeysDown, sizeof(InputData.KeysDownPreviousFrame));
			memcpy(InputData.ButtonsDownPreviousFrame, InputData.ButtonsDown, sizeof(InputData.ButtonsDownPreviousFrame));

			int previousMouseX = InputData.MouseX;
			int previousMouseY = InputData.MouseY;
			InputData.PreviousMouseState = InputData.CurrentMouseState;
			InputData.CurrentMouseState = SDL_GetMouseState(&InputData.MouseX, &InputData.MouseY);
			if (InputData.RelativeMode)
			{
				SDL_GetRelativeMouseState(&InputData.MouseDeltaX, &InputData.MouseDeltaY);
			}
			else
			{
				InputData.MouseDeltaX = InputData.MouseX - previousMouseX;
				InputData.MouseDeltaY = InputData.MouseY - previousMouseY;
			}

			SDL_Keymod modifiers = SDL_GetModState();
			InputData.KeysDown[TranslateKey(Key::Control)] = ((modifiers & KMOD_CTRL) != 0);
			InputData.KeysDown[TranslateKey(Key::Shift)] = ((modifiers & KMOD_SHIFT) != 0);
			InputData.KeysDown[TranslateKey(Key::Alt)] = ((modifiers & KMOD_ALT) != 0);
		}

		void HandleEvent(const OpaquePointer evtPtr)
		{
			const SDL_Event& evt = *(SDL_Event*)evtPtr;
			if (evt.type == SDL_KEYDOWN)
			{
				InputData.KeysDown[evt.key.keysym.scancode] = 1;
			}
			if (evt.type == SDL_KEYUP)
			{
				InputData.KeysDown[evt.key.keysym.scancode] = 0;
			}
			if (evt.type == SDL_CONTROLLERBUTTONDOWN)
			{
				InputData.ButtonsDown[evt.cbutton.button] = 1;
			}
			if (evt.type == SDL_CONTROLLERBUTTONUP)
			{
				InputData.ButtonsDown[evt.cbutton.button] = 0;
			}
			if (evt.type == SDL_CONTROLLERDEVICEADDED || evt.type == SDL_CONTROLLERDEVICEREMOVED)
			{
				SearchGamepad();
				if (evt.type == SDL_CONTROLLERDEVICEREMOVED)
				{
					memset(&InputData.ButtonsDown, 0, sizeof(InputData.ButtonsDown)); // All buttons released
				}
			}
		}

		void Dispose()
		{
			if (InputData.Controller)
			{
				SDL_GameControllerClose(InputData.Controller);
			}
		}

		bool IsKeyDown(Key key)
		{
			return InputData.KeysDown[TranslateKey(key)];
		}

		bool IsKeyPressed(Key key)
		{
			int code = TranslateKey(key);
			return !InputData.KeysDownPreviousFrame[code] && InputData.KeysDown[code];
		}

		bool IsKeyReleased(Key key)
		{
			int code = TranslateKey(key);
			return InputData.KeysDownPreviousFrame[code] && !InputData.KeysDown[code];
		}

		bool IsControllerConnected()
		{
			return InputData.Controller;
		}

		bool IsButtonDown(Button button)
		{
			return InputData.ButtonsDown[TranslateButton(button)];
		}

		bool IsButtonPressed(Button button)
		{
			SDL_GameControllerButton code = TranslateButton(button);
			return !InputData.ButtonsDownPreviousFrame[code] && InputData.ButtonsDown[code];
		}

		bool IsButtonReleased(Button button)
		{
			SDL_GameControllerButton code = TranslateButton(button);
			return InputData.ButtonsDownPreviousFrame[code] && !InputData.ButtonsDown[code];
		}

		int GetMousePositionX()
		{
			return InputData.MouseX;
		}

		int GetMousePositionY()
		{
			return InputData.MouseY;
		}

		int GetMouseDeltaX()
		{
			return InputData.MouseDeltaX;
		}

		int GetMouseDeltaY()
		{
			return InputData.MouseDeltaY;
		}

		bool IsMouseButtonDown(MouseButton button)
		{
			return (InputData.CurrentMouseState & SDL_BUTTON(TranslateMouseButton(button)));
		}

		bool IsMouseButtonPressed(MouseButton button)
		{
			auto mask = SDL_BUTTON(TranslateMouseButton(button));
			return ((InputData.CurrentMouseState & mask) && !(InputData.PreviousMouseState & mask));
		}

		bool IsMouseButtonReleased(MouseButton button)
		{
			auto mask = SDL_BUTTON(TranslateMouseButton(button));
			return (!(InputData.CurrentMouseState & mask) && (InputData.PreviousMouseState & mask));
		}

		constexpr float ThumbstickDeadzoneRatio = 0.1f;

		float NormalizeThumbstick(Sint16 rawValue)
		{
			float normalizedValue = rawValue / 32767.f;
			/*if (fabs(normalizedValue) <= ThumbstickDeadzoneRatio)
			{
				normalizedValue = 0.f;
			}*/
			return normalizedValue;
		}

		float GetLeftThumbstickX()
		{
			if (InputData.Controller)
			{
				return NormalizeThumbstick(SDL_GameControllerGetAxis(InputData.Controller, SDL_CONTROLLER_AXIS_LEFTX));
			}
			else return 0.f;
		}

		float GetLeftThumbstickY()
		{
			if (InputData.Controller)
			{
				return NormalizeThumbstick(SDL_GameControllerGetAxis(InputData.Controller, SDL_CONTROLLER_AXIS_LEFTY));
			}
			else return 0.f;
		}

		void SetRelativeMouseState(bool state)
		{
			InputData.RelativeMode = state;
			SDL_SetRelativeMouseMode(state ? SDL_TRUE : SDL_FALSE);
		}
	}
}

extern "C"
{
	DLLExport int SPF_IsKeyDown(int key)
	{
		return SPF::Input::IsKeyDown((SPF::Key)key) ? 1 : 0;
	}

	DLLExport int SPF_IsKeyPressed(int key)
	{
		return SPF::Input::IsKeyPressed((SPF::Key)key) ? 1 : 0;
	}

	DLLExport int SPF_IsKeyReleased(int key)
	{
		return SPF::Input::IsKeyReleased((SPF::Key)key) ? 1 : 0;
	}

	DLLExport int SPF_IsControllerConnected()
	{
		return SPF::Input::IsControllerConnected() ? 1 : 0;
	}

	DLLExport int SPF_IsButtonDown(int button)
	{
		return SPF::Input::IsButtonDown((SPF::Button)button) ? 1 : 0;
	}

	DLLExport int SPF_IsButtonPressed(int button)
	{
		return SPF::Input::IsButtonPressed((SPF::Button)button) ? 1 : 0;
	}

	DLLExport int SPF_IsButtonReleased(int button)
	{
		return SPF::Input::IsButtonReleased((SPF::Button)button) ? 1 : 0;
	}

	DLLExport int SPF_GetMousePositionX()
	{
		return SPF::Input::GetMousePositionX();
	}

	DLLExport int SPF_GetMousePositionY()
	{
		return SPF::Input::GetMousePositionY();
	}

	DLLExport int SPF_GetMouseDeltaX()
	{
		return SPF::Input::GetMouseDeltaX();
	}

	DLLExport int SPF_GetMouseDeltaY()
	{
		return SPF::Input::GetMouseDeltaY();
	}

	DLLExport int SPF_IsMouseButtonDown(int button)
	{
		return SPF::Input::IsMouseButtonDown((SPF::MouseButton)button) ? 1 : 0;
	}

	DLLExport int SPF_IsMouseButtonPressed(int button)
	{
		return SPF::Input::IsMouseButtonPressed((SPF::MouseButton)button) ? 1 : 0;
	}

	DLLExport int SPF_IsMouseButtonReleased(int button)
	{
		return SPF::Input::IsMouseButtonReleased((SPF::MouseButton)button) ? 1 : 0;
	}

	DLLExport float SPF_GetLeftThumbstickX()
	{
		return SPF::Input::GetLeftThumbstickX();
	}

	DLLExport float SPF_GetLeftThumbstickY()
	{
		return SPF::Input::GetLeftThumbstickY();
	}

	DLLExport void SPF_SetRelativeMouseState(bool state)
	{
		SPF::Input::SetRelativeMouseState(state);
	}
}