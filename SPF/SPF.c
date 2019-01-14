#include <stdlib.h>
#include <SDL.h>
#include <SDL_opengl.h>
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

DLLExport void Open(const char* title, int w, int h)
{
	InitData();
	Data.WindowWidth = w;
	Data.WindowHeight = h;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	Data.Window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	Data.OpenGLContext = SDL_GL_CreateContext(Data.Window);
	SDL_GL_LoadLibrary(NULL);

	InitOpenGL4();
	InitRenderer(w,h);

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

	memcpy(Data.KeysDownPreviousFrame, Data.KeysDown, sizeof(Data.KeysDownPreviousFrame));

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
	}
	glClear(GL_COLOR_BUFFER_BIT);
	return 1;
}

DLLExport void EndLoop()
{
	IssueVertices();
	glFlush();
	SDL_GL_SwapWindow(Data.Window);
}

DLLExport void Close()
{
	SDL_GL_DeleteContext(Data.OpenGLContext);
	SDL_DestroyWindow(Data.Window);
	SDL_Quit();
}