#pragma once
#include "Core.h"

namespace SPF
{
	enum class BlendMode : int
	{
		Alpha,
		Additive
	};

	constexpr uint32_t MaxSprites = 2000;
	constexpr uint32_t VerticesPerSprite = 4;
	constexpr uint32_t VerticesCount = MaxSprites * VerticesPerSprite;

	class Renderer
	{
	private:
		Vertex mVertices[VerticesCount];
		HardwareID mBatchVBOID;
		HardwareID mEmptyTexture;
		HardwareID mProgram;
		ResourceIndex mFinalSurface;
		struct
		{
			int VertexCount;
			HardwareID CurrentTexture;
		} mBatchInfo;

		void Prepare();
		void IssueVertices();

		void PushVertex(HardwareID texture,
			float x, float y, float z,
			float u, float v, float bu, float bv,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA);

		int mCurrentWidth;
		int mCurrentHeight;
		float mMVP[16];
		float mCameraSideX = 0.f, mCameraSideY = 0.f, mCameraSideZ = 0.f;
		float mFogIntensity = 0.f;

	public:
		void Begin(ResourceIndex surface);
		void BeginLookAtPerspective(ResourceIndex surface,
			float cameraX, float cameraY, float cameraZ,
			float cameraTargetX, float cameraTargetY, float cameraTargetZ,
			float fov, float nearDist, float farDist, float fogIntensity);
		void DrawFinalSurface(int w, int h);
		void Init(int w, int h);
		void FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a);
		void DrawTexturedQuad(
			ResourceIndex tex,
			float Ax, float Ay, float Az,
			float Bx, float By, float Bz,
			float Cx, float Cy, float Cz,
			float Dx, float Dy, float Dz,
			int srcx, int srcy, int srcw, int srch,
			bool flipX, bool flipY,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA);
		void DrawMesh(ResourceIndex tex, ResourceIndex mesh);
		void SetBlending(BlendMode blendMode);
		void DrawBillboard(ResourceIndex tex,
			float x, float y, float z, float radius,
			int srcx, int srcy, int srcw, int srch,
			bool flipX, bool flipY,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA);

		inline ResourceIndex GetFinalSurface() const
		{
			return mFinalSurface;
		}
	};

	extern Renderer mRenderer;
}