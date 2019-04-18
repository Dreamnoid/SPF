#pragma once
#include <SDL.h>
#include "Core.h"

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

class Input
{
private:
	SDL_GameController* mController;
	int mKeysDownPreviousFrame[SDL_NUM_SCANCODES];
	int mKeysDown[SDL_NUM_SCANCODES];

	int mButtonsDownPreviousFrame[SDL_CONTROLLER_BUTTON_MAX];
	int mButtonsDown[SDL_CONTROLLER_BUTTON_MAX];

	int mMouseX;
	int mMouseY;
	unsigned int mCurrentMouseState;
	unsigned int mPreviousMouseState;

	void SearchGamepad();
	
public:
	void Init();
	void Update();
	void HandleEvent(const SDL_Event& evt);
	void Dispose();

	bool IsKeyDown(Key key);
	bool IsKeyPressed(Key key);
	bool IsKeyReleased(Key key);
	bool IsControllerConnected();
	bool IsButtonDown(Button button);
	bool IsButtonPressed(Button button);
	bool IsButtonReleased(Button button);
	int GetMousePositionX();
	int GetMousePositionY();
	bool IsMouseButtonDown(MouseButton button);
	bool IsMouseButtonPressed(MouseButton button);
	bool IsMouseButtonReleased(MouseButton button);
};

extern Input mInput;