#pragma once
#include <vector>
#include <Core.h>

namespace SPF
{
	struct Mesh
	{
		bool InUse = false;
		HardwareID VertexArrayGLID;
		HardwareID VertexBufferGLID;
		int VerticesCount = 0;
	};

	struct Surface
	{
		bool InUse;
		HardwareID GLID;
		ResourceIndex ColorTexture;
		ResourceIndex DepthTexture;
		SurfaceFlags Flags;
	};

	struct Texture
	{
		bool InUse;
		HardwareID GLID;
		unsigned int Width;
		unsigned int Height;
		TextureFlags Flags;
	};

	struct Shader
	{
		bool InUse;
		HardwareID GLID;
	};

	struct ResourcesData
	{
		std::vector<Mesh> Meshes;
		std::vector<Surface> Surfaces;
		std::vector<Texture> Textures;
		std::vector<Shader> Shaders;
	};
	extern ResourcesData Resources;

	template<typename T>
	ResourceIndex CreateResource(std::vector<T>& pool, const T& resource)
	{
		for (ResourceIndex resID = 0; resID < (ResourceIndex)pool.size(); ++resID)
		{
			if (!pool[resID].InUse)
			{
				pool[resID] = resource;
				pool[resID].InUse = true;
				return resID;
			}
		}
		pool.push_back(resource);
		pool[pool.size() - 1].InUse = true;
		return pool.size() - 1;
	}

	template<typename T>
	void DeleteResource(std::vector<T>& pool, ResourceIndex resID)
	{
		pool[resID].InUse = false;
	}
}