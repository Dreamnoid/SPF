#pragma once
#include <cstdint>

#if defined(_MSC_VER)
	#define DLLExport __declspec(dllexport)
#elif defined(__GNUC__)
	#define DLLExport __attribute__((visibility("default")))
#else
	#define DLLExport
#endif

namespace SPF
{
	using OpaquePointer = void*;

	using ResourceIndex = int32_t;
	using HardwareID = uint32_t;
	
	constexpr ResourceIndex InvalidResource = -1;

	constexpr ResourceIndex UnsetResource = -2;

	DLLExport void FatalError(const char* msg);

	struct Rect
	{
		int X, Y, Width, Height;

		static Rect Unit;
	};

	struct Size
	{
		int Width, Height;
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

		static RGB Black;
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

	struct Matrix
	{
		float M[16];

		Matrix();

		Matrix(const float* other);

		static Matrix Identity;
	};

	inline bool operator!=(const Vector4& a, const Vector4& b)
	{
		return a.X != b.X || a.Y != b.Y || a.Z != b.Z;
	}

	inline bool operator!=(const Matrix& a, const Matrix& b)
	{
		for (int i = 0; i < 16; ++i)
		{
			if (a.M[i] != b.M[i])
				return true;
		}
		return false;
	}

	enum class TextureFlags : int
	{
		None = 0,
		Depth = (1 << 0),
		Flipped = (1 << 1),
		Stencil = (1 << 2),
		MipMap = (1 << 3),
	};

	inline constexpr TextureFlags operator|(TextureFlags x, TextureFlags y)
	{
		return static_cast<TextureFlags>(static_cast<int>(x) | static_cast<int>(y));
	}

	enum class SurfaceFlags : int
	{
		None = 0,
		Color = (1 << 0),
		Depth = (1 << 1),
		Stencil = (1 << 2),

		All = Color | Depth | Stencil
	};

	inline constexpr SurfaceFlags operator|(SurfaceFlags x, SurfaceFlags y)
	{
		return static_cast<SurfaceFlags>(static_cast<int>(x) | static_cast<int>(y));
	}

	template<typename T> inline constexpr bool HasFlag(T x, T y)
	{
		return (static_cast<int>(x) & static_cast<int>(y));
	}
}