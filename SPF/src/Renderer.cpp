#include <Renderer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "gl4.h"
#include <cstdio>
#include <Surfaces.h>
#include <Textures.h>
#include <Meshes.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Resources.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace SPF
{
	constexpr uint32_t MaxSprites = 2000;
	constexpr uint32_t VerticesPerSprite = 4;
	constexpr uint32_t VerticesCount = MaxSprites * VerticesPerSprite;

	struct
	{
		Vertex Vertices[VerticesCount];
		HardwareID BatchVBOID;
		HardwareID EmptyTexture;
		HardwareID Program;
		ResourceIndex FinalSurface;
		struct
		{
			int VertexCount;
			HardwareID CurrentTexture;
		} BatchInfo;

		int CurrentWidth;
		int CurrentHeight;
		glm::mat4 ViewProj;
		glm::mat4 Model;
		float CameraSideX = 0.f, CameraSideY = 0.f, CameraSideZ = 0.f;
		float FogIntensity = 0.f;
		bool Wireframe;
		bool BackfaceCulling;
	} RendererData;

	namespace Renderer
	{

		void SetupOrthographic(float* m, float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
		{
			m[0] = (float)(2.0 / ((double)right - (double)left));
			m[1] = 0.0f;
			m[2] = 0.0f;
			m[3] = 0.0f;
			m[4] = 0.0f;
			m[5] = (float)(2.0 / ((double)top - (double)bottom));
			m[6] = 0.0f;
			m[7] = 0.0f;
			m[8] = 0.0f;
			m[9] = 0.0f;
			m[10] = (float)(1.0 / ((double)zNearPlane - (double)zFarPlane));
			m[11] = 0.0f;
			m[12] = (float)(((double)left + (double)right) / ((double)left - (double)right));
			m[13] = (float)(((double)top + (double)bottom) / ((double)bottom - (double)top));
			m[14] = (float)((double)zNearPlane / ((double)zNearPlane - (double)zFarPlane));
			m[15] = 1.0f;
		}

		unsigned int InternalCompileShader(GLenum type, const char* source)
		{
			auto shader = glCreateShader(type);
			GLint length[1];
			length[0] = strlen(source);
			glShaderSource(shader, 1, &source, length);
			glCompileShader(shader);
			GLint param[1];
			glGetShaderiv(shader, GL_COMPILE_STATUS, param);
			if (param[0] == 0)
			{
				char log[4096];
				int lgt;
				glGetShaderInfoLog(shader, 4096, &lgt, log);
				printf(log);
			}
			return shader;
		}

		unsigned int LinkProgram(unsigned int vert, unsigned int frag)
		{
			auto program = glCreateProgram();
			glAttachShader(program, vert);
			glAttachShader(program, frag);
			glLinkProgram(program);
			GLint param[1];
			glGetProgramiv(program, GL_LINK_STATUS, param);
			if (param[0] == 0)
			{
				char log[4096];
				int lgt;
				glGetProgramInfoLog(program, 4096, &lgt, log);
				printf(log);
			}
			return program;
		}

		unsigned int CompileShader(const char* vertexCode, const char* fragmentCode)
		{
			auto vert = InternalCompileShader(GL_VERTEX_SHADER, vertexCode);
			auto frag = InternalCompileShader(GL_FRAGMENT_SHADER, fragmentCode);
			auto id = LinkProgram(vert, frag);
			glDeleteShader(frag);
			glDeleteShader(vert);
			return id;
		}

		void DeleteShader(unsigned int id)
		{
			glDeleteProgram(id);
		}

		void Init(int w, int h)
		{
			GLuint VAO;
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);

			GLuint ids[1];
			glGenBuffers(1, ids);
			RendererData.BatchVBOID = ids[0];
			glBindBuffer(GL_ARRAY_BUFFER, RendererData.BatchVBOID);
			glBufferData(GL_ARRAY_BUFFER, VerticesCount * sizeof(Vertex), NULL, GL_STREAM_DRAW);

			glGenTextures(1, ids);
			RendererData.EmptyTexture = ids[0];

			glBindTexture(GL_TEXTURE_2D, RendererData.EmptyTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			uint32_t pixels[1];
			pixels[0] = 0xFFFFFFFF;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)pixels);

			RendererData.Program = CompileShader("#version 330 core\n"
				"layout (location = 0) in vec3 in_Position;\n"
				"layout (location = 1) in vec4 in_UV;\n"
				"layout (location = 2) in vec4 in_Color;\n"
				"layout (location = 3) in vec4 in_Overlay;\n"
				"uniform mat4 MVP;\n"
				"uniform vec3 CameraSide;\n"
				"out float share_Distance;\n"
				"out vec2 share_UV;\n"
				"out vec4 share_Color;\n"
				"out vec4 share_Overlay;\n"
				"void main()\n"
				"{\n"
				"	vec3 actualPosition = in_Position + (in_UV.z * CameraSide) + (in_UV.w * vec3(0,1,0));\n"
				"	gl_Position = MVP * vec4(actualPosition,1.0);\n"
				"	share_Distance = min(gl_Position.z / 25.0,1);\n"
				"   share_UV = in_UV.xy;\n"
				"	share_Color = in_Color;\n"
				"	share_Overlay = in_Overlay;\n"
				"}\n"
				,
				"#version 330 core\n"
				"uniform sampler2D Texture;\n"
				"uniform float FogIntensity;\n"
				"in float share_Distance;\n"
				"in vec2 share_UV;\n"
				"in vec4 share_Color;\n"
				"in vec4 share_Overlay;\n"
				"out vec4 out_Color;\n"
				"void main()\n"
				"{\n"
				"	vec4 texColor = texture2D(Texture, share_UV) * share_Color;\n"
				"	if (texColor.a <= 0) discard;\n"
				"	out_Color = mix(texColor, vec4(share_Overlay.xyz, texColor.a), share_Overlay.a);\n"
				"	out_Color = mix(out_Color, vec4(0,0,0,texColor.a), FogIntensity * share_Distance);\n"
				"}\n");

			glUseProgram(RendererData.Program);
			glUniform1i(glGetUniformLocation(RendererData.Program, "Texture"), 0);

			RendererData.FinalSurface = Surfaces::Create(w, h, true);
			RendererData.Model = glm::identity<glm::mat4>();
			RendererData.Wireframe = false;
			RendererData.BackfaceCulling = false;

			glFrontFace(GL_CW);
			glCullFace(GL_BACK);

			glEnable(GL_POLYGON_OFFSET_LINE);
			glPolygonOffset(-1.0f, -1.0f);
		}

		void Prepare()
		{
			glViewport(0, 0, (GLsizei)RendererData.CurrentWidth, (GLsizei)RendererData.CurrentHeight);

			glPolygonMode(GL_FRONT_AND_BACK, (RendererData.Wireframe) ? GL_LINE : GL_FILL);
			if (RendererData.BackfaceCulling)
			{
				glEnable(GL_CULL_FACE);
			}
			else
			{
				glDisable(GL_CULL_FACE);
			}

			glUseProgram(RendererData.Program);
			glm::mat4 mvp = RendererData.ViewProj * RendererData.Model;
			glUniformMatrix4fv(glGetUniformLocation(RendererData.Program, "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
			glUniform3f(glGetUniformLocation(RendererData.Program, "CameraSide"), RendererData.CameraSideX, RendererData.CameraSideY, RendererData.CameraSideZ);
			glUniform1f(glGetUniformLocation(RendererData.Program, "FogIntensity"), RendererData.FogIntensity);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0 * sizeof(float)));
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(3 * sizeof(float)));
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(7 * sizeof(float)));
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(11 * sizeof(float)));
		}

		void IssueVertices()
		{
			if (RendererData.BatchInfo.VertexCount == 0) return;

			glBindBuffer(GL_ARRAY_BUFFER, RendererData.BatchVBOID);
			glBufferSubData(GL_ARRAY_BUFFER, 0, VerticesCount * sizeof(Vertex), &RendererData.Vertices);

			glActiveTexture2(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, RendererData.BatchInfo.CurrentTexture);

			Prepare();
			glDrawArrays(GL_QUADS, 0, RendererData.BatchInfo.VertexCount);
			memset(&RendererData.BatchInfo, 0, sizeof(RendererData.BatchInfo));
		}

		void PushVertex(HardwareID texture,
			float x, float y, float z,
			float u, float v, float bu, float bv,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA)
		{
			if (RendererData.BatchInfo.VertexCount == VerticesCount || texture != RendererData.BatchInfo.CurrentTexture)
			{
				IssueVertices();
				RendererData.BatchInfo.CurrentTexture = texture;
			}
			Vertex* vertex = &RendererData.Vertices[RendererData.BatchInfo.VertexCount];
			vertex->X = x;
			vertex->Y = y;
			vertex->Z = z;
			vertex->U = u;
			vertex->V = v;
			vertex->BU = bu;
			vertex->BV = bv;
			vertex->R = r;
			vertex->G = g;
			vertex->B = b;
			vertex->A = a;
			vertex->OverlayR = overlayR;
			vertex->OverlayG = overlayG;
			vertex->OverlayB = overlayB;
			vertex->OverlayA = overlayA;
			RendererData.BatchInfo.VertexCount++;
		}

		void FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a)
		{
			PushVertex(RendererData.EmptyTexture, (float)x, (float)y, 0.f, 0.f, 0.f, 0.f, 0.f, r, g, b, a, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)(x + w), (float)y, 0.f, 1.f, 0.f, 0.f, 0.f, r, g, b, a, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)(x + w), (float)(y + h), 0.f, 1.f, 1.f, 0.f, 0.f, r, g, b, a, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)x, (float)(y + h), 0.f, 0.f, 1.f, 0.f, 0.f, r, g, b, a, 0.f, 0.f, 0.f, 0.f);
		}

		void FillVerticalGradient(
			int x, int y, int w, int h,
			float r1, float g1, float b1, float a1,
			float r2, float g2, float b2, float a2)
		{
			PushVertex(RendererData.EmptyTexture, (float)x, (float)y, 0.f, 0.f, 0.f, 0.f, 0.f, r1, g1, b1, a1, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)(x + w), (float)y, 0.f, 1.f, 0.f, 0.f, 0.f, r1, g1, b1, a1, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)(x + w), (float)(y + h), 0.f, 1.f, 1.f, 0.f, 0.f, r2, g2, b2, a2, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)x, (float)(y + h), 0.f, 0.f, 1.f, 0.f, 0.f, r2, g2, b2, a2, 0.f, 0.f, 0.f, 0.f);
		}

		void FillHorizontalGradient(
			int x, int y, int w, int h,
			float r1, float g1, float b1, float a1,
			float r2, float g2, float b2, float a2)
		{
			PushVertex(RendererData.EmptyTexture, (float)x, (float)y, 0.f, 0.f, 0.f, 0.f, 0.f, r1, g1, b1, a1, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)(x + w), (float)y, 0.f, 1.f, 0.f, 0.f, 0.f, r2, g2, b2, a2, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)(x + w), (float)(y + h), 0.f, 1.f, 1.f, 0.f, 0.f, r2, g2, b2, a2, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)x, (float)(y + h), 0.f, 0.f, 1.f, 0.f, 0.f, r1, g1, b1, a1, 0.f, 0.f, 0.f, 0.f);
		}
		
		void DrawTexturedQuad(
			ResourceIndex tex,
			float Ax, float Ay, float Az,
			float Bx, float By, float Bz,
			float Cx, float Cy, float Cz,
			float Dx, float Dy, float Dz,
			int srcx, int srcy, int srcw, int srch,
			bool flipX, bool flipY,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA)
		{
			unsigned int id = Resources.Textures[tex].GLID;
			int texW = Resources.Textures[tex].Width;
			int texH = Resources.Textures[tex].Height;
			float u1 = srcx / (float)texW;
			float u2 = (srcx + srcw) / (float)texW;
			float v1 = srcy / (float)texH;
			float v2 = (srcy + srch) / (float)texH;
			if (flipX)
			{
				float t = u1;
				u1 = u2;
				u2 = t;
			}
			if (Resources.Textures[tex].Flipped)
			{
				float t = v1;
				v1 = v2;
				v2 = t;
			}
			if (flipY)
			{
				float t = v1;
				v1 = v2;
				v2 = t;
			}
			PushVertex(id, Ax, Ay, Az, u1, v1, 0.f, 0.f, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
			PushVertex(id, Bx, By, Bz, u2, v1, 0.f, 0.f, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
			PushVertex(id, Cx, Cy, Cz, u2, v2, 0.f, 0.f, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
			PushVertex(id, Dx, Dy, Dz, u1, v2, 0.f, 0.f, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
		}

		void DrawTexture(
			ResourceIndex tex,
			int x, int y, int w, int h,
			int srcx, int srcy, int srcw, int srch,
			bool flipX, bool flipY,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA)
		{
			DrawTexturedQuad(tex,
				(float)x, (float)y, 0.f,
				(float)(x + w), (float)y, 0.f,
				(float)(x + w), (float)(y + h), 0.f,
				(float)x, (float)(y + h), 0.f,
				srcx, srcy, srcw, srch,
				flipX, flipY,
				r, g, b, a,
				overlayR, overlayG, overlayB, overlayA);
		}

		void DrawMesh(ResourceIndex tex, ResourceIndex mesh, const float* world)
		{
			DrawMesh(tex, mesh, 0, Resources.Meshes[mesh].VerticesCount, world);
		}

		void DrawMesh(ResourceIndex tex, ResourceIndex mesh, int first, int count, const float* world)
		{
			IssueVertices();

			glBindBuffer(GL_ARRAY_BUFFER, Resources.Meshes[mesh].GLID);

			glActiveTexture2(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, (tex < 0) ? RendererData.EmptyTexture : Resources.Textures[tex].GLID);

			RendererData.Model = glm::make_mat4x4(world);

			Prepare();
			glDrawArrays(GL_TRIANGLES, first, count);

			RendererData.Model = glm::identity<glm::mat4>();
		}

		void SetBlending(BlendMode blendMode)
		{
			IssueVertices();
			if (blendMode == BlendMode::Alpha)
			{
				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			}
			else if (blendMode == BlendMode::Additive)
			{
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			}
		}

		void Begin(ResourceIndex surface)
		{
			IssueVertices();

			surface = (surface < 0) ? RendererData.FinalSurface : surface;
			ResourceIndex texture = Surfaces::GetTexture(surface);
			glBindFramebuffer(GL_FRAMEBUFFER, Resources.Surfaces[surface].GLID);

			RendererData.CurrentWidth = Resources.Textures[texture].Width;
			RendererData.CurrentHeight = Resources.Textures[texture].Height;
			SetupOrthographic(glm::value_ptr(RendererData.ViewProj), 0, (float)RendererData.CurrentWidth, (float)RendererData.CurrentHeight, 0, -1.0f, 1.0f);
			RendererData.CameraSideX = 0.f;
			RendererData.CameraSideY = 0.f;
			RendererData.CameraSideZ = 0.f;

			glDisable(GL_DEPTH_TEST);
			RendererData.FogIntensity = 0.f;
		}

		void BeginLookAtPerspective(ResourceIndex surface,
			float cameraX, float cameraY, float cameraZ,
			float cameraTargetX, float cameraTargetY, float cameraTargetZ,
			float fov, float nearDist, float farDist, float fogIntensity)
		{
			IssueVertices();

			surface = (surface < 0) ? RendererData.FinalSurface : surface;
			ResourceIndex texture = Surfaces::GetTexture(surface);
			glBindFramebuffer(GL_FRAMEBUFFER, Resources.Surfaces[surface].GLID);

			RendererData.CurrentWidth = Resources.Textures[texture].Width;
			RendererData.CurrentHeight = Resources.Textures[texture].Height;

			auto cameraPos = glm::vec3(cameraX, cameraY, cameraZ);
			auto cameraTarget = glm::vec3(cameraTargetX, cameraTargetY, cameraTargetZ);
			auto cameraForward = glm::normalize(cameraTarget - cameraPos);
			auto cameraSide = glm::cross(cameraForward, glm::vec3(0.f, 1.f, 0.f));
			RendererData.CameraSideX = cameraSide.x;
			RendererData.CameraSideY = cameraSide.y;
			RendererData.CameraSideZ = cameraSide.z;

			RendererData.ViewProj = 
				glm::perspectiveFov(fov, (float)RendererData.CurrentWidth, (float)RendererData.CurrentHeight, nearDist, farDist) *
				glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.f, 1.f, 0.f));;

			glEnable(GL_DEPTH_TEST);
			RendererData.FogIntensity = fogIntensity;
		}

		void DrawFinalSurface(int w, int h)
		{
			IssueVertices();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			RendererData.CurrentWidth = w;
			RendererData.CurrentHeight = h;
			glDisable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			SetupOrthographic(glm::value_ptr(RendererData.ViewProj), 0, (float)RendererData.CurrentWidth, (float)RendererData.CurrentHeight, 0, -1.0f, 1.0f);

			ResourceIndex texture = Surfaces::GetTexture(RendererData.FinalSurface);
			DrawTexturedQuad(texture,
				0.f, 0.f, 0.f,
				(float)w, 0.f, 0.f,
				(float)w, (float)h, 0.f,
				0.f, (float)h, 0.f,
				0, 0, Textures::GetWidth(texture), Textures::GetHeight(texture),
				false, false,
				1.f, 1.f, 1.f, 1.f,
				0.f, 0.f, 0.f, 0.f);
			IssueVertices();
		}

		void DrawBillboard(ResourceIndex tex,
			float x, float y, float z,
			float width, float height,
			int srcx, int srcy, int srcw, int srch,
			bool flipX, bool flipY,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA)
		{
			unsigned int id = Resources.Textures[tex].GLID;
			int texW = Resources.Textures[tex].Width;
			int texH = Resources.Textures[tex].Height;
			float u1 = srcx / (float)texW;
			float u2 = (srcx + srcw) / (float)texW;
			float v1 = srcy / (float)texH;
			float v2 = (srcy + srch) / (float)texH;
			if (flipX)
			{
				float t = u1;
				u1 = u2;
				u2 = t;
			}
			if (Resources.Textures[tex].Flipped)
			{
				float t = v1;
				v1 = v2;
				v2 = t;
			}
			if (flipY)
			{
				float t = v1;
				v1 = v2;
				v2 = t;
			}

			float halfWidth = width * 0.5f;

			PushVertex(id, x, y, z, u1, v1, -halfWidth, height, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
			PushVertex(id, x, y, z, u2, v1, +halfWidth, height, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
			PushVertex(id, x, y, z, u2, v2, +halfWidth, 0.f, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
			PushVertex(id, x, y, z, u1, v2, -halfWidth, 0.f, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
		}

		void SetWireframe(bool wireframeEnabled)
		{
			RendererData.Wireframe = wireframeEnabled;
		}

		void SetBackfaceCulling(bool cullingEnabled)
		{
			RendererData.BackfaceCulling = cullingEnabled;
		}

		ResourceIndex GetFinalSurface()
		{
			return RendererData.FinalSurface;
		}
	}
}

extern "C"
{
	DLLExport void SPF_FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a)
	{
		SPF::Renderer::FillRectangle(x, y, w, h, r, g, b, a);
	}

	DLLExport void SPF_FillVerticalGradient(
		int x, int y, int w, int h,
		float r1, float g1, float b1, float a1,
		float r2, float g2, float b2, float a2)
	{
		SPF::Renderer::FillVerticalGradient(x, y, w, h, r1, g1, b1, a1, r2, g2, b2, a2);
	}

	DLLExport void SPF_FillHorizontalGradient(
		int x, int y, int w, int h,
		float r1, float g1, float b1, float a1,
		float r2, float g2, float b2, float a2)
	{
		SPF::Renderer::FillHorizontalGradient(x, y, w, h, r1, g1, b1, a1, r2, g2, b2, a2);
	}

	DLLExport void SPF_DrawTexturedQuad(
		int tex,
		float Ax, float Ay, float Az,
		float Bx, float By, float Bz,
		float Cx, float Cy, float Cz,
		float Dx, float Dy, float Dz,
		int srcx, int srcy, int srcw, int srch,
		bool flipX, bool flipY,
		float r, float g, float b, float a,
		float overlayR, float overlayG, float overlayB, float overlayA)
	{
		SPF::Renderer::DrawTexturedQuad(tex,
			Ax, Ay, Az,
			Bx, By, Bz,
			Cx, Cy, Cz,
			Dx, Dy, Dz,
			srcx, srcy, srcw, srch,
			flipX, flipY,
			r, g, b, a,
			overlayR, overlayG, overlayB, overlayA);
	}

	DLLExport void SPF_DrawTexture(int tex,
		int x, int y, int w, int h,
		int srcx, int srcy, int srcw, int srch,
		bool flipX, bool flipY,
		float r, float g, float b, float a,
		float overlayR, float overlayG, float overlayB, float overlayA)
	{
		SPF::Renderer::DrawTexture(tex,
			x, y, w, h,
			srcx, srcy, srcw, srch,
			flipX, flipY,
			r, g, b, a,
			overlayR, overlayG, overlayB, overlayA);
	}

	DLLExport void SPF_DrawMesh(int tex, int mesh, int first, int count, float* world)
	{
		SPF::Renderer::DrawMesh(tex, mesh, first, count, world);
	}

	DLLExport void SPF_SetBlending(int blendMode)
	{
		SPF::Renderer::SetBlending((SPF::BlendMode)blendMode);
	}

	DLLExport void SPF_Begin(int surface)
	{
		SPF::Renderer::Begin(surface);
	}

	DLLExport void SPF_BeginLookAtPerspective(int surface,
		float cameraX, float cameraY, float cameraZ,
		float cameraTargetX, float cameraTargetY, float cameraTargetZ,
		float fov, float nearDist, float farDist, float fogIntensity)
	{
		SPF::Renderer::BeginLookAtPerspective(surface,
			cameraX, cameraY, cameraZ,
			cameraTargetX, cameraTargetY, cameraTargetZ,
			fov, nearDist, farDist, fogIntensity);
	}

	DLLExport void SPF_DrawBillboard(int tex,
		float x, float y, float z, 
		float width, float height,
		int srcx, int srcy, int srcw, int srch,
		bool flipX, bool flipY,
		float r, float g, float b, float a,
		float overlayR, float overlayG, float overlayB, float overlayA)
	{
		SPF::Renderer::DrawBillboard(tex,
			x, y, z, 
			width, height,
			srcx, srcy, srcw, srch,
			flipX, flipY,
			r, g, b, a,
			overlayR, overlayG, overlayB, overlayA);
	}

	DLLExport void SPF_SetWireframe(bool wireframeEnabled)
	{
		SPF::Renderer::SetWireframe(wireframeEnabled);
	}

	DLLExport void SPF_SetBackfaceCulling(bool cullingEnabled)
	{
		SPF::Renderer::SetBackfaceCulling(cullingEnabled);
	}
}
