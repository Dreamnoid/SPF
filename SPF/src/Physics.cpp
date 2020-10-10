#include <vector>
#include <btBulletDynamicsCommon.h>
#include <Physics.h>
#include "Resources.h"

namespace SPF
{
	struct PhysicsBody
	{
		bool InUse;
		btCollisionShape* Shape;
		btRigidBody* Body;
		btTriangleMesh* Mesh;
		float ContactUpFactor;
		bool CollidedWithWalls;
		btVector3 WallCollisionNormal;
	};

	struct
	{
		btDefaultCollisionConfiguration* Configuration;
		btCollisionDispatcher* Dispatcher;
		btBroadphaseInterface* Broadphase;
		btSequentialImpulseConstraintSolver* Solver;
		btDiscreteDynamicsWorld* World;
		std::vector<PhysicsBody> Bodies;

	} PhysicsData;

	struct ClosestRayResultCallbackEx : public btCollisionWorld::ClosestRayResultCallback
	{
		int m_hitTriangleIndex;

		ClosestRayResultCallbackEx(const btVector3& rayFrom, const btVector3& rayTo)
			: btCollisionWorld::ClosestRayResultCallback(rayFrom, rayTo), m_hitTriangleIndex(0){}

		virtual ~ClosestRayResultCallbackEx(){}

		virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
		{
			m_hitTriangleIndex = (rayResult.m_localShapeInfo) ? rayResult.m_localShapeInfo->m_triangleIndex : 0;
			return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
		}
	};

	namespace Physics
	{
		void TickCallback(btDynamicsWorld* world, btScalar timeStep)
		{
			int numManifolds = PhysicsData.Dispatcher->getNumManifolds();
			btVector3 upVector(0.0f, 1.0f, 0.0f);

			for (ResourceIndex bodyID = 0; bodyID < PhysicsData.Bodies.size(); ++bodyID)
			{
				PhysicsBody& body = PhysicsData.Bodies[bodyID];
				if (body.InUse)
				{
					body.ContactUpFactor = -1;
					body.CollidedWithWalls = false;
					for (int i = 0; i < numManifolds; ++i)
					{
						btPersistentManifold* contactManifold = PhysicsData.Dispatcher->getManifoldByIndexInternal(i);
						const btCollisionObject* objA = contactManifold->getBody0();
						const btCollisionObject* objB = contactManifold->getBody1();
						if (objA == body.Body || objB == body.Body)
						{
							int numContacts = contactManifold->getNumContacts();
							for (int j = 0; j < numContacts; ++j)
							{
								const btManifoldPoint& pt = contactManifold->getContactPoint(j);
								//if (pt.getDistance() < 0.0f)
								{
									btVector3 normal = (objB == body.Body) ? -pt.m_normalWorldOnB : pt.m_normalWorldOnB;
									float groundAngle = normal.dot(upVector);
									body.ContactUpFactor = groundAngle > body.ContactUpFactor ? groundAngle : body.ContactUpFactor;
									if (groundAngle > -0.5f && groundAngle < 0.5f)
									{
										body.CollidedWithWalls = true;
										body.WallCollisionNormal = normal;
									}
								}
							}
						}
					}
				}
			}
		}

		void Start()
		{
			PhysicsData.Configuration = new btDefaultCollisionConfiguration();
			PhysicsData.Dispatcher = new btCollisionDispatcher(PhysicsData.Configuration);
			PhysicsData.Broadphase = new btDbvtBroadphase();
			PhysicsData.Solver = new btSequentialImpulseConstraintSolver;
			PhysicsData.World = new btDiscreteDynamicsWorld(PhysicsData.Dispatcher, PhysicsData.Broadphase, PhysicsData.Solver, PhysicsData.Configuration);
			PhysicsData.World->setInternalTickCallback(TickCallback);
			PhysicsData.World->setGravity(btVector3(0, 0, 0));
		}

		void Update(float dt)
		{
			PhysicsData.World->stepSimulation(dt, 10);
		}

		void Delete(ResourceIndex bodyID)
		{
			btCollisionShape* shape = PhysicsData.Bodies[bodyID].Shape;
			btRigidBody* body = PhysicsData.Bodies[bodyID].Body;
			btTriangleMesh* mesh = PhysicsData.Bodies[bodyID].Mesh;
			DeleteResource(PhysicsData.Bodies, bodyID);
			PhysicsData.World->removeRigidBody(body);
			delete body;
			delete shape;
			if (mesh)
				delete mesh;
		}

		void Stop()
		{
			for (ResourceIndex bodyID = 0; bodyID < PhysicsData.Bodies.size(); ++bodyID)
			{
				if (PhysicsData.Bodies[bodyID].InUse)
				{
					Delete(bodyID);
				}
			}

			delete PhysicsData.World;
			delete PhysicsData.Solver;
			delete PhysicsData.Broadphase;
			delete PhysicsData.Dispatcher;
			delete PhysicsData.Configuration;
		}

		ResourceIndex CreateBody(btCollisionShape* shape, float mass)
		{
			btTransform startTransform;
			startTransform.setIdentity();
			startTransform.setOrigin(btVector3(0, 0, 0));

			btScalar btMass(mass);
			bool isDynamic = (mass > 0.0f);

			btVector3 localInertia(0, 0, 0);
			if (isDynamic)
				shape->calculateLocalInertia(btMass, localInertia);

			btRigidBody::btRigidBodyConstructionInfo rbInfo(btMass, nullptr, shape, localInertia);
			btRigidBody* body = new btRigidBody(rbInfo);

			PhysicsData.World->addRigidBody(body);

			if (isDynamic)
			{
				body->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));
				body->setRestitution(0.0f);
				body->setActivationState(DISABLE_DEACTIVATION);
			}

			return CreateResource(PhysicsData.Bodies, { true, shape, body, nullptr, -1.0f });
		}

		ResourceIndex CreateMesh(Vector3* vertices, int count)
		{
			btTriangleMesh* mesh = new btTriangleMesh();
			int index = 0;
			for (int i = 0; i < count/3; ++i)
			{
				const Vector3& a = vertices[(i * 3) + 0];
				const Vector3& b = vertices[(i * 3) + 1];
				const Vector3& c = vertices[(i * 3) + 2];
				mesh->addTriangle(
					btVector3(a.X, a.Y, a.Z),
					btVector3(b.X, b.Y, b.Z),
					btVector3(c.X, c.Y, c.Z));
			}
			
			btCollisionShape* shape = new btBvhTriangleMeshShape(mesh, false);
			ResourceIndex bodyID = CreateBody(shape, 0.0f);
			PhysicsData.Bodies[bodyID].Mesh = mesh;
			return bodyID;
		}

		ResourceIndex CreateCapsule(float radius, float height)
		{
			btCollisionShape* shape = new btCapsuleShape(btScalar(radius), btScalar(height));
			return CreateBody(shape, 1.0f);
		}

		ResourceIndex CreateSphere(float radius, bool ccd)
		{
			btCollisionShape* shape = new btSphereShape(btScalar(radius));
			ResourceIndex bodyID = CreateBody(shape, 1.0f);
			if (ccd)
			{
				PhysicsData.Bodies[bodyID].Body->setCcdMotionThreshold(btScalar(1e-7));
				PhysicsData.Bodies[bodyID].Body->setCcdSweptSphereRadius(btScalar(radius));
			}
			return bodyID;
		}

		void SetVelocity(ResourceIndex bodyID, float x, float y, float z)
		{
			PhysicsBody& body = PhysicsData.Bodies[bodyID];
			body.Body->setLinearVelocity(btVector3(x, y, z));
		}

		void SetPosition(ResourceIndex bodyID, float x, float y, float z)
		{
			PhysicsBody& body = PhysicsData.Bodies[bodyID];

			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(btVector3(x, y, z));

			body.Body->setWorldTransform(transform);
		}

		Vector3 GetPosition(ResourceIndex bodyID)
		{
			PhysicsBody& body = PhysicsData.Bodies[bodyID];
			const btVector3& position = body.Body->getCenterOfMassPosition();
			return { position.x(), position.y(), position.z() };
		}

		bool Raycast(
			float srcX, float srcY, float srcZ,
			float destX, float destY, float destZ,
			float* dist, int* triangleIndex)
		{
			btVector3 from(srcX, srcY, srcZ);
			btVector3 to(destX, destY, destZ);

			ClosestRayResultCallbackEx callback(from, to);
			callback.m_collisionFilterMask = btBroadphaseProxy::StaticFilter;
			PhysicsData.World->rayTest(from, to, callback);
			*dist = callback.m_closestHitFraction;
			*triangleIndex = callback.m_hitTriangleIndex;
			callback.m_hitNormalWorld;
			return callback.hasHit();
		}

		bool IsGrounded(ResourceIndex bodyID, float treshold)
		{
			PhysicsBody& body = PhysicsData.Bodies[bodyID];
			return (body.ContactUpFactor > treshold);
		}

		bool CollidedWithWalls(ResourceIndex bodyID, Vector3* outNormal)
		{
			PhysicsBody& body = PhysicsData.Bodies[bodyID];
			if (body.CollidedWithWalls)
			{
				*outNormal = 
				{
					body.WallCollisionNormal.x(),
					body.WallCollisionNormal.y(),
					body.WallCollisionNormal.z()
				};
				return true;
			}
			*outNormal = Vector3::Zero;
			return false;
		}
	}

}

extern "C"
{
	DLLExport void SPF_StartPhysics()
	{
		return SPF::Physics::Start();
	}

	DLLExport void SPF_UpdatePhysics(float dt)
	{
		SPF::Physics::Update(dt);
	}

	DLLExport void SPF_StopPhysics()
	{
		SPF::Physics::Stop();
	}

	DLLExport int SPF_CreateMeshBody(SPF::Vector3* vertices, int count)
	{
		return SPF::Physics::CreateMesh(vertices, count);
	}

	DLLExport int SPF_CreateCapsuleBody(float radius, float height)
	{
		return SPF::Physics::CreateCapsule(radius, height);
	}

	DLLExport int SPF_CreateSphereBody(float radius, int ccd)
	{
		return SPF::Physics::CreateSphere(radius, ccd);
	}

	DLLExport void SPF_DeleteBody(int bodyID)
	{
		SPF::Physics::Delete(bodyID);
	}

	DLLExport void SPF_SetBodyVelocity(int bodyID, float x, float y, float z)
	{
		SPF::Physics::SetVelocity(bodyID, x, y, z);
	}

	DLLExport void SPF_SetBodyPosition(int bodyID, float x, float y, float z)
	{
		SPF::Physics::SetPosition(bodyID, x, y, z);
	}

	DLLExport SPF::Vector3 SPF_GetBodyPosition(int bodyID)
	{
		return SPF::Physics::GetPosition(bodyID);
	}

	DLLExport int SPF_PhysicsRaycast(
		float srcX, float srcY, float srcZ,
		float destX, float destY, float destZ,
		float* dist, int* triangleIndex)
	{
		return SPF::Physics::Raycast(srcX, srcY, srcZ, destX, destY, destZ, dist, triangleIndex);
	}

	DLLExport int SPF_IsBodyGrounded(int bodyID, float treshold)
	{
		return SPF::Physics::IsGrounded(bodyID, treshold);
	}

	DLLExport int SPF_BodyCollidedWithWalls(int bodyID, SPF::Vector3* outNormal)
	{
		return SPF::Physics::CollidedWithWalls(bodyID, outNormal);
	}
}