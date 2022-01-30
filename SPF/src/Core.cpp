#include <Core.h>
#include <SDL.h>
#include <string.h>

namespace SPF
{
	void FatalError(const char* msg)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", msg, NULL);
		exit(1);
	}

	Rect Rect::Unit = { 0,0,1,1 };
	Vector3 Vector3::Zero = { 0.f, 0.f, 0.f };
	Vector3 Vector3::Up = { 0.f, 1.f, 0.f };
	Vector2 Vector2::Zero = { 0.f, 0.f };
	RGBA RGBA::White = { 1.f, 1.f, 1.f, 1.f };
	RGBA RGBA::TransparentBlack = { 0.f, 0.f, 0.f, 0.f };
	RGB RGB::Black = { 0.f, 0.f, 0.f };

	Matrix::Matrix()
	{
		M[0] = 1.f;
		M[1] = 0.f;
		M[2] = 0.f;
		M[3] = 0.f;
		M[4] = 0.f;
		M[5] = 1.f;
		M[6] = 0.f;
		M[7] = 0.f;
		M[8] = 0.f;
		M[9] = 0.f;
		M[10] = 1.f;
		M[11] = 0.f;
		M[12] = 0.f;
		M[13] = 0.f;
		M[14] = 0.f;
		M[15] = 1.f;
	}

	Matrix::Matrix(const float* other)
	{
		memcpy(&M, other, 16 * sizeof(float));
	}

	Matrix Matrix::Identity = {};

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
}

extern "C"
{
	DLLExport void SPF_FatalError(const char* msg)
	{
		SPF::FatalError(msg);
	}
}