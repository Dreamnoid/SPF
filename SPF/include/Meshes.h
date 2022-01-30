#pragma once
#include <vector>
#include <Core.h>

namespace SPF
{
	namespace Meshes
	{
		DLLExport ResourceIndex Load(Vertex* vertices, int count, bool dynamic = false);
		DLLExport void Update(ResourceIndex mesh, Vertex* vertices, int count);
		DLLExport void Bind(ResourceIndex mesh);
		DLLExport void Delete(ResourceIndex mesh);
	};
}
