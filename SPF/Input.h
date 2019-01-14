#pragma once

typedef enum
{
	Key_Up = 0,
	Key_Down = 1,
	Key_Right = 2,
	Key_Left = 3,
	Key_Space = 4,
	Key_Escape = 5
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
		default: return SDL_SCANCODE_RETURN;
	}
}

DLLExport int IsKeyDown(Key key)
{
	return Data.KeysDown[TranslateKey(key)];
}

DLLExport int IsKeyPressed(Key key)
{
	SDL_Scancode code = TranslateKey(key);
	return !Data.KeysDownPreviousFrame[code] && Data.KeysDown[code];
}

DLLExport int IsKeyReleased(Key key)
{
	SDL_Scancode code = TranslateKey(key);
	return Data.KeysDownPreviousFrame[code] && !Data.KeysDown[code];
}
