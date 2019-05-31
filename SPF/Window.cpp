#include "Window.h"
#include "gl4.h"
#include "Audio.h"
#include "Renderer.h"
#include "Input.h"
#include "Surfaces.h"

Window mWindow;

void Window::Open(const char* title, int w, int h)
{
	mWindowWidth = w;
	mWindowHeight = h;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);

	SDL_GL_LoadLibrary(NULL);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	mWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
	mOpenGLContext = SDL_GL_CreateContext(mWindow);

	InitOpenGL4();

	mRenderer.Init(w, h);
	mAudio.Init();
	mInput.Init();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0f);

	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

bool Window::BeginLoop(float* dt)
{
	unsigned int currentTick = SDL_GetTicks();
	unsigned int timeElapsed = currentTick - mLastTick;
	mLastTick = currentTick;
	*dt = timeElapsed / 1000.0f;

	mInput.Update();

	SDL_Event evt;
	while (SDL_PollEvent(&evt))
	{
		if ((evt.type == SDL_WINDOWEVENT) && (evt.window.event == SDL_WINDOWEVENT_CLOSE))
		{
			return false;
		}
		mInput.HandleEvent(evt);
	}

	mSurfaces.Clear(mRenderer.GetFinalSurface());
	mRenderer.Begin(mRenderer.GetFinalSurface());
	return true;
}

void Window::EndLoop()
{
	int w, h;
	SDL_GetWindowSize(mWindow, &w, &h);
	mRenderer.DrawFinalSurface(w, h);
	glFlush();
	SDL_GL_SwapWindow(mWindow);
}

void Window::Close()
{
	mInput.Dispose();
	mAudio.Dispose();
	SDL_GL_DeleteContext(mOpenGLContext);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Window::SetFullscreen(bool fullscreen)
{
	SDL_SetWindowFullscreen(mWindow, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

void Window::SetWindowSize(int w, int h)
{
	SDL_SetWindowSize(mWindow, w, h);
	mWindowWidth = w;
	mWindowHeight = h;
}

extern "C"
{
	DLLExport void Open(const char* title, int w, int h)
	{
		mWindow.Open(title, w, h);
	}

	DLLExport int BeginLoop(float* dt)
	{
		return mWindow.BeginLoop(dt) ? 1 : 0;
	}

	DLLExport void EndLoop()
	{
		mWindow.EndLoop();
	}

	DLLExport void Close()
	{
		mWindow.Close();
	}

	DLLExport void SetFullscreen(bool fullscreen)
	{
		mWindow.SetFullscreen(fullscreen);
	}

	DLLExport int GetWindowWidth()
	{
		return mWindow.GetWidth();
	}

	DLLExport int GetWindowHeight()
	{
		return mWindow.GetHeight();
	}

	DLLExport void SetWindowSize(int w, int h)
	{
		mWindow.SetWindowSize(w, h);
	}

}