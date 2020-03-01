#pragma once
#include <vector>
#include <Core.h>

namespace SPF
{
	namespace Shaders
	{
		void Init();
		DLLExport ResourceIndex Create(const char* sourceCode);
		DLLExport void Delete(ResourceIndex shader);
	};
}
