#pragma once
#include "Core.h"

namespace SPF
{
	class Window
	{
	private:
		int mWindowWidth;
		int mWindowHeight;
		OpaquePointer mOpenGLContext;
		OpaquePointer mWindow;
		unsigned int mLastTick;

	public:
		void Open(const char* title, int w, int h);
		bool BeginLoop(float* dt);
		void EndLoop();
		void Close();
		void SetFullscreen(bool fullscreen);

		void SetWindowSize(int w, int h);

		inline int GetWidth() const
		{
			return mWindowWidth;
		}

		inline int GetHeight() const
		{
			return mWindowHeight;
		}
	};

	extern Window mWindow;
}