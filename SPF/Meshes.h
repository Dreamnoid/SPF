#pragma once
#include "Core.h"

struct Mesh
{
	bool InUse = false;
	HardwareID GLID;
	int VerticesCount;
};

constexpr uint32_t MeshesCount = 10;

class Meshes
{
private:
	Mesh mMeshes[MeshesCount];

public:
	ResourceIndex Load(Vertex* vertices, int count);
	void Delete(ResourceIndex mesh);

	inline const Mesh& Get(ResourceIndex mesh) const
	{
		return mMeshes[mesh];
	}

};

extern Meshes mMeshes;

