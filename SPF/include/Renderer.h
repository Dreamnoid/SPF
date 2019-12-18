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
		DLLExport void FillVerticalGradient(
			int x, int y, int w, int h,
			float r1, float g1, float b1, float a1,
			float r2, float g2, float b2, float a2);
		DLLExport void FillHorizontalGradient(
			int x, int y, int w, int h,
			float r1, float g1, float b1, float a1,
			float r2, float g2, float b2, float a2);
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
		DLLExport void DrawTexturedTriangle(ResourceIndex tex, Vertex a, Vertex b, Vertex c);
		DLLExport void DrawTexture(
			ResourceIndex tex,
			int x, int y, int w, int h,
			int srcx, int srcy, int srcw, int srch,
			bool flipX, bool flipY,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA);
		DLLExport void DrawMesh(
			ResourceIndex tex, ResourceIndex mesh, 
			const float* world,
			float overlayR, float overlayG, float overlayB, float overlayA);
		DLLExport void DrawMesh(
			ResourceIndex tex, ResourceIndex mesh, 
			int first, int count, 
			const float* world, 
			float overlayR, float overlayG, float overlayB, float overlayA);
		DLLExport void SetBlending(BlendMode blendMode);
		DLLExport void DrawBillboard(ResourceIndex tex,
			float x, float y, float z, 
			float width, float height,
			int srcx, int srcy, int srcw, int srch,
			bool flipX, bool flipY,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA);
		DLLExport void SetWireframe(bool wireframeEnabled);
		DLLExport void SetBackfaceCulling(bool cullingEnabled);
		DLLExport void SetFogColor(float r, float g, float b);
		ResourceIndex GetFinalSurface();
	}
}