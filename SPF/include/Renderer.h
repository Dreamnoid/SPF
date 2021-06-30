#pragma once
#include "Core.h"
#include "RenderStates.h"

namespace SPF
{
	enum class PrimitiveType
	{
		Quad,
		Triangle,
		Line
	};

	namespace Renderer
	{
		DLLExport void Begin(ResourceIndex surface, bool clear);

		DLLExport void SetCamera(const RenderState::Camera& camera);

		DLLExport void SetMaterial(const RenderState::Material& material);

		DLLExport void SetFog(const RenderState::Fog& fog);

		DLLExport void SetUserData(const RenderState::UserData& userData);

		DLLExport void SetRasterization(const RenderState::Rasterization& rasterization);

		DLLExport void SetBuffers(const RenderState::Buffers& buffers);

		DLLExport void SetStencil(const RenderState::Stencil& stencil);

		DLLExport void SetPrimitiveType(PrimitiveType type);

		DLLExport void PushVertex(const Vertex& v);

		DLLExport void DrawLine(const Vector3& from, const Vector3& to, const RGBA& color, float width);

		DLLExport void DrawMesh(ResourceIndex mesh, int first, int count, const RenderState::ModelData& modelData);

		DLLExport ResourceIndex GetFinalSurface();

		DLLExport ResourceIndex GetDefaultShader();

		void Init(int w, int h);

		void Resize(int w, int h);

		void DrawFinalSurface(int w, int h);
	}
}