#pragma once
#include <vector>
#include <Core.h>

namespace SPF
{
	struct Mesh
	{
		bool InUse = false;
		HardwareID GLID;
		int VerticesCount = 0;
	};

	struct Surface
	{
		bool InUse;
		HardwareID GLID;
		HardwareID DepthGLID;
		ResourceIndex Texture;
		bool HasDepth;
	};

	struct Texture
	{
		bool InUse;
		HardwareID GLID;
		unsigned int Width;
		unsigned int Height;
		bool Flipped;
	};

	struct ResourcesData
	{
		std::vector<Mesh> Meshes;
		std::vector<Surface> Surfaces;
		std::vector<Texture> Textures;
	};
	extern ResourcesData Resources;
}