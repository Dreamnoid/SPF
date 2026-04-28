#include <Core.h>
#include <SDL3/SDL.h>
#include <filesystem>

namespace SPF
{
	struct
	{
		SDL_Window* Window = nullptr;
		SDL_Renderer* Renderer = nullptr;
		Uint64 LastTick = 0;
		char* LastClipboardData = nullptr;
	} WindowData;

	namespace Input
	{
		void Init(SDL_Window* window);
		void Update(const Size& windowSize);
		void HandleEvent(const SDL_Event& evt);
		void Dispose();
	}

	namespace Images
	{
		void Save(const char* filename, int width, int height, unsigned char* pixels);
	}

	namespace Textures
	{
		void Init(SDL_Renderer* renderer);
	}

	namespace Renderer
	{
		void Init(SDL_Renderer* renderer);
		void Begin(ResourceIndex surface, bool clear);
	}

	namespace Audio
	{
		void Init();
		void Dispose();
	}

	namespace Steam
	{
		bool Init(uint32_t appID);
		void Update();
		bool IsOverlayActive();
		void Dispose();
	}

	namespace Window
	{
		bool IsFullscreenRequested()
		{
			return SDL_GetEnvironmentVariable(SDL_GetEnvironment(), "SteamTenfoot");
		}

		void Open(const char* title, int w, int h, uint32_t appID)
		{
			if (Steam::Init(appID)) // Must be called as soon as possible, and always before SDL_Init
			{
				exit(1);
				return;
			}

			if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD))
			{
				FatalError(SDL_GetError());
			}

			if (!SDL_CreateWindowAndRenderer(title, w, h, IsFullscreenRequested() ? SDL_WINDOW_FULLSCREEN : 0, &WindowData.Window, &WindowData.Renderer))
			{
				FatalError(SDL_GetError());
			}

			if (!SDL_SetRenderLogicalPresentation(WindowData.Renderer, w, h, SDL_LOGICAL_PRESENTATION_LETTERBOX))
			{
				FatalError(SDL_GetError());
			}

			Renderer::Init(WindowData.Renderer);
			Textures::Init(WindowData.Renderer);
			Audio::Init();
			Input::Init(WindowData.Window);
		}

		bool BeginLoop(float* dt)
		{
			Uint64 currentTick = SDL_GetTicks();
			Uint64 timeElapsed = currentTick - WindowData.LastTick;
			WindowData.LastTick = currentTick;
			*dt = timeElapsed / 1000.0f;

			int width, height;
			SDL_GetWindowSize(WindowData.Window, &width, &height);
			Input::Update({ width, height });

			SDL_Event evt;
			while (SDL_PollEvent(&evt))
			{
				switch (evt.type)
				{
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
					return false;

				default:
					Input::HandleEvent(evt);
					break;
				}
			}

			Steam::Update();
			Renderer::Begin(UnsetResource, true);
			return true;
		}

		void EndLoop()
		{
			SDL_RenderPresent(WindowData.Renderer);
		}

		void Close()
		{
			Steam::Dispose();
			Input::Dispose();
			Audio::Dispose();
			SDL_DestroyRenderer(WindowData.Renderer);
			SDL_DestroyWindow(WindowData.Window);
			SDL_Quit();
		}

		void SetFullscreen(bool fullscreen)
		{
			SDL_SetWindowFullscreen(WindowData.Window, fullscreen);
			if (fullscreen)
			{
				SDL_HideCursor();
			}
			else
			{
				SDL_ShowCursor();
			}
		}

		void SetVSync(bool enabled)
		{
			SDL_SetRenderVSync(WindowData.Renderer, enabled ? 1 : 0);
		}

		void SetWindowTitle(const char* title)
		{
			SDL_SetWindowTitle(WindowData.Window, title);
		}

		int GetWidth()
		{
			int width = 0;
			SDL_GetWindowSize(WindowData.Window, &width, NULL);
			return width;
		}

		int GetHeight()
		{
			int height = 0;
			SDL_GetWindowSize(WindowData.Window, NULL, &height);
			return height;
		}

		bool HasFocus()
		{
			return ((SDL_GetWindowFlags(WindowData.Window) & SDL_WINDOW_INPUT_FOCUS) == SDL_WINDOW_INPUT_FOCUS) && !Steam::IsOverlayActive();
		}

		void SaveScreenshot(const char* filename)
		{
			SDL_Surface* image = SDL_GetWindowSurface(WindowData.Window);
			Images::Save(filename, image->w, image->h, (unsigned char*)image->pixels);
		}

		void ShowMessageBox(const char* title, const char* message, bool isError)
		{
			SDL_ShowSimpleMessageBox(isError ? SDL_MESSAGEBOX_ERROR : SDL_MESSAGEBOX_INFORMATION, title, message, NULL);
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
	
		char* GetSavePath(const char* org, const char* app, const char* fileName)
		{
			char* basePath = SDL_GetPrefPath(org, app);
			char* result = StringToCharArray(std::string(basePath) + std::string(fileName));
			SDL_free(basePath);
			return result;
		}
	}
}

extern "C"
{
	DLLExport void SPF_Open(const char* title, int w, int h, unsigned int appID)
	{
		SPF::Window::Open(title, w, h, appID);
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

	DLLExport bool SPF_IsFullscreenRequested()
	{
		return SPF::Window::IsFullscreenRequested();
	}

	DLLExport void SPF_SetFullscreen(bool fullscreen)
	{
		SPF::Window::SetFullscreen(fullscreen);
	}

	DLLExport void SPF_SetVSync(bool enabled)
	{
		SPF::Window::SetVSync(enabled);
	}

	DLLExport int SPF_GetWindowWidth()
	{
		return SPF::Window::GetWidth();
	}

	DLLExport int SPF_GetWindowHeight()
	{
		return SPF::Window::GetHeight();
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

	DLLExport void SPF_ShowMessageBox(const char* title, const char* message, bool isError)
	{
		SPF::Window::ShowMessageBox(title, message, isError);
	}

	DLLExport char* SPF_GetSavePath(const char* org, const char* app, const char* fileName)
	{
		return SPF::Window::GetSavePath(org, app, fileName);
	}
}