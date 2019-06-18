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

			for (ResourceIndex meshID = 0; meshID < Resources.Meshes.size(); ++meshID)
			{
				if (!Resources.Meshes[meshID].InUse)
				{
					Resources.Meshes[meshID] = { true,id,count };
					return meshID;
				}
			}
			Resources.Meshes.push_back({ true,id,count });
			return Resources.Meshes.size() - 1;
		}

		void Meshes::Delete(ResourceIndex mesh)
		{
			Resources.Meshes[mesh].InUse = false;
			glDeleteBuffers(1, &Resources.Meshes[mesh].GLID);
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
