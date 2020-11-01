#pragma once
#include "Core.h"

namespace SPF
{
	enum class BlendMode : int
	{
		Alpha,
		Additive
	};

	struct TextureSet
	{
		ResourceIndex Texture1;
		ResourceIndex Texture2;
		ResourceIndex Texture3;
	};

	namespace Renderer
	{
		DLLExport void Begin(ResourceIndex surface);

		DLLExport void BeginLookAtPerspective(ResourceIndex surface,
			const Vector3& cameraPosition,
			const Vector3& cameraTarget,
			float fov, float nearDist, float farDist, float fogIntensity);

		DLLExport void BeginOrthographic(ResourceIndex surface,
			float minX, float maxX,
			float minY, float maxY,
			float minZ, float maxZ, float fogIntensity);

		DLLExport void DrawFinalSurface(int w, int h);
		DLLExport void Init(int w, int h);
		DLLExport void Resize(int w, int h);

		DLLExport void FillRectangle(const Rect& dest, const RGBA& color);

		DLLExport void FillVerticalGradient(const Rect& dest, const RGBA& topColor, const RGBA& bottomColor);

		DLLExport void FillHorizontalGradient(const Rect& dest, const RGBA& leftColor, const RGBA& rightColor);

		DLLExport void DrawTexturedQuad(
			ResourceIndex tex,
			const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d,
			const Rect& src,
			bool flipX, bool flipY,
			const RGBA& aColor, const RGBA& bColor, const RGBA& cColor, const RGBA& dColor,
			const RGBA& overlay);

		DLLExport void DrawTexturedTriangle(ResourceIndex tex, const Vertex& a, const Vertex& b, const Vertex& c);

		DLLExport void DrawTexture(
			ResourceIndex tex,
			const Rect& dest,
			const Rect& src,
			bool flipX, bool flipY,
			const RGBA& color,
			const RGBA& overlay);

		DLLExport void DrawMesh(
			ResourceIndex tex, ResourceIndex mesh, 
			const float* world,
			const RGBA& overlay);

		DLLExport void DrawMesh(
			ResourceIndex shader, const TextureSet& texSet,
			ResourceIndex mesh, int first, int count, 
			const float* world, 
			const RGBA& overlay);

		DLLExport void DrawLine(const Vector3& from, const Vector3& to, const RGBA& color, float width = 1.0f);

		DLLExport void SetBlending(BlendMode blendMode);

		DLLExport void DrawBillboard(ResourceIndex tex,
			const Vector3& center, 
			float width, float height,
			const Rect& src,
			bool flipX, bool flipY,
			const RGBA& color, const RGBA& overlay);

		DLLExport void SetWireframe(bool wireframeEnabled);

		DLLExport void SetBackfaceCulling(int culling);

		DLLExport void SetFogColor(const RGB& color);

		DLLExport void SetAnimation(float animation);

		DLLExport void SetUserData(const Vector4& userData);

		ResourceIndex GetFinalSurface();

		ResourceIndex GetDefaultShader();

	}
}