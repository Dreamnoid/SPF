#include <Window.h>
#include <SDL.h>
#include "gl4.h"
#include <Audio.h>
#include <Renderer.h>
#include <Input.h>
#include <Shaders.h>
#include <Surfaces.h>

namespace SPF
{
	struct
	{
		int Width;
		int Height;
		SDL_GLContext OpenGLContext;
		SDL_Window* Window;
		unsigned int LastTick;
	} WindowData;

	namespace Window
	{
		void Open(const char* title, int w, int h)
		{
			WindowData.Width = w;
			WindowData.Height = h;

			SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);

			SDL_GL_LoadLibrary(NULL);
			SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
			//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
			//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

			WindowData.Window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
			WindowData.OpenGLContext = SDL_GL_CreateContext(WindowData.Window);

			InitOpenGL4();

			Shaders::Init();
			Renderer::Init(w, h);
			Audio::Init();
			Input::Init(WindowData.Window);

			glClearColor(0.0, 0.0, 0.0, 0.0);
			glClearDepth(1.0f);

			glDisable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);

			glEnable(GL_BLEND);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		}

		bool BeginLoop(float* dt)
		{
			unsigned int currentTick = SDL_GetTicks();
			unsigned int timeElapsed = currentTick - WindowData.LastTick;
			WindowData.LastTick = currentTick;
			*dt = timeElapsed / 1000.0f;

			Input::Update();

			SDL_Event evt;
			while (SDL_PollEvent(&evt))
			{
				if ((evt.type == SDL_WINDOWEVENT) && (evt.window.event == SDL_WINDOWEVENT_CLOSE))
				{
					return false;
				}
				Input::HandleEvent(&evt);
			}

			Surfaces::Clear(Renderer::GetFinalSurface());
			Renderer::Begin(Renderer::GetFinalSurface());
			return true;
		}

		void EndLoop()
		{
			int w, h;
			SDL_GetWindowSize(WindowData.Window, &w, &h);
			Renderer::DrawFinalSurface(w, h);
			glFlush();
			SDL_GL_SwapWindow(WindowData.Window);
		}

		void Close()
		{
			Input::Dispose();
			Audio::Dispose();
			SDL_GL_DeleteContext(WindowData.OpenGLContext);
			SDL_DestroyWindow(WindowData.Window);
			SDL_Quit();
		}

		void SetFullscreen(bool fullscreen)
		{
			SDL_SetWindowFullscreen(WindowData.Window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
		}

		void SetWindowSize(int w, int h)
		{
			SDL_SetWindowSize(WindowData.Window, w, h);
			WindowData.Width = w;
			WindowData.Height = h; 
			Renderer::Resize(w, h);
		}

		void SetWindowTitle(const char* title)
		{
			SDL_SetWindowTitle(WindowData.Window, title);
		}

		int GetWidth()
		{
			return WindowData.Width;
		}

		int GetHeight()
		{
			return WindowData.Height;
		}

		bool HasFocus()
		{
			return (SDL_GetWindowFlags(WindowData.Window) & SDL_WINDOW_INPUT_FOCUS) == SDL_WINDOW_INPUT_FOCUS;
		}
	}
}

extern "C"
{
	DLLExport void SPF_Open(const char* title, int w, int h)
	{
		SPF::Window::Open(title, w, h);
	}

	DLLExport int SPF_BeginLoop(float* dt)
	{
		return SPF::Window::BeginLoop(dt) ? 1 : 0;
	}

	DLLExport void SPF_EndLoop()
	{
		SPF::Window::EndLoop();
	}

	DLLExport void SPF_Close()
	{
		SPF::Window::Close();
	}

	DLLExport void SPF_SetFullscreen(bool fullscreen)
	{
		SPF::Window::SetFullscreen(fullscreen);
	}

	DLLExport int SPF_GetWindowWidth()
	{
		return SPF::Window::GetWidth();
	}

	DLLExport int SPF_GetWindowHeight()
	{
		return SPF::Window::GetHeight();
	}

	DLLExport void SPF_SetWindowSize(int w, int h)
	{
		SPF::Window::SetWindowSize(w, h);
	}

	DLLExport void SPF_SetWindowTitle(const char* title)
	{
		SPF::Window::SetWindowTitle(title);
	}

	DLLExport int SPF_HasFocus()
	{
		return SPF::Window::HasFocus();
	}
}