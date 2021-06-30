using System;
using System.Numerics;

namespace SPFSharp
{
	public static partial class SPF
	{
		public static class Physics
		{
			public static void Start() => Native.Physics.SPF_StartPhysics();

			public static void Update(float dt) => Native.Physics.SPF_UpdatePhysics(dt);

			public static void Stop() => Native.Physics.SPF_StopPhysics();

			public static bool Raycast(Vector3 from, Vector3 to, out float dist, out int triangleIndex) 
				=> Native.Physics.SPF_PhysicsRaycast(from.X, from.Y, from.Z, to.X, to.Y, to.Z, out dist, out triangleIndex);

			public abstract class Body : IDisposable
			{
				private readonly int _bodyID;

				protected Body(int bodyID)
				{
					_bodyID = bodyID;
				}

				public void Dispose() => Native.Physics.SPF_DeleteBody(_bodyID);

				public void SetVelocity(Vector3 velocity) => Native.Physics.SPF_SetBodyVelocity(_bodyID, velocity.X, velocity.Y, velocity.Z);

				public void SetPosition(Vector3 position) => Native.Physics.SPF_SetBodyPosition(_bodyID, position.X, position.Y, position.Z);

				public Vector3 GetPosition() => Native.Physics.SPF_GetBodyPosition(_bodyID);

				public bool IsGrounded(float treshold) => Native.Physics.SPF_IsBodyGrounded(_bodyID, treshold);

				public bool CollidedWithWalls(out Vector3 normal) => Native.Physics.SPF_BodyCollidedWithWalls(_bodyID, out normal);
			}

			public class Capsule : Body
			{
				public Capsule(float radius, float height)
					: base(Native.Physics.SPF_CreateCapsuleBody(radius, height))
				{
				}
			}

			public class Sphere : Body
			{
				public readonly bool CCD;

				public Sphere(float radius, bool ccd)
					: base(Native.Physics.SPF_CreateSphereBody(radius, ccd))
				{
					CCD = ccd;
				}
			}

			public class Mesh : Body
			{
				public Mesh(Vector3[] vertices)
					: base(Native.Physics.SPF_CreateMeshBody(vertices, vertices.Length))
				{
				}
			}
		}
	}
}
