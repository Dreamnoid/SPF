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
		DLLExport void SetClipboard(const char* text);
		DLLExport char* GetClipboard();
		DLLExport Size GetDesktopSize();
		DLLExport void ShowMessageBox(const char* title, const char* message, bool isError);
		DLLExport char* OpenFileDialog(const char* defaultFolder, const char* filter);
		DLLExport char* SaveFileDialog(const char* defaultFolder, const char* filter);
	}
}