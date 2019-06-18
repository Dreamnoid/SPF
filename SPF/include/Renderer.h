#pragma once
#include "Core.h"

namespace SPF
{
	enum class BlendMode : int
	{
		Alpha,
		Additive
	};

	namespace Renderer
	{
		DLLExport void Begin(ResourceIndex surface);
		DLLExport void BeginLookAtPerspective(ResourceIndex surface,
			float cameraX, float cameraY, float cameraZ,
			float cameraTargetX, float cameraTargetY, float cameraTargetZ,
			float fov, float nearDist, float farDist, float fogIntensity);
		DLLExport void DrawFinalSurface(int w, int h);
		DLLExport void Init(int w, int h);
		DLLExport void FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a);
		DLLExport void DrawTexturedQuad(
			ResourceIndex tex,
			float Ax, float Ay, float Az,
			float Bx, float By, float Bz,
			float Cx, float Cy, float Cz,
			float Dx, float Dy, float Dz,
			int srcx, int srcy, int srcw, int srch,
			bool flipX, bool flipY,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA);
		DLLExport void DrawTexture(
			ResourceIndex tex,
			int x, int y, int w, int h,
			int srcx, int srcy, int srcw, int srch,
			bool flipX, bool flipY,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA);
		DLLExport void DrawMesh(ResourceIndex tex, ResourceIndex mesh);
		DLLExport void SetBlending(BlendMode blendMode);
		DLLExport void DrawBillboard(ResourceIndex tex,
			float x, float y, float z, float radius,
			int srcx, int srcy, int srcw, int srch,
			bool flipX, bool flipY,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA);
		ResourceIndex GetFinalSurface();
	}
}