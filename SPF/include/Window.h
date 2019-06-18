#pragma once
#include <Core.h>

namespace SPF
{
	namespace Window
	{
		DLLExport void Open(const char* title, int w, int h);
		DLLExport bool BeginLoop(float* dt);
		DLLExport void EndLoop();
		DLLExport void Close();
		DLLExport void SetFullscreen(bool fullscreen);
		DLLExport void SetWindowSize(int w, int h);
		DLLExport inline int GetWidth();
		DLLExport int GetHeight();
	}
}