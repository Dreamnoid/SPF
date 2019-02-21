#include <stdlib.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include "gl4.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include "stb_image.h"

#define DLLExport __declspec(dllexport)

typedef unsigned int ResourceIndex;
typedef int bool;

#include "Data.h"
#include "Input.h"
#include "Textures.h"
#include "Renderer.h"
#include "Audio.h"

DLLExport void Open(const char* title, int w, int h)
{
	InitData();
	Data.WindowWidth = w;
	Data.WindowHeight = h;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
	Data.Window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	Data.OpenGLContext = SDL_GL_CreateContext(Data.Window);
	SDL_GL_LoadLibrary(NULL);

	InitOpenGL4();
	InitRenderer(w,h);
	InitAudio();
	SDL_GameControllerEventState(SDL_ENABLE);
	InitInput();

	glClearColor(0.0, 0.0, 0.0, 1.0);

	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

DLLExport bool BeginLoop(float* dt)
{
	unsigned int currentTick = SDL_GetTicks();
	unsigned int timeElapsed = currentTick - Data.LastTick;
	Data.LastTick = currentTick;
	*dt = timeElapsed / 1000.0f;

	UpdateInput();

	SDL_Event evt;
	while (SDL_PollEvent(&evt))
	{
		if ((evt.type == SDL_WINDOWEVENT) && (evt.window.event == SDL_WINDOWEVENT_CLOSE))
		{
			return 0;
		}
		if (evt.type == SDL_KEYDOWN)
		{
			Data.KeysDown[evt.key.keysym.scancode] = 1;
		}
		if (evt.type == SDL_KEYUP)
		{
			Data.KeysDown[evt.key.keysym.scancode] = 0;
		}
		if (evt.type == SDL_CONTROLLERBUTTONDOWN)
		{
			Data.ButtonsDown[evt.cbutton.button] = 1;
		}
		if (evt.type == SDL_CONTROLLERBUTTONUP)
		{
			Data.ButtonsDown[evt.cbutton.button] = 0;
		}
		if (evt.type == SDL_CONTROLLERDEVICEADDED || evt.type == SDL_CONTROLLERDEVICEREMOVED)
		{
			InitInput();
			if(evt.type == SDL_CONTROLLERDEVICEREMOVED)
			{ 
				memset(&Data.ButtonsDown, 0, sizeof(Data.ButtonsDown)); // All buttons released
			}
		}
	}
	BeginSurface(Data.FinalSurface);
	return 1;
}

DLLExport void EndLoop()
{
	IssueVertices();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	SDL_GetWindowSize(Data.Window, &Data.CurrentWidth, &Data.CurrentHeight);
	glViewport(0, 0, (GLsizei)Data.CurrentWidth, (GLsizei)Data.CurrentHeight);
	glClear(GL_COLOR_BUFFER_BIT);

	DrawTexture(GetSurfaceTexture(Data.FinalSurface), 0, 0, Data.CurrentWidth, Data.CurrentHeight, 0, 0, Data.WindowWidth, Data.WindowHeight, 0, 0, 1, 1, 1, 1);

	IssueVertices();
	glFlush();
	SDL_GL_SwapWindow(Data.Window);
}

DLLExport void Close()
{
	QuitInput();
	QuitAudio();
	SDL_GL_DeleteContext(Data.OpenGLContext);
	SDL_DestroyWindow(Data.Window);
	SDL_Quit();
}

DLLExport void SetFullscreen(bool fullscreen)
{
	SDL_SetWindowFullscreen(Data.Window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}
