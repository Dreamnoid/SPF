#include "Meshes.h"
#include "gl4.h"

Meshes mMeshes;

ResourceIndex Meshes::Load(Vertex* vertices, int count)
{
	GLuint id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(Vertex), vertices, GL_STATIC_DRAW);
	
	for (ResourceIndex meshID = 0; meshID < MeshesCount; ++meshID)
	{
		if (!mMeshes[meshID].InUse)
		{
			mMeshes[meshID].GLID = id;
			mMeshes[meshID].VerticesCount = count;
			mMeshes[meshID].InUse = true;
			return meshID;
		}
	}
	FatalError("All mesh slots are used");
}

void Meshes::Delete(ResourceIndex mesh)
{
	mMeshes[mesh].InUse = false;
	glDeleteBuffers(1, &mMeshes[mesh].GLID);
}

extern "C"
{
	DLLExport ResourceIndex LoadMesh(Vertex* vertices, int count)
	{
		return mMeshes.Load(vertices, count);
	}

	DLLExport void DeleteMesh(ResourceIndex mesh)
	{
		mMeshes.Delete(mesh);
	}
}