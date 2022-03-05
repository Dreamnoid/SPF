#include <Input.h>
#include <SDL.h>
#include <cstring>
#include <string>

namespace SPF
{
	constexpr int ModifiersCount = 3;

	struct
	{
		SDL_Window* Window;
		SDL_GameController* Controller;
		int KeysDownPreviousFrame[SDL_NUM_SCANCODES + ModifiersCount];
		int KeysDown[SDL_NUM_SCANCODES + ModifiersCount];

		int ButtonsDownPreviousFrame[SDL_CONTROLLER_BUTTON_MAX];
		int ButtonsDown[SDL_CONTROLLER_BUTTON_MAX];

		int MouseX;
		int MouseY;
		int MouseDeltaX;
		int MouseDeltaY;
		int MouseWheel;
		unsigned int CurrentMouseState;
		unsigned int PreviousMouseState;
		bool RelativeMode = false;

		std::string TextInput;
		bool TextInputEnabled = false;

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
			case Key::A: return SDL_SCANCODE_A;
			case Key::B: return SDL_SCANCODE_B;
			case Key::C: return SDL_SCANCODE_C;
			case Key::D: return SDL_SCANCODE_D;
			case Key::E: return SDL_SCANCODE_E;
			case Key::F: return SDL_SCANCODE_F;
			case Key::G: return SDL_SCANCODE_G;
			case Key::H: return SDL_SCANCODE_H;
			case Key::I: return SDL_SCANCODE_I;
			case Key::J: return SDL_SCANCODE_J;
			case Key::K: return SDL_SCANCODE_K;
			case Key::L: return SDL_SCANCODE_L;
			case Key::M: return SDL_SCANCODE_M;
			case Key::N: return SDL_SCANCODE_N;
			case Key::O: return SDL_SCANCODE_O;
			case Key::P: return SDL_SCANCODE_P;
			case Key::Q: return SDL_SCANCODE_Q;
			case Key::R: return SDL_SCANCODE_R;
			case Key::S: return SDL_SCANCODE_S;
			case Key::T: return SDL_SCANCODE_T;
			case Key::U: return SDL_SCANCODE_U;
			case Key::V: return SDL_SCANCODE_V;
			case Key::W: return SDL_SCANCODE_W;
			case Key::X: return SDL_SCANCODE_X;
			case Key::Y: return SDL_SCANCODE_Y;
			case Key::Z: return SDL_SCANCODE_Z;
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
			case Key::Return: return SDL_SCANCODE_RETURN;
			case Key::Control: return (SDL_NUM_SCANCODES + 0);
			case Key::Shift: return (SDL_NUM_SCANCODES + 1);
			case Key::Alt: return (SDL_NUM_SCANCODES + 2);
			case Key::F1: return SDL_SCANCODE_F1;
			case Key::F2: return SDL_SCANCODE_F2;
			case Key::F3: return SDL_SCANCODE_F3;
			case Key::F4: return SDL_SCANCODE_F4;
			case Key::F5: return SDL_SCANCODE_F5;
			case Key::F6: return SDL_SCANCODE_F6;
			case Key::F7: return SDL_SCANCODE_F7;
			case Key::F8: return SDL_SCANCODE_F8;
			case Key::F9: return SDL_SCANCODE_F9;
			case Key::F10: return SDL_SCANCODE_F10;
			case Key::F11: return SDL_SCANCODE_F11;
			case Key::F12: return SDL_SCANCODE_F12;
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
			case MouseButton::Middle: return SDL_BUTTON_MIDDLE;
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

		void Init(const OpaquePointer window)
		{
			InputData.Window = (SDL_Window*)window;
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

			InputData.MouseWheel = 0;
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
			if (InputData.TextInputEnabled && (evt.type == SDL_KEYDOWN) && (evt.key.keysym.sym == SDLK_BACKSPACE) && InputData.TextInput.size())
			{
				InputData.TextInput.pop_back();
			}
			if (evt.type == SDL_TEXTINPUT)
			{
				InputData.TextInput.append(evt.text.text);
			}
			if (evt.type == SDL_MOUSEWHEEL)
			{
				InputData.MouseWheel = evt.wheel.y;
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

		int GetMouseWheel()
		{
			return InputData.MouseWheel;
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

		float GetRightThumbstickX()
		{
			if (InputData.Controller)
			{
				return NormalizeThumbstick(SDL_GameControllerGetAxis(InputData.Controller, SDL_CONTROLLER_AXIS_RIGHTX));
			}
			else return 0.f;
		}

		float GetRightThumbstickY()
		{
			if (InputData.Controller)
			{
				return NormalizeThumbstick(SDL_GameControllerGetAxis(InputData.Controller, SDL_CONTROLLER_AXIS_RIGHTY));
			}
			else return 0.f;
		}

		void SetRelativeMouseState(bool state)
		{
			if (InputData.RelativeMode == state)
				return;

			int w, h;
			SDL_GetWindowSize(InputData.Window, &w, &h);
			InputData.MouseX = w / 2;
			InputData.MouseY = h / 2;

			InputData.RelativeMode = state;
			if (state)
			{
				SDL_WarpMouseInWindow(InputData.Window, InputData.MouseX, InputData.MouseY);
				SDL_SetRelativeMouseMode(SDL_TRUE);
				int mdx, mdy;
				SDL_GetRelativeMouseState(&mdx, &mdy); // Exhaust the delta, so it won't jump next Update
			}
			else
			{
				SDL_SetRelativeMouseMode(SDL_FALSE);
				SDL_WarpMouseInWindow(InputData.Window, InputData.MouseX, InputData.MouseY);
			}
		}

		void StartTextInput()
		{
			InputData.TextInput.clear();
			SDL_StartTextInput();
			InputData.TextInputEnabled = true;
		}

		void StopTextInput()
		{
			InputData.TextInputEnabled = false;
			SDL_StopTextInput();
		}

		const char* GetTextInput()
		{
			return InputData.TextInput.c_str();
		}

		void Rumble(float duration, float lowIntensity, float highIntensity)
		{
			if (InputData.Controller)
			{
				SDL_GameControllerRumble(InputData.Controller, (uint16_t)(lowIntensity * 65535.0f), (uint16_t)(highIntensity * 65535.0f), (uint32_t)(duration * 1000.0f));
			}
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

	DLLExport int SPF_GetMouseWheel()
	{
		return SPF::Input::GetMouseWheel();
	}

	DLLExport float SPF_GetLeftThumbstickX()
	{
		return SPF::Input::GetLeftThumbstickX();
	}

	DLLExport float SPF_GetLeftThumbstickY()
	{
		return SPF::Input::GetLeftThumbstickY();
	}

	DLLExport float SPF_GetRightThumbstickX()
	{
		return SPF::Input::GetRightThumbstickX();
	}

	DLLExport float SPF_GetRightThumbstickY()
	{
		return SPF::Input::GetRightThumbstickY();
	}

	DLLExport void SPF_SetRelativeMouseState(bool state)
	{
		SPF::Input::SetRelativeMouseState(state);
	}

	DLLExport void SPF_StartTextInput()
	{
		SPF::Input::StartTextInput();
	}

	DLLExport void SPF_StopTextInput()
	{
		SPF::Input::StopTextInput();
	}

	DLLExport const char* SPF_GetTextInput()
	{
		return SPF::Input::GetTextInput();
	}

	DLLExport void SPF_Rumble(float duration, float lowIntensity, float highIntensity)
	{
		SPF::Input::Rumble(duration, lowIntensity, highIntensity);
	}
}