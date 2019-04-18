#pragma once
#include "Core.h"

enum class BlendMode : int
{
	Alpha,
	Additive
};

struct Vertex
{
	float X;
	float Y;
	float U;
	float V;
	float R;
	float G;
	float B;
	float A;
	float OverlayR;
	float OverlayG;
	float OverlayB;
	float OverlayA;
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

	void IssueVertices();
	void PushVertex(HardwareID texture, int x, int y, float u, float v,
		float r, float g, float b, float a,
		float overlayR, float overlayG, float overlayB, float overlayA);

	int mCurrentWidth;
	int mCurrentHeight;

public:
	void Begin(ResourceIndex surface);
	void DrawFinalSurface(int w, int h);
	void End();
	void Init(int w, int h);
	void FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a);
	void DrawTexturedQuad(
		int tex,
		float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Dx, float Dy,
		int srcx, int srcy, int srcw, int srch,
		bool flipX, bool flipY,
		float r, float g, float b, float a,
		float overlayR, float overlayG, float overlayB, float overlayA);
	void SetBlending(BlendMode blendMode);

};

extern Renderer mRenderer;