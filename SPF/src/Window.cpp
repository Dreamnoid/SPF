#include <Window.h>
#include <SDL.h>
#include "GL4.h"
#include <Audio.h>
#include <Renderer.h>
#include <Input.h>
#include <Shaders.h>
#include <Surfaces.h>
#include "portable-file-dialogs.h"
#include <filesystem>
#include <Images.h>

namespace SPF
{
	struct
	{
		int Width = 0;
		int Height = 0;
		SDL_GLContext OpenGLContext = nullptr;
		SDL_Window* Window = nullptr;
		unsigned int LastTick = 0;
		char* LastClipboardData = nullptr;
	} WindowData;

	namespace Window
	{
		void Open(const char* title, int w, int h)
		{
			WindowData.Width = w;
			WindowData.Height = h;

			if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
			{
				FatalError(SDL_GetError());
			}

			SDL_GL_LoadLibrary(NULL);
			SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
			SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

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

			Input::Update({ WindowData.Width, WindowData.Height });

			SDL_Event evt;
			while (SDL_PollEvent(&evt))
			{
				if ((evt.type == SDL_WINDOWEVENT) && (evt.window.event == SDL_WINDOWEVENT_CLOSE))
				{
					return false;
				}
				Input::HandleEvent(&evt);
			}

			Renderer::Begin(Renderer::GetFinalSurface(), true);
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

			SDL_SetWindowPosition(WindowData.Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
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

		void FlipSurfaceVertical(SDL_Surface* surface)
		{
			Uint16 pitch = surface->pitch; // Allocates memory to store temp lines
			Uint8* t = (Uint8*)malloc(pitch);
			if (t == nullptr)
				return;

			memcpy(t, surface->pixels, pitch); // Save the first line

			Uint8* a = (Uint8*)surface->pixels;
			Uint8* last = a + pitch * (surface->h - 1);
			Uint8* b = last;
			while (a < b)
			{
				memcpy(a, b, pitch);
				a += pitch;
				memcpy(b, a, pitch);
				b -= pitch;
			}

			memmove(b, b + pitch, last - b); // In this shuffled state, the bottom slice is too far down
			memcpy(last, t, pitch); // Put back the first row in the last place

			free(t);
		}

		void SaveScreenshot(const char* filename)
		{
			SDL_Surface* image = SDL_CreateRGBSurface(SDL_SWSURFACE, WindowData.Width, WindowData.Height, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
			glReadPixels(0, 0, WindowData.Width, WindowData.Height, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
			FlipSurfaceVertical(image);

			Images::Save(filename, WindowData.Width, WindowData.Height, (unsigned char*)image->pixels);
			SDL_FreeSurface(image);
		}

		void SetClipboard(const char* text)
		{
			SDL_SetClipboardText(text);
		}

		char* GetClipboard()
		{
			if (WindowData.LastClipboardData != nullptr)
			{
				SDL_free(WindowData.LastClipboardData);
				WindowData.LastClipboardData = nullptr;
			}
			if (SDL_HasClipboardText())
			{
				WindowData.LastClipboardData = SDL_GetClipboardText();
			}
			return WindowData.LastClipboardData;
		}

		Size GetDesktopSize()
		{
			SDL_DisplayMode mode;
			if (SDL_GetDesktopDisplayMode(0, &mode) == 0)
			{
				return { mode.w, mode.h };
			}
			return { 0, 0 };
		}

		void ShowMessageBox(const char* title, const char* message, bool isError)
		{
			SDL_ShowSimpleMessageBox(isError ? SDL_MESSAGEBOX_ERROR : SDL_MESSAGEBOX_INFORMATION, title, message, NULL);
		}

		std::vector<std::string> ParseFilter(const char* filter)
		{
			std::string s(filter);
			const std::string delimiter = "|";
			std::vector<std::string> results;

			size_t pos = 0;
			while ((pos = s.find(delimiter)) != std::string::npos)
			{
				results.push_back(s.substr(0, pos));
				s.erase(0, pos + delimiter.length());
			}
			results.push_back(s);

			return results;
		}

		char* StringToCharArray(const std::string& str)
		{
			char* c = new char[str.length() + 1];
#if _MSC_VER
			strcpy_s(c, str.length() + 1, str.c_str());
#else
			strcpy(c, str.c_str());
#endif
			return c;
		}

		char* OpenFileDialog(const char* defaultFolder, const char* filter)
		{
			auto selection = pfd::open_file("Select a file", defaultFolder, ParseFilter(filter), pfd::opt::none).result();
			if (!selection.empty())
			{
				return StringToCharArray(selection[0]);
			}
			return nullptr;
		}

		char* SaveFileDialog(const char* defaultFolder, const char* filter)
		{
			std::vector<std::string> filterParts = ParseFilter(filter);
			std::string destination = pfd::save_file("Select a file", defaultFolder, filterParts, pfd::opt::none).result();
			std::filesystem::path path(destination);
			if (!path.empty() && !path.has_extension())
			{
				for (const std::string& filterPart : filterParts)
				{
					if(filterPart.rfind("*.", 0) == 0) // Skip the display names, keep only the masks
					{
						path.replace_extension(filterPart.substr(1)); // Skip the *
						break;
					}
				}
			}
			return StringToCharArray(path.string());
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

	DLLExport void SPF_SaveScreenshot(const char* filename)
	{
		SPF::Window::SaveScreenshot(filename);
	}

	DLLExport void SPF_SetClipboard(const char* text)
	{
		SPF::Window::SetClipboard(text);
	}

	DLLExport char* SPF_GetClipboard()
	{
		return SPF::Window::GetClipboard();
	}

	DLLExport void SPF_GetDesktopSize(int* w, int* h)
	{
		SPF::Size size = SPF::Window::GetDesktopSize();
		*w = size.Width;
		*h = size.Height;
	}

	DLLExport void SPF_ShowMessageBox(const char* title, const char* message, bool isError)
	{
		SPF::Window::ShowMessageBox(title, message, isError);
	}

	DLLExport char* SPF_OpenFileDialog(const char* defaultFolder, const char* filter)
	{
		return SPF::Window::OpenFileDialog(defaultFolder, filter);
	}

	DLLExport char* SPF_SaveFileDialog(const char* defaultFolder, const char* filter)
	{
		return SPF::Window::SaveFileDialog(defaultFolder, filter);
	}
}