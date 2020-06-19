#include <Renderer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "gl4.h"
#include <cstdio>
#include <Surfaces.h>
#include <Textures.h>
#include <Meshes.h>
#include <Shaders.h>
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
		HardwareID VertexShader;
		ResourceIndex DefaultShader;
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
		float CameraUpX = 0.f, CameraUpY = 1.f, CameraUpZ = 0.f;
		float CameraSideX = 0.f, CameraSideY = 0.f, CameraSideZ = 0.f;
		float CameraNearPlane, CameraFarPlane;
		float FogIntensity = 0.f;
		bool Wireframe;
		float FogColorR = 0.f, FogColorG = 0.f, FogColorB = 0.f;
		float OverlayR = 0.f, OverlayG = 0.f, OverlayB = 0.f, OverlayA = 0.f;
		float Animation = 0.f;
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
			Resources.Textures.insert(Resources.Textures.begin(), { true,RendererData.EmptyTexture,1,1,false });

			glBindTexture(GL_TEXTURE_2D, RendererData.EmptyTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			uint32_t pixels[1];
			pixels[0] = 0xFFFFFFFF;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)pixels);

			RendererData.DefaultShader = Shaders::Create(
				"#version 330 core\n"
				"uniform sampler2D Texture;\n"
				"uniform sampler2D Texture1;\n"
				"uniform sampler2D Texture2;\n"
				"uniform float FogIntensity;\n"
				"uniform vec3 FogColor;\n"
				"uniform vec4 Overlay;\n"
				"uniform float Animation;\n"
				"uniform float NearPlane;\n"
				"uniform float FarPlane;\n"
				"in float share_Distance;\n"
				"in vec2 share_UV;\n"
				"in vec4 share_Color;\n"
				"in vec4 share_Overlay;\n"
				"in vec3 share_Position;\n"
				"out vec4 out_Color;\n"
				"void main()\n"
				"{\n"
				"	vec4 texColor = texture2D(Texture, share_UV) * share_Color;\n"
				"	if (texColor.a <= 0) discard;\n"
				"	out_Color = mix(texColor, vec4(share_Overlay.xyz, texColor.a), share_Overlay.a);\n"
				"	out_Color = mix(out_Color, vec4(FogColor,texColor.a), FogIntensity * share_Distance);\n"
				"	out_Color = mix(out_Color, vec4(Overlay.xyz, 1.0), Overlay.a);\n"
				"}\n");

			RendererData.FinalSurface = Surfaces::Create(w, h, true);
			RendererData.Model = glm::identity<glm::mat4>();
			RendererData.Wireframe = false;

			SetBackfaceCulling(0);

			glEnable(GL_POLYGON_OFFSET_LINE);
			glPolygonOffset(-1.0f, -1.0f);
		}

		void Resize(int w, int h)
		{
			Surfaces::Delete(RendererData.FinalSurface);
			RendererData.FinalSurface = Surfaces::Create(w, h, true);
		}

		void Prepare(ResourceIndex shader = InvalidResource)
		{
			glViewport(0, 0, (GLsizei)RendererData.CurrentWidth, (GLsizei)RendererData.CurrentHeight);

			glPolygonMode(GL_FRONT_AND_BACK, (RendererData.Wireframe) ? GL_LINE : GL_FILL);

			shader = (shader < 0) ? RendererData.DefaultShader : shader;
			HardwareID programID = Resources.Shaders[shader].GLID;

			glUseProgram(programID);
			glm::mat4 mvp = RendererData.ViewProj * RendererData.Model;
			glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
			glUniform3f(glGetUniformLocation(programID, "CameraUp"), RendererData.CameraUpX, RendererData.CameraUpY, RendererData.CameraUpZ);
			glUniform3f(glGetUniformLocation(programID, "CameraSide"), RendererData.CameraSideX, RendererData.CameraSideY, RendererData.CameraSideZ);
			glUniform1f(glGetUniformLocation(programID, "FogIntensity"), RendererData.FogIntensity);
			glUniform3f(glGetUniformLocation(programID, "FogColor"), RendererData.FogColorR, RendererData.FogColorG, RendererData.FogColorB);
			glUniform1f(glGetUniformLocation(programID, "NearPlane"), RendererData.CameraNearPlane);
			glUniform1f(glGetUniformLocation(programID, "FarPlane"), RendererData.CameraFarPlane);
			glUniform4f(glGetUniformLocation(programID, "Overlay"), RendererData.OverlayR, RendererData.OverlayG, RendererData.OverlayB, RendererData.OverlayA);
			glUniform1f(glGetUniformLocation(programID, "Animation"), RendererData.Animation);

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

		void PushVertex(HardwareID texture, const Vertex& v)
		{
			if (RendererData.BatchInfo.VertexCount == VerticesCount || texture != RendererData.BatchInfo.CurrentTexture)
			{
				IssueVertices();
				RendererData.BatchInfo.CurrentTexture = texture;
			}
			RendererData.Vertices[RendererData.BatchInfo.VertexCount] = v;
			RendererData.BatchInfo.VertexCount++;
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
			PushVertex(RendererData.EmptyTexture, (float)x, (float)(y + h), 0.f, 0.f, 1.f, 0.f, 0.f, r, g, b, a, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)(x + w), (float)(y + h), 0.f, 1.f, 1.f, 0.f, 0.f, r, g, b, a, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)(x + w), (float)y, 0.f, 1.f, 0.f, 0.f, 0.f, r, g, b, a, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)x, (float)y, 0.f, 0.f, 0.f, 0.f, 0.f, r, g, b, a, 0.f, 0.f, 0.f, 0.f);
		}

		void FillVerticalGradient(
			int x, int y, int w, int h,
			float r1, float g1, float b1, float a1,
			float r2, float g2, float b2, float a2)
		{
			PushVertex(RendererData.EmptyTexture, (float)x, (float)(y + h), 0.f, 0.f, 1.f, 0.f, 0.f, r2, g2, b2, a2, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)(x + w), (float)(y + h), 0.f, 1.f, 1.f, 0.f, 0.f, r2, g2, b2, a2, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)(x + w), (float)y, 0.f, 1.f, 0.f, 0.f, 0.f, r1, g1, b1, a1, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)x, (float)y, 0.f, 0.f, 0.f, 0.f, 0.f, r1, g1, b1, a1, 0.f, 0.f, 0.f, 0.f);
		}

		void FillHorizontalGradient(
			int x, int y, int w, int h,
			float r1, float g1, float b1, float a1,
			float r2, float g2, float b2, float a2)
		{
			PushVertex(RendererData.EmptyTexture, (float)x, (float)(y + h), 0.f, 0.f, 1.f, 0.f, 0.f, r1, g1, b1, a1, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)(x + w), (float)(y + h), 0.f, 1.f, 1.f, 0.f, 0.f, r2, g2, b2, a2, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)(x + w), (float)y, 0.f, 1.f, 0.f, 0.f, 0.f, r2, g2, b2, a2, 0.f, 0.f, 0.f, 0.f);
			PushVertex(RendererData.EmptyTexture, (float)x, (float)y, 0.f, 0.f, 0.f, 0.f, 0.f, r1, g1, b1, a1, 0.f, 0.f, 0.f, 0.f);
		}
		
		void DrawTexturedQuad(
			ResourceIndex tex,
			float Ax, float Ay, float Az,
			float Bx, float By, float Bz,
			float Cx, float Cy, float Cz,
			float Dx, float Dy, float Dz,
			int srcx, int srcy, int srcw, int srch,
			bool flipX, bool flipY,
			float Ar, float Ag, float Ab, float Aa,
			float Br, float Bg, float Bb, float Ba,
			float Cr, float Cg, float Cb, float Ca,
			float Dr, float Dg, float Db, float Da,
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
			
			PushVertex(id, Dx, Dy, Dz, u1, v2, 0.f, 0.f, Dr, Dg, Db, Da, overlayR, overlayG, overlayB, overlayA);
			PushVertex(id, Cx, Cy, Cz, u2, v2, 0.f, 0.f, Cr, Cg, Cb, Ca, overlayR, overlayG, overlayB, overlayA);
			PushVertex(id, Bx, By, Bz, u2, v1, 0.f, 0.f, Br, Bg, Bb, Ba, overlayR, overlayG, overlayB, overlayA);
			PushVertex(id, Ax, Ay, Az, u1, v1, 0.f, 0.f, Ar, Ag, Ab, Aa, overlayR, overlayG, overlayB, overlayA);
		}

		void DrawTexturedTriangle(ResourceIndex tex, Vertex a, Vertex b, Vertex c)
		{
			unsigned int id = Resources.Textures[tex].GLID;
			PushVertex(id, a);
			PushVertex(id, b);
			PushVertex(id, c);
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
				r, g, b, a,
				r, g, b, a,
				r, g, b, a,
				overlayR, overlayG, overlayB, overlayA);
		}

		void DrawMesh(
			ResourceIndex tex, ResourceIndex mesh, 
			const float* world,
			float overlayR, float overlayG, float overlayB, float overlayA)
		{
			DrawMesh(
				RendererData.DefaultShader, tex, InvalidResource, InvalidResource,
				mesh, 0, Resources.Meshes[mesh].VerticesCount,
				world, 
				overlayR, overlayG, overlayB, overlayA);
		}

		void DrawMesh(
			ResourceIndex shader, ResourceIndex tex, ResourceIndex tex1, ResourceIndex tex2,
			ResourceIndex mesh, int first, int count, 
			const float* world,
			float overlayR, float overlayG, float overlayB, float overlayA)
		{
			IssueVertices();

			glBindBuffer(GL_ARRAY_BUFFER, Resources.Meshes[mesh].GLID);

			glActiveTexture2(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, (tex < 0) ? RendererData.EmptyTexture : Resources.Textures[tex].GLID);

			glActiveTexture2(GL_TEXTURE1);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, (tex1 < 0) ? RendererData.EmptyTexture : Resources.Textures[tex1].GLID);

			glActiveTexture2(GL_TEXTURE2);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, (tex2 < 0) ? RendererData.EmptyTexture : Resources.Textures[tex2].GLID);

			RendererData.Model = glm::make_mat4x4(world);
			RendererData.OverlayR = overlayR;
			RendererData.OverlayG = overlayG;
			RendererData.OverlayB = overlayB;
			RendererData.OverlayA = overlayA;

			Prepare(shader);
			glDrawArrays(GL_TRIANGLES, first, count);

			RendererData.Model = glm::identity<glm::mat4>();
			RendererData.OverlayR = 0.0f;
			RendererData.OverlayG = 0.0f;
			RendererData.OverlayB = 0.0f;
			RendererData.OverlayA = 0.0f;
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
			RendererData.CameraUpX = 0.f;
			RendererData.CameraUpY = 1.f;
			RendererData.CameraUpZ = 0.f;
			RendererData.CameraSideX = 0.f;
			RendererData.CameraSideY = 0.f;
			RendererData.CameraSideZ = 0.f;
			RendererData.CameraNearPlane = -1.0f;
			RendererData.CameraFarPlane = 1.0f;

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
			RendererData.CameraUpX = 0.f;
			RendererData.CameraUpY = 1.f;
			RendererData.CameraUpZ = 0.f;
			RendererData.CameraSideX = cameraSide.x;
			RendererData.CameraSideY = cameraSide.y;
			RendererData.CameraSideZ = cameraSide.z;
			RendererData.CameraNearPlane = nearDist;
			RendererData.CameraFarPlane = farDist;

			RendererData.ViewProj = 
				glm::perspectiveFov(fov, (float)RendererData.CurrentWidth, (float)RendererData.CurrentHeight, nearDist, farDist) *
				glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.f, 1.f, 0.f));;

			glEnable(GL_DEPTH_TEST);
			RendererData.FogIntensity = fogIntensity;
		}

		void BeginOrthographic(ResourceIndex surface,
			float minX, float maxX,
			float minY, float maxY,
			float minZ, float maxZ, float fogIntensity)
		{
			IssueVertices();

			surface = (surface < 0) ? RendererData.FinalSurface : surface;
			ResourceIndex texture = Surfaces::GetTexture(surface);
			glBindFramebuffer(GL_FRAMEBUFFER, Resources.Surfaces[surface].GLID);

			RendererData.CurrentWidth = Resources.Textures[texture].Width;
			RendererData.CurrentHeight = Resources.Textures[texture].Height;

			RendererData.CameraUpX = 0.f;
			RendererData.CameraUpY = 1.f;
			RendererData.CameraUpZ = 0.f;
			RendererData.CameraSideX = 1.f;
			RendererData.CameraSideY = 0.f;
			RendererData.CameraSideZ = 0.f;
			RendererData.CameraNearPlane = minZ;
			RendererData.CameraFarPlane = maxZ;

			RendererData.ViewProj = glm::orthoLH(minX, maxX, minY, maxY, minZ, maxZ);

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
				1.f, 1.f, 1.f, 1.f,
				1.f, 1.f, 1.f, 1.f,
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

			PushVertex(id, x, y, z, u1, v2, -halfWidth, 0.f, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
			PushVertex(id, x, y, z, u2, v2, +halfWidth, 0.f, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
			PushVertex(id, x, y, z, u2, v1, +halfWidth, height, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
			PushVertex(id, x, y, z, u1, v1, -halfWidth, height, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
		}

		void SetWireframe(bool wireframeEnabled)
		{
			RendererData.Wireframe = wireframeEnabled;
		}

		void SetBackfaceCulling(int culling)
		{
			glCullFace(GL_BACK);
			if (culling == 0) // Disabled
			{
				glDisable(GL_CULL_FACE);
				
			}
			else if(culling == 1) // CW
			{
				glFrontFace(GL_CCW);
				glEnable(GL_CULL_FACE);
			}
			else if (culling == 2) // CCW
			{
				glFrontFace(GL_CW);
				glEnable(GL_CULL_FACE);
			}
		}

		void SetFogColor(float r, float g, float b)
		{
			RendererData.FogColorR = r;
			RendererData.FogColorG = g;
			RendererData.FogColorB = b;
		}

		void SetAnimation(float animation)
		{
			RendererData.Animation = animation;
		}

		ResourceIndex GetFinalSurface()
		{
			return RendererData.FinalSurface;
		}

		ResourceIndex GetDefaultShader()
		{
			return RendererData.DefaultShader;
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
		float Ar, float Ag, float Ab, float Aa,
		float Br, float Bg, float Bb, float Ba,
		float Cr, float Cg, float Cb, float Ca,
		float Dr, float Dg, float Db, float Da,
		float overlayR, float overlayG, float overlayB, float overlayA)
	{
		SPF::Renderer::DrawTexturedQuad(tex,
			Ax, Ay, Az,
			Bx, By, Bz,
			Cx, Cy, Cz,
			Dx, Dy, Dz,
			srcx, srcy, srcw, srch,
			flipX, flipY,
			Ar, Ag, Ab, Aa,
			Br, Bg, Bb, Ba,
			Cr, Cg, Cb, Ca,
			Dr, Dg, Db, Da,
			overlayR, overlayG, overlayB, overlayA);
	}

	DLLExport void SPF_DrawTexturedTriangle(int tex, SPF::Vertex a, SPF::Vertex b, SPF::Vertex c)
	{
		SPF::Renderer::DrawTexturedTriangle(tex, a, b, c);
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

	DLLExport void SPF_DrawMesh(
		int shader, int tex, int tex1, int tex2,
		int mesh, int first, int count, 
		float* world,
		float overlayR, float overlayG, float overlayB, float overlayA)
	{
		SPF::Renderer::DrawMesh(
			shader, tex, tex1, tex2,
			mesh, first, count, 
			world, 
			overlayR, overlayG, overlayB, overlayA);
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

	DLLExport void SPF_BeginOrthographic(int surface,
		float minX, float maxX,
		float minY, float maxY,
		float minZ, float maxZ, float fogIntensity)
	{
		SPF::Renderer::BeginOrthographic(surface,
			minX, maxX,
			minY, maxY,
			minZ, maxZ, fogIntensity);
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

	DLLExport void SPF_SetBackfaceCulling(int culling)
	{
		SPF::Renderer::SetBackfaceCulling(culling);
	}

	DLLExport void SPF_SetFogColor(float r, float g, float b)
	{
		SPF::Renderer::SetFogColor(r, g, b);
	}

	DLLExport void SPF_SetAnimation(float animation)
	{
		SPF::Renderer::SetAnimation(animation);
	}
}
