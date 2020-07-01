#include <Core.h>
#include <SDL.h>

namespace SPF
{
	void FatalError(const char* msg)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", msg, NULL);
		exit(1);
	}

	Rect Rect::Unit = { 0,0,1,1 };
	Vector3 Vector3::Zero = { 0.f, 0.f, 0.f };
	Vector3 Vector3::Up = { 0.f, 1.f, 0.f };
	Vector2 Vector2::Zero = { 0.f, 0.f };
	RGBA RGBA::White = { 1.f, 1.f, 1.f, 1.f };
	RGBA RGBA::TransparentBlack = { 0.f, 0.f, 0.f, 0.f };

}

extern "C"
{
	DLLExport void SPF_FatalError(const char* msg)
	{
		SPF::FatalError(msg);
	}
}