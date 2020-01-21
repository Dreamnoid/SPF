using System;
using System.Numerics;

namespace SPFSharp
{
	public static partial class SPF
	{
		public static class Physics
		{
			public static void Start() => Native.SPF_StartPhysics();

			public static void Update(float dt) => Native.SPF_UpdatePhysics(dt);

			public static void Stop() => Native.SPF_StopPhysics();

			public static bool Raycast(Vector3 from, Vector3 to, out float dist) => Native.SPF_PhysicsRaycast(from.X, from.Y, from.Z, to.X, to.Y, to.Z, out dist);

			public abstract class Body : IDisposable
			{
				private readonly int _bodyID;

				protected Body(int bodyID)
				{
					_bodyID = bodyID;
				}

				public void Dispose() => Native.SPF_DeleteBody(_bodyID);

				public void SetVelocity(Vector3 velocity) => Native.SPF_SetBodyVelocity(_bodyID, velocity.X, velocity.Y, velocity.Z);

				public void SetPosition(Vector3 velocity) => Native.SPF_SetBodyPosition(_bodyID, velocity.X, velocity.Y, velocity.Z);

				public Vector3 GetPosition() => Native.SPF_GetBodyPosition(_bodyID);

				public bool IsGrounded(float treshold) => Native.SPF_IsBodyGrounded(_bodyID, treshold);
			}

			public class Capsule : Body
			{
				public Capsule(float radius, float height)
					: base(Native.SPF_CreateCapsuleBody(radius, height))
				{
				}
			}

			public class Mesh : Body
			{
				public Mesh(Vector3[] vertices)
					: base(Native.SPF_CreateMeshBody(vertices, vertices.Length))
				{
				}
			}
		}
	}
}
