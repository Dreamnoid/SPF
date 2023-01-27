#include <Meshes.h>
#include <vector>
#include "GL4.h"
#include "Resources.h"

namespace SPF
{
	namespace Meshes
	{
		constexpr void* BufferOffset(int i)
		{
			return ((char*)NULL + (i));
		}

		ResourceIndex Load(Vertex* vertices, int count, bool dynamic)
		{
			GLuint vao;
			glGenVertexArrays(1, &vao);

			GLuint vbo;
			glGenBuffers(1, &vbo);

			glBindVertexArray(vao);

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, count * sizeof(Vertex), vertices, dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			glEnableVertexAttribArray(4);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BufferOffset(0 * sizeof(float))); // Position
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), BufferOffset(3 * sizeof(float))); // Normal
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BufferOffset(6 * sizeof(float))); // UVs 1&2
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BufferOffset(10 * sizeof(float))); // Color
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BufferOffset(14 * sizeof(float))); // Overlay

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			return CreateResource(Resources.Meshes, { true, vao, vbo, count });
		}

		void Update(ResourceIndex mesh, Vertex* vertices, int count)
		{
			const Mesh& meshDef = Resources.Meshes[mesh];
			glBindBuffer(GL_ARRAY_BUFFER, meshDef.VertexBufferGLID);
			glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(Vertex), vertices);
		}

		void Bind(ResourceIndex mesh)
		{
			glBindVertexArray(Resources.Meshes[mesh].VertexArrayGLID);
		}

		void Delete(ResourceIndex mesh)
		{
			const Mesh& meshDef = Resources.Meshes[mesh];
			glDeleteVertexArrays(1, &meshDef.VertexArrayGLID);
			glDeleteBuffers(1, &meshDef.VertexBufferGLID);
			DeleteResource(Resources.Meshes, mesh);
		}
	}
}

extern "C"
{
	DLLExport int SPF_LoadMesh(SPF::Vertex* vertices, int count, bool dynamic)
	{
		return SPF::Meshes::Load(vertices, count, dynamic);
	}

	DLLExport void SPF_UpdateMesh(int mesh, SPF::Vertex* vertices, int count)
	{
		SPF::Meshes::Bind(mesh);
		SPF::Meshes::Update(mesh, vertices, count);
	}

	DLLExport void SPF_DeleteMesh(int mesh)
	{
		SPF::Meshes::Delete(mesh);
	}
}
