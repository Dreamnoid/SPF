#pragma once
#include <vector>
#include <Core.h>

namespace SPF
{
	struct Texture
	{
		bool InUse;
		OpaquePointer Pointer;
		OpaquePointer MaskPointer;
		unsigned int Width;
		unsigned int Height;
	};

	struct ResourcesData
	{
		std::vector<Texture> Textures;
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