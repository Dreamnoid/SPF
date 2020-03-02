#pragma once
#include <cstdint>

#define DLLExport __declspec(dllexport)

namespace SPF
{
	using OpaquePointer = void*;

	using ResourceIndex = int32_t;
	using HardwareID = uint32_t;
	
	constexpr ResourceIndex InvalidResource = -1;

	DLLExport void FatalError(const char* msg);

	struct Vertex
	{
		float X, Y, Z;
		float U, V, BU, BV;
		float R, G, B, A;
		float OverlayR, OverlayG, OverlayB, OverlayA;
	};

	struct Vector3
	{
		float X, Y, Z;
	};
}