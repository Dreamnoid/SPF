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
		DLLExport void SetWindowTitle(const char* title);
		DLLExport inline int GetWidth();
		DLLExport int GetHeight();
		DLLExport bool HasFocus();
		DLLExport void SaveScreenshot(const char* filename);
	}
}