#include "Core.h"
#include <SDL.h>

void FatalError(const char* msg)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", msg, NULL);
}