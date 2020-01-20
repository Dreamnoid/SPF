#include <vector>
#include <btBulletDynamicsCommon.h>
#include <Physics.h>

namespace SPF
{
	struct PhysicsBody
	{
		bool InUse;
		btCollisionShape* Shape;
		btRigidBody* Body;
		btTriangleMesh* Mesh;
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


	namespace Physics
	{

		void Start()
		{
			PhysicsData.Configuration = new btDefaultCollisionConfiguration();
			PhysicsData.Dispatcher = new btCollisionDispatcher(PhysicsData.Configuration);
			PhysicsData.Broadphase = new btDbvtBroadphase();
			PhysicsData.Solver = new btSequentialImpulseConstraintSolver;
			PhysicsData.World = new btDiscreteDynamicsWorld(PhysicsData.Dispatcher, PhysicsData.Broadphase, PhysicsData.Solver, PhysicsData.Configuration);
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
			PhysicsData.Bodies[bodyID] = { false, nullptr, nullptr, nullptr };
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
				body->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));

			for (ResourceIndex bodyID = 0; bodyID < PhysicsData.Bodies.size(); ++bodyID)
			{
				if (!PhysicsData.Bodies[bodyID].InUse)
				{
					PhysicsData.Bodies[bodyID] = { true, shape, body, nullptr };
					return bodyID;
				}
			}
			PhysicsData.Bodies.push_back({ true, shape, body, nullptr });
			return PhysicsData.Bodies.size() - 1;
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

		void SetVelocity(ResourceIndex bodyID, float x, float y, float z)
		{
			PhysicsBody& body = PhysicsData.Bodies[bodyID];
			body.Body->activate();
			body.Body->setLinearVelocity(btVector3(x, y, z));
		}

		void SetPosition(ResourceIndex bodyID, float x, float y, float z)
		{
			PhysicsBody& body = PhysicsData.Bodies[bodyID];
			body.Body->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));

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
			float* dist)
		{
			btVector3 from(srcX, srcY, srcZ);
			btVector3 to(destX, destY, destZ);

			btCollisionWorld::ClosestRayResultCallback callback(from, to);
			callback.m_collisionFilterMask = btBroadphaseProxy::StaticFilter;
			PhysicsData.World->rayTest(from, to, callback);
			*dist = callback.m_closestHitFraction;
			return callback.hasHit();
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
		float* dist)
	{
		return SPF::Physics::Raycast(srcX, srcY, srcZ, destX, destY, destZ, dist);
	}
}