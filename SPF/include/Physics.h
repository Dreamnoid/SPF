#pragma once
#include "Core.h"

namespace SPF
{
	namespace Physics
	{
		DLLExport void Start();
		DLLExport void Update(float dt);
		DLLExport void Delete(ResourceIndex bodyID);
		DLLExport void Stop();
		DLLExport ResourceIndex CreateMesh(Vector3* vertices, int count);
		DLLExport ResourceIndex CreateCapsule(float radius, float height);
		DLLExport ResourceIndex CreateSphere(float radius, bool ccd);
		DLLExport void SetVelocity(ResourceIndex bodyID, float x, float y, float z);
		DLLExport void SetPosition(ResourceIndex bodyID, float x, float y, float z);
		DLLExport Vector3 GetPosition(ResourceIndex bodyID);
		DLLExport bool Raycast(
			float srcX, float srcY, float srcZ,
			float destX, float destY, float destZ,
			float* dist, int* triangleIndex);
		DLLExport bool IsGrounded(ResourceIndex bodyID, float treshold);
		DLLExport bool CollidedWithWalls(ResourceIndex bodyID, Vector3* outNormal);
	}
}