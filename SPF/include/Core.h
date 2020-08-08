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

	struct Rect
	{
		int X, Y, Width, Height;

		static Rect Unit;
	};

	struct Vector3
	{
		float X, Y, Z;

		static Vector3 Zero;
		static Vector3 Up;
	};

	struct Vector4
	{
		float X, Y, Z, W;
	};

	struct Vector2
	{
		float X, Y;

		static Vector2 Zero;
	};

	struct RGB
	{
		float R, G, B;
	};

	struct RGBA
	{
		float R, G, B, A;

		static RGBA White;
		static RGBA TransparentBlack;
	};

	struct Vertex
	{
		Vector3 Position;
		Vector3 Normal;
		Vector2 UV;
		Vector2 BUV;
		RGBA Color;
		RGBA Overlay;
	};

}