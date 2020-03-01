#include <Meshes.h>
#include <vector>
#include "gl4.h"
#include "Resources.h"

namespace SPF
{
	namespace Meshes
	{
		ResourceIndex Meshes::Load(Vertex* vertices, int count)
		{
			GLuint id;
			glGenBuffers(1, &id);
			glBindBuffer(GL_ARRAY_BUFFER, id);
			glBufferData(GL_ARRAY_BUFFER, count * sizeof(Vertex), vertices, GL_STATIC_DRAW);

			return CreateResource(Resources.Meshes, { true,id,count });
		}

		void Meshes::Delete(ResourceIndex mesh)
		{
			glDeleteBuffers(1, &Resources.Meshes[mesh].GLID);
			DeleteResource(Resources.Meshes, mesh);
		}
	}
}

extern "C"
{
	DLLExport int SPF_LoadMesh(SPF::Vertex* vertices, int count)
	{
		return SPF::Meshes::Load(vertices, count);
	}

	DLLExport void SPF_DeleteMesh(int mesh)
	{
		SPF::Meshes::Delete(mesh);
	}
}
