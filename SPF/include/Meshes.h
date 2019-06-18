#pragma once
#include <vector>
#include <Core.h>

namespace SPF
{
	namespace Meshes
	{
		DLLExport ResourceIndex Load(Vertex* vertices, int count);
		DLLExport void Delete(ResourceIndex mesh);
	};
}
