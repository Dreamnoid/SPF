#pragma once
#include "Core.h"
#include <SDL.h>

class Window
{
private:
	int mWindowWidth;
	int mWindowHeight;
	SDL_GLContext mOpenGLContext;
	SDL_Window* mWindow;
	unsigned int mLastTick;

public:
	void Open(const char* title, int w, int h);
	bool BeginLoop(float* dt);
	void EndLoop();
	void Close();
	void SetFullscreen(bool fullscreen);

	void SetWindowSize(int w, int h);

	inline int GetWidth() const
	{
		return mWindowWidth;
	}

	inline int GetHeight() const
	{
		return mWindowHeight;
	}
};

extern Window mWindow;