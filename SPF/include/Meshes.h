#pragma once
#include <vector>
#include "Core.h"

namespace SPF
{
	struct Mesh
	{
		bool InUse = false;
		HardwareID GLID;
		int VerticesCount;
	};

	class Meshes
	{
	private:
		std::vector<Mesh> mMeshes;

	public:
		Meshes() :mMeshes(10) {};
		ResourceIndex Load(Vertex* vertices, int count);
		void Delete(ResourceIndex mesh);

		inline const Mesh& Get(ResourceIndex mesh) const
		{
			return mMeshes[mesh];
		}

	};

	extern Meshes mMeshes;
}
