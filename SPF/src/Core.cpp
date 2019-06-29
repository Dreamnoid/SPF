#include <Core.h>
#include <SDL.h>

namespace SPF
{
	void FatalError(const char* msg)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", msg, NULL);
		exit(1);
	}
}

extern "C"
{
	DLLExport void SPF_FatalError(const char* msg)
	{
		SPF::FatalError(msg);
	}
}