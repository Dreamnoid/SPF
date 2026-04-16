#include <Core.h>
#include <SDL3/SDL.h>
#include <cstring>
#include <string>

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
		PrintScreen,
		Backspace,
		Home,
		End
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
		Home = 12,
	};

	enum class MouseButton : int
	{
		Left = 0,
		Right = 1,
		Middle = 2
	};

	enum class ControllerModel
	{
		None = 0,
		XBox = 1,
		Playstation = 2,
	};

	constexpr int ModifiersCount = 3;

	struct
	{
		SDL_Window* Window = nullptr;
		SDL_Gamepad* Controller = nullptr;
		int KeysDownPreviousFrame[SDL_SCANCODE_COUNT + ModifiersCount];
		int KeysDown[SDL_SCANCODE_COUNT + ModifiersCount];

		int ButtonsDownPreviousFrame[SDL_GAMEPAD_BUTTON_COUNT];
		int ButtonsDown[SDL_GAMEPAD_BUTTON_COUNT];

		uint64_t GamepadTimestamp = 0;
		uint64_t KeyboardTimestamp = 0;

		int MouseX = 0;
		int MouseY = 0;
		int MouseDeltaX = 0;
		int MouseDeltaY = 0;
		int MouseWheel = 0;
		unsigned int CurrentMouseState = 0;
		unsigned int PreviousMouseState = 0;

		bool IsGamepadLost = false;
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
			case Key::Control: return (SDL_SCANCODE_COUNT + 0);
			case Key::Shift: return (SDL_SCANCODE_COUNT + 1);
			case Key::Alt: return (SDL_SCANCODE_COUNT + 2);
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
			case Key::Tab: return SDL_SCANCODE_TAB;
			case Key::PrintScreen: return SDL_SCANCODE_PRINTSCREEN;
			case Key::Backspace: return SDL_SCANCODE_BACKSPACE;
			case Key::Home: return SDL_SCANCODE_HOME;
			case Key::End: return SDL_SCANCODE_END;
			default: return SDL_SCANCODE_RETURN;
			}
		}

		Key LocalizeKey(Key key)
		{
			int pseudoScanCode = TranslateKey(key);
			if (pseudoScanCode >= SDL_SCANCODE_COUNT)
				return key;

			SDL_Keycode keyCode = SDL_GetKeyFromScancode((SDL_Scancode)pseudoScanCode, SDL_KMOD_NONE, false);
			switch (keyCode)
			{
			case SDLK_UP: return Key::Up;
			case SDLK_DOWN: return Key::Down;
			case SDLK_LEFT: return Key::Left;
			case SDLK_RIGHT: return Key::Right;
			case SDLK_SPACE: return Key::Space;
			case SDLK_ESCAPE: return Key::Escape;
			case SDLK_DELETE: return Key::Delete;
			case SDLK_A: return Key::A;
			case SDLK_B: return Key::B;
			case SDLK_C: return Key::C;
			case SDLK_D: return Key::D;
			case SDLK_E: return Key::E;
			case SDLK_F: return Key::F;
			case SDLK_G: return Key::G;
			case SDLK_H: return Key::H;
			case SDLK_I: return Key::I;
			case SDLK_J: return Key::J;
			case SDLK_K: return Key::K;
			case SDLK_L: return Key::L;
			case SDLK_M: return Key::M;
			case SDLK_N: return Key::N;
			case SDLK_O: return Key::O;
			case SDLK_P: return Key::P;
			case SDLK_Q: return Key::Q;
			case SDLK_R: return Key::R;
			case SDLK_S: return Key::S;
			case SDLK_T: return Key::T;
			case SDLK_U: return Key::U;
			case SDLK_V: return Key::V;
			case SDLK_W: return Key::W;
			case SDLK_X: return Key::X;
			case SDLK_Y: return Key::Y;
			case SDLK_Z: return Key::Z;
			case SDLK_KP_0: return Key::Num0;
			case SDLK_KP_1: return Key::Num1;
			case SDLK_KP_2: return Key::Num2;
			case SDLK_KP_3: return Key::Num3;
			case SDLK_KP_4: return Key::Num4;
			case SDLK_KP_5: return Key::Num5;
			case SDLK_KP_6: return Key::Num6;
			case SDLK_KP_7: return Key::Num7;
			case SDLK_KP_8: return Key::Num8;
			case SDLK_KP_9: return Key::Num9;
			case SDLK_RETURN: return Key::Return;
			case SDLK_F1: return Key::F1;
			case SDLK_F2: return Key::F2;
			case SDLK_F3: return Key::F3;
			case SDLK_F4: return Key::F4;
			case SDLK_F5: return Key::F5;
			case SDLK_F6: return Key::F6;
			case SDLK_F7: return Key::F7;
			case SDLK_F8: return Key::F8;
			case SDLK_F9: return Key::F9;
			case SDLK_F10: return Key::F10;
			case SDLK_F11: return Key::F11;
			case SDLK_F12: return Key::F12;
			case SDLK_TAB: return Key::Tab;
			case SDLK_PRINTSCREEN: return Key::PrintScreen;
			case SDLK_BACKSPACE: return Key::Backspace;
			case SDLK_HOME: return Key::Home;
			case SDLK_END: return Key::End;
			default: return Key::Return;
			}
		}

		SDL_GamepadButton TranslateButton(Button button)
		{
			switch (button)
			{
			case Button::A: return SDL_GAMEPAD_BUTTON_SOUTH;
			case Button::B: return SDL_GAMEPAD_BUTTON_EAST;
			case Button::X: return SDL_GAMEPAD_BUTTON_WEST;
			case Button::Y: return SDL_GAMEPAD_BUTTON_NORTH;
			case Button::Start: return SDL_GAMEPAD_BUTTON_START;
			case Button::Select: return SDL_GAMEPAD_BUTTON_BACK;
			case Button::DPadUp: return SDL_GAMEPAD_BUTTON_DPAD_UP;
			case Button::DPadDown: return SDL_GAMEPAD_BUTTON_DPAD_DOWN;
			case Button::DPadRight: return SDL_GAMEPAD_BUTTON_DPAD_RIGHT;
			case Button::DPadLeft: return SDL_GAMEPAD_BUTTON_DPAD_LEFT;
			case Button::LeftShoulder: return SDL_GAMEPAD_BUTTON_LEFT_SHOULDER;
			case Button::RightShoulder: return SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER;
			case Button::Home: return SDL_GAMEPAD_BUTTON_GUIDE;
			default: return SDL_GAMEPAD_BUTTON_SOUTH;
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

		void Init(SDL_Window* window)
		{
			InputData.Window = window;
			SDL_SetGamepadEventsEnabled(true);
		}

		void Update(const Size& windowSize)
		{
			memcpy(InputData.KeysDownPreviousFrame, InputData.KeysDown, sizeof(InputData.KeysDownPreviousFrame));
			memcpy(InputData.ButtonsDownPreviousFrame, InputData.ButtonsDown, sizeof(InputData.ButtonsDownPreviousFrame));

			int previousMouseX = InputData.MouseX;
			int previousMouseY = InputData.MouseY;
			InputData.PreviousMouseState = InputData.CurrentMouseState;
			float mouseX, mouseY;
			InputData.CurrentMouseState = SDL_GetMouseState(&mouseX, &mouseY);
			InputData.MouseX = (int)mouseX;
			InputData.MouseY = (int)mouseY;

			// Scale mouse position with fullscreen desktop mode
			{
				int hardwareWindowWidth, hardwareWindowHeight;
				SDL_GetWindowSize(InputData.Window, &hardwareWindowWidth, &hardwareWindowHeight);
				InputData.MouseX = (int)((InputData.MouseX / (float)hardwareWindowWidth) * windowSize.Width);
				InputData.MouseY = (int)((InputData.MouseY / (float)hardwareWindowHeight) * windowSize.Height);
			}

			InputData.MouseDeltaX = InputData.MouseX - previousMouseX;
			InputData.MouseDeltaY = InputData.MouseY - previousMouseY;

			SDL_Keymod modifiers = SDL_GetModState();
			InputData.KeysDown[TranslateKey(Key::Control)] = ((modifiers & SDL_KMOD_CTRL) != 0);
			InputData.KeysDown[TranslateKey(Key::Shift)] = ((modifiers & SDL_KMOD_SHIFT) != 0);
			InputData.KeysDown[TranslateKey(Key::Alt)] = ((modifiers & SDL_KMOD_ALT) != 0);

			InputData.MouseWheel = 0;
		}

		uint64_t GetCurrentTimestamp()
		{
			SDL_Time time;
			SDL_GetCurrentTime(&time);
			return (uint64_t)time;
		}

		constexpr float ThumbstickDeadzoneRatio = 0.33f;

		float NormalizeThumbstick(Sint16 rawValue)
		{
			return rawValue / 32767.f;
		}

		void UseGamepad(SDL_Gamepad* gamepad)
		{
			if (gamepad)
			{
				InputData.Controller = gamepad; // Mark this gamepad as the current one
			}
			InputData.GamepadTimestamp = GetCurrentTimestamp(); // Record timestamp to compare against other input methods
		}

		void HandleEvent(const SDL_Event& evt)
		{
			if (evt.type == SDL_EVENT_KEY_DOWN)
			{
				InputData.KeyboardTimestamp = GetCurrentTimestamp();
				InputData.KeysDown[evt.key.scancode] = 1;
			}
			if (evt.type == SDL_EVENT_KEY_UP)
			{
				InputData.KeysDown[evt.key.scancode] = 0;
			}
			if (evt.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN)
			{
				UseGamepad(SDL_GetGamepadFromID(evt.gbutton.which));
				InputData.ButtonsDown[evt.gbutton.button] = 1;
			}
			if (evt.type == SDL_EVENT_GAMEPAD_BUTTON_UP)
			{
				InputData.ButtonsDown[evt.gbutton.button] = 0;
			}
			if (evt.type == SDL_EVENT_GAMEPAD_ADDED) // Will be received even if the gamepad was connected prior to the start
			{
				SDL_Gamepad* gamepad = SDL_OpenGamepad(evt.gdevice.which);
				if (gamepad && InputData.Controller == nullptr)
				{
					InputData.Controller = gamepad; // If no other gamepad is currently in use, assume this one will be
				}
			}
			else if (evt.type == SDL_EVENT_GAMEPAD_REMOVED)
			{
				SDL_Gamepad* gamepad = SDL_GetGamepadFromID(evt.gdevice.which);
				if (gamepad)
				{
					if (gamepad == InputData.Controller) // We lost the gamepad that was in use
					{
						InputData.Controller = nullptr;
						memset(&InputData.ButtonsDown, 0, sizeof(InputData.ButtonsDown)); // All buttons released
						if (InputData.GamepadTimestamp > InputData.KeyboardTimestamp)
						{
							InputData.IsGamepadLost = true; // We were currently using this gamepad as the input method, losing it is an issue
						}
					}
					SDL_CloseGamepad(gamepad);
				}
			}
			else if (evt.type == SDL_EVENT_GAMEPAD_AXIS_MOTION)
			{
				if (fabs(NormalizeThumbstick(evt.gaxis.value)) > ThumbstickDeadzoneRatio)
				{
					UseGamepad(SDL_GetGamepadFromID(evt.gaxis.which));
				}
			}
			if (evt.type == SDL_EVENT_MOUSE_WHEEL)
			{
				InputData.MouseWheel = (int)evt.wheel.y;
			}
		}

		void Dispose()
		{
			if (InputData.Controller)
			{
				SDL_CloseGamepad(InputData.Controller);
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
			SDL_GamepadButton code = TranslateButton(button);
			return !InputData.ButtonsDownPreviousFrame[code] && InputData.ButtonsDown[code];
		}

		bool IsButtonReleased(Button button)
		{
			SDL_GamepadButton code = TranslateButton(button);
			return InputData.ButtonsDownPreviousFrame[code] && !InputData.ButtonsDown[code];
		}

		bool IsUsingController()
		{
			return InputData.Controller && (InputData.GamepadTimestamp > InputData.KeyboardTimestamp);
		}

		bool IsCurrentInputLost()
		{
			bool isLost = InputData.IsGamepadLost;
			InputData.IsGamepadLost = false;
			return isLost;
		}

		ControllerModel GetControllerModel()
		{
			if (IsUsingController())
			{
				switch (SDL_GetGamepadType(InputData.Controller))
				{
				case SDL_GAMEPAD_TYPE_XBOX360:
				case SDL_GAMEPAD_TYPE_XBOXONE:
					return ControllerModel::XBox;

				case SDL_GAMEPAD_TYPE_PS3:
				case SDL_GAMEPAD_TYPE_PS4:
				case SDL_GAMEPAD_TYPE_PS5:
					return ControllerModel::Playstation;

				default:
					return ControllerModel::XBox;
				}
			}
			return ControllerModel::None;
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
			return (InputData.CurrentMouseState & SDL_BUTTON_MASK(TranslateMouseButton(button)));
		}

		bool IsMouseButtonPressed(MouseButton button)
		{
			auto mask = SDL_BUTTON_MASK(TranslateMouseButton(button));
			return ((InputData.CurrentMouseState & mask) && !(InputData.PreviousMouseState & mask));
		}

		bool IsMouseButtonReleased(MouseButton button)
		{
			auto mask = SDL_BUTTON_MASK(TranslateMouseButton(button));
			return (!(InputData.CurrentMouseState & mask) && (InputData.PreviousMouseState & mask));
		}

		int GetMouseWheel()
		{
			return InputData.MouseWheel;
		}

		float GetLeftThumbstickX()
		{
			if (InputData.Controller)
			{
				return NormalizeThumbstick(SDL_GetGamepadAxis(InputData.Controller, SDL_GAMEPAD_AXIS_LEFTX));
			}
			else return 0.f;
		}

		float GetLeftThumbstickY()
		{
			if (InputData.Controller)
			{
				return NormalizeThumbstick(SDL_GetGamepadAxis(InputData.Controller, SDL_GAMEPAD_AXIS_LEFTY));
			}
			else return 0.f;
		}

		float GetRightThumbstickX()
		{
			if (InputData.Controller)
			{
				return NormalizeThumbstick(SDL_GetGamepadAxis(InputData.Controller, SDL_GAMEPAD_AXIS_RIGHTX));
			}
			else return 0.f;
		}

		float GetRightThumbstickY()
		{
			if (InputData.Controller)
			{
				return NormalizeThumbstick(SDL_GetGamepadAxis(InputData.Controller, SDL_GAMEPAD_AXIS_RIGHTY));
			}
			else return 0.f;
		}

		void Rumble(float duration, float lowIntensity, float highIntensity)
		{
			if (InputData.Controller)
			{
				SDL_RumbleGamepad(InputData.Controller, (uint16_t)(lowIntensity * 65535.0f), (uint16_t)(highIntensity * 65535.0f), (uint32_t)(duration * 1000.0f));
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

	DLLExport int SPF_IsUsingController()
	{
		return SPF::Input::IsUsingController() ? 1 : 0;
	}

	DLLExport int SPF_IsCurrentInputLost()
	{
		return SPF::Input::IsCurrentInputLost() ? 1 : 0;
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

	DLLExport int SPF_GetControllerModel()
	{
		return (int)SPF::Input::GetControllerModel();
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

	DLLExport void SPF_Rumble(float duration, float lowIntensity, float highIntensity)
	{
		SPF::Input::Rumble(duration, lowIntensity, highIntensity);
	}

	DLLExport int SPF_LocalizeKey(int key)
	{
		return (int)SPF::Input::LocalizeKey((SPF::Key)key);
	}
}