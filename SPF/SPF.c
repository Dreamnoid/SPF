#include <SDL.h>
#include <SDL_opengl.h>
#include "gl4.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include "stb_image.h"

#define DLLExport __declspec(dllexport)

void SetupOrthographic(float* m, float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
{
	m[0] = (float)(2.0 / ((double)right - (double)left));
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;
	m[4] = 0.0f;
	m[5] = (float)(2.0 / ((double)top - (double)bottom));
	m[6] = 0.0f;
	m[7] = 0.0f;
	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = (float)(1.0 / ((double)zNearPlane - (double)zFarPlane));
	m[11] = 0.0f;
	m[12] = (float)(((double)left + (double)right) / ((double)left - (double)right));
	m[13] = (float)(((double)top + (double)bottom) / ((double)bottom - (double)top));
	m[14] = (float)((double)zNearPlane / ((double)zNearPlane - (double)zFarPlane));
	m[15] = 1.0f;
}

// TODO: Free-list
typedef struct
{
	int GLID;
	int Width;
	int Height;
} TextureInfo;
TextureInfo Textures[200];
int TextureUsed = 0;

#include "Input.h"
#include "Renderer.h"

SDL_GLContext _glContext;

SDL_Window* _window;
unsigned int _lastTick;

DLLExport void Open(const char* title, int w, int h)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	_glContext = SDL_GL_CreateContext(_window);
	SDL_GL_LoadLibrary(NULL);

	InitOpenGL4();
	InitRenderer(w,h);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

DLLExport int BeginLoop(float* dt)
{
	unsigned int currentTick = SDL_GetTicks();
	unsigned int timeElapsed = currentTick - _lastTick;
	_lastTick = currentTick;
	*dt = timeElapsed / 1000.0f;

	memcpy(KeysDownPreviousFrame, KeysDown, sizeof(KeysDownPreviousFrame));

	SDL_Event evt;
	while (SDL_PollEvent(&evt))
	{
		if ((evt.type == SDL_WINDOWEVENT) && (evt.window.event == SDL_WINDOWEVENT_CLOSE))
		{
			return 0;
		}
		if (evt.type == SDL_KEYDOWN)
		{
			KeysDown[evt.key.keysym.scancode] = 1;
		}
		if (evt.type == SDL_KEYUP)
		{
			KeysDown[evt.key.keysym.scancode] = 0;
		}
	}
	glClear(GL_COLOR_BUFFER_BIT);
	return 1;
}

DLLExport int LoadTexture(const char* filename)
{
	int w, h, bpp;
	stbi_uc* pixels = stbi_load(filename, &w, &h, &bpp, 4);

	GLuint ids[1];
	glGenTextures(1, ids);
	auto id = ids[0];

	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)pixels);

	stbi_image_free((void*)pixels);

	Textures[TextureUsed].GLID = id;
	Textures[TextureUsed].Width = w;
	Textures[TextureUsed].Height = h;
	return TextureUsed++;
}

DLLExport void DeleteTexture(int texture)
{
	GLuint ids[1];
	ids[0] = Textures[texture].GLID;
	glDeleteTextures(1, ids);
}

DLLExport void EndLoop()
{
	IssueVertices();
	glFlush();
	SDL_GL_SwapWindow(_window);
}

DLLExport void Close()
{
	SDL_GL_DeleteContext(_glContext);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}
