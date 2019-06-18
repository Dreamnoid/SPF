#include <Meshes.h>
#include "gl4.h"

namespace SPF
{
	Meshes mMeshes;

	ResourceIndex Meshes::Load(Vertex* vertices, int count)
	{
		GLuint id;
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(Vertex), vertices, GL_STATIC_DRAW);

		for (ResourceIndex meshID = 0; meshID < mMeshes.size(); ++meshID)
		{
			if (!mMeshes[meshID].InUse)
			{
				mMeshes[meshID] = { true,id,count };
				return meshID;
			}
		}
		mMeshes.push_back({ true,id,count });
		return mMeshes.size() - 1;
	}

	void Meshes::Delete(ResourceIndex mesh)
	{
		mMeshes[mesh].InUse = false;
		glDeleteBuffers(1, &mMeshes[mesh].GLID);
	}
}

extern "C"
{
	DLLExport SPF::ResourceIndex LoadMesh(SPF::Vertex* vertices, int count)
	{
		return SPF::mMeshes.Load(vertices, count);
	}

	DLLExport void DeleteMesh(SPF::ResourceIndex mesh)
	{
		SPF::mMeshes.Delete(mesh);
	}
}