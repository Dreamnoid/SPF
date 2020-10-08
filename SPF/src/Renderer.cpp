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
			ResourceIndex CurrentTexture;
		} BatchInfo;

		int CurrentWidth;
		int CurrentHeight;
		glm::mat4 ViewProj;
		glm::mat4 Model;
		Vector3 CameraUp = { 0.f, 0.f, 0.f };
		Vector3 CameraSide = { 0.f, 0.f, 0.f };
		float CameraNearPlane, CameraFarPlane;
		float FogIntensity = 0.f;
		bool Wireframe;
		RGB FogColor = { 0.f, 0.f, 0.f };
		RGBA Overlay = { 0.f, 0.f, 0.f, 0.f };
		float Animation = 0.f;
		Vector4 UserData = { 0.f, 0.f, 0.f, 0.f };
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
				"uniform vec4 UserData;\n"
				"uniform float NearPlane;\n"
				"uniform float FarPlane;\n"
				"in float share_Distance;\n"
				"in vec2 share_UV;\n"
				"in vec4 share_Color;\n"
				"in vec4 share_Overlay;\n"
				"in vec3 share_Position;\n"
				"in vec3 share_Normal;\n"
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

		struct DrawCall
		{
			GLenum Mode;
		};

		void Prepare(ResourceIndex shader, const TextureSet& texSet)
		{
			glViewport(0, 0, (GLsizei)RendererData.CurrentWidth, (GLsizei)RendererData.CurrentHeight);

			glPolygonMode(GL_FRONT_AND_BACK, (RendererData.Wireframe) ? GL_LINE : GL_FILL);

			// Bind the shader
			shader = (shader < 0) ? RendererData.DefaultShader : shader;
			HardwareID programID = Resources.Shaders[shader].GLID;

			glUseProgram(programID);
			glm::mat4 mvp = RendererData.ViewProj * RendererData.Model;
			glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
			glUniform3f(glGetUniformLocation(programID, "CameraUp"), RendererData.CameraUp.X, RendererData.CameraUp.Y, RendererData.CameraUp.Z);
			glUniform3f(glGetUniformLocation(programID, "CameraSide"), RendererData.CameraSide.X, RendererData.CameraSide.Y, RendererData.CameraSide.Z);
			glUniform1f(glGetUniformLocation(programID, "FogIntensity"), RendererData.FogIntensity);
			glUniform3f(glGetUniformLocation(programID, "FogColor"), RendererData.FogColor.R, RendererData.FogColor.G, RendererData.FogColor.B);
			glUniform1f(glGetUniformLocation(programID, "NearPlane"), RendererData.CameraNearPlane);
			glUniform1f(glGetUniformLocation(programID, "FarPlane"), RendererData.CameraFarPlane);
			glUniform4f(glGetUniformLocation(programID, "Overlay"), RendererData.Overlay.R, RendererData.Overlay.G, RendererData.Overlay.B, RendererData.Overlay.A);
			glUniform1f(glGetUniformLocation(programID, "Animation"), RendererData.Animation);
			glUniform4f(glGetUniformLocation(programID, "UserData"), RendererData.UserData.X, RendererData.UserData.Y, RendererData.UserData.Z, RendererData.UserData.W);

			// Bind the textures
			glActiveTexture2(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, (texSet.Texture1 < 0) ? RendererData.EmptyTexture : Resources.Textures[texSet.Texture1].GLID);

			glActiveTexture2(GL_TEXTURE1);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, (texSet.Texture2 < 0) ? RendererData.EmptyTexture : Resources.Textures[texSet.Texture2].GLID);

			glActiveTexture2(GL_TEXTURE2);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, (texSet.Texture3 < 0) ? RendererData.EmptyTexture : Resources.Textures[texSet.Texture3].GLID);

			// Bind the vertex attributes
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			glEnableVertexAttribArray(4);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0 * sizeof(float)));
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE,  sizeof(Vertex), BUFFER_OFFSET(3 * sizeof(float)));
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(6 * sizeof(float)));
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(10 * sizeof(float)));
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(14 * sizeof(float)));
		}

		void IssueVertices()
		{
			if (RendererData.BatchInfo.VertexCount == 0) return;

			glBindBuffer(GL_ARRAY_BUFFER, RendererData.BatchVBOID);
			glBufferSubData(GL_ARRAY_BUFFER, 0, VerticesCount * sizeof(Vertex), &RendererData.Vertices);

			Prepare(RendererData.DefaultShader, { RendererData.BatchInfo.CurrentTexture, InvalidResource, InvalidResource });
			glDrawArrays(GL_QUADS, 0, RendererData.BatchInfo.VertexCount);
			memset(&RendererData.BatchInfo, 0, sizeof(RendererData.BatchInfo));
		}

		void PushVertex(ResourceIndex texture, const Vertex& v)
		{
			if (RendererData.BatchInfo.VertexCount == VerticesCount || texture != RendererData.BatchInfo.CurrentTexture)
			{
				IssueVertices();
				RendererData.BatchInfo.CurrentTexture = texture;
			}
			RendererData.Vertices[RendererData.BatchInfo.VertexCount] = v;
			RendererData.BatchInfo.VertexCount++;
		}

		void PushVertex(ResourceIndex texture,
			const Vector3& position,
			const Vector2& uv, const Vector2& buv,
			const RGBA& color, const RGBA& overlay)
		{
			if (RendererData.BatchInfo.VertexCount == VerticesCount || texture != RendererData.BatchInfo.CurrentTexture)
			{
				IssueVertices();
				RendererData.BatchInfo.CurrentTexture = texture;
			}
			Vertex* vertex = &RendererData.Vertices[RendererData.BatchInfo.VertexCount];
			vertex->Position = position;
			vertex->Normal = Vector3::Zero;
			vertex->UV = uv;
			vertex->BUV = buv;
			vertex->Color = color;
			vertex->Overlay = overlay;
			RendererData.BatchInfo.VertexCount++;
		}

		void DetermineUV(ResourceIndex tex, const Rect& src, bool flipX, bool flipY, Vector2& uv1, Vector2& uv2)
		{
			const int texW = Resources.Textures[tex].Width;
			const int texH = Resources.Textures[tex].Height;

			uv1.X = src.X / (float)texW;
			uv2.X = (src.X + src.Width) / (float)texW;
			uv1.Y = src.Y / (float)texH;
			uv2.Y = (src.Y + src.Height) / (float)texH;

			if (flipX)
			{
				const float t = uv1.X;
				uv1.X = uv2.X;
				uv2.X = t;
			}

			if (Resources.Textures[tex].Flipped)
			{
				uv1.Y = 1.f - uv1.Y;
				uv2.Y = 1.f - uv2.Y;
			}

			if (flipY)
			{
				const float t = uv1.Y;
				uv1.Y = uv2.Y;
				uv2.Y = t;
			}
		}

		void DrawTexturedRect(
			ResourceIndex tex,
			const Rect& dest,
			const Rect& src,
			bool flipX, bool flipY,
			const RGBA& aColor, const RGBA& bColor, const RGBA& cColor, const RGBA& dColor,
			const RGBA& overlay)
		{
			Vector2 uv1, uv2;
			DetermineUV(tex, src, flipX, flipY, uv1, uv2);

			PushVertex(tex, { (float)dest.X, (float)(dest.Y + dest.Height), 0.f }, { uv1.X, uv2.Y }, Vector2::Zero, dColor, overlay);;
			PushVertex(tex, { (float)(dest.X + dest.Width), (float)(dest.Y + dest.Height), 0.f }, { uv2.X, uv2.Y }, Vector2::Zero, cColor, overlay);
			PushVertex(tex, { (float)(dest.X + dest.Width), (float)dest.Y, 0.f }, { uv2.X, uv1.Y }, Vector2::Zero, bColor, overlay);
			PushVertex(tex, { (float)dest.X, (float)dest.Y, 0.f }, { uv1.X, uv1.Y }, Vector2::Zero, aColor, overlay);
		}

		void FillRectangle(const Rect& dest, const RGBA& color)
		{
			PushVertex(InvalidResource, { (float)dest.X, (float)(dest.Y + dest.Height), 0.f }, { 0.f, 1.f }, Vector2::Zero, color, RGBA::TransparentBlack);
			PushVertex(InvalidResource, { (float)(dest.X + dest.Width), (float)(dest.Y + dest.Height), 0.f }, { 1.f, 1.f }, Vector2::Zero, color, RGBA::TransparentBlack);
			PushVertex(InvalidResource, { (float)(dest.X + dest.Width), (float)dest.Y, 0.f }, { 1.f, 0.f }, Vector2::Zero, color, RGBA::TransparentBlack);
			PushVertex(InvalidResource, { (float)dest.X, (float)dest.Y, 0.f }, { 0.f, 0.f }, Vector2::Zero, color, RGBA::TransparentBlack);
		}

		void FillVerticalGradient(const Rect& dest, const RGBA& top, const RGBA& bottom)
		{
			DrawTexturedRect(0, dest,
				Rect::Unit, false, false,
				top, top, bottom, bottom, 
				RGBA::TransparentBlack);
		}

		void FillHorizontalGradient(const Rect& dest, const RGBA& left, const RGBA& right)
		{
			DrawTexturedRect(0, dest,
				Rect::Unit, false, false,
				left, right, right, left,
				RGBA::TransparentBlack);
		}

		void DrawTexturedQuad(
			ResourceIndex tex,
			const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d,
			const Rect& src,
			bool flipX, bool flipY,
			const RGBA& aColor, const RGBA& bColor, const RGBA& cColor, const RGBA& dColor,
			const RGBA& overlay)
		{
			Vector2 uv1, uv2;
			DetermineUV(tex, src, flipX, flipY, uv1, uv2);
			
			PushVertex(tex, d, { uv1.X, uv2.Y }, Vector2::Zero, dColor, overlay);
			PushVertex(tex, c, { uv2.X, uv2.Y }, Vector2::Zero, cColor, overlay);
			PushVertex(tex, b, { uv2.X, uv1.Y }, Vector2::Zero, bColor, overlay);
			PushVertex(tex, a, { uv1.X, uv1.Y }, Vector2::Zero, aColor, overlay);
		}

		void DrawTexturedTriangle(ResourceIndex tex, const Vertex& a, const Vertex& b, const Vertex& c)
		{
			PushVertex(tex, a);
			PushVertex(tex, b);
			PushVertex(tex, c);
		}

		void DrawTexture(
			ResourceIndex tex,
			const Rect& dest, const Rect& src,
			bool flipX, bool flipY,
			const RGBA& color, const RGBA& overlay)
		{
			DrawTexturedRect(tex, dest, src, flipX, flipY, color, color, color, color, overlay);
		}

		void DrawMesh(
			ResourceIndex tex, ResourceIndex mesh, 
			const float* world,
			const RGBA& overlay)
		{
			DrawMesh(
				RendererData.DefaultShader, { tex, InvalidResource, InvalidResource },
				mesh, 0, Resources.Meshes[mesh].VerticesCount,
				world, 
				overlay);
		}

		void DrawMesh(
			ResourceIndex shader, const TextureSet& texSet,
			ResourceIndex mesh, int first, int count, 
			const float* world,
			const RGBA& overlay)
		{
			IssueVertices();

			glBindBuffer(GL_ARRAY_BUFFER, Resources.Meshes[mesh].GLID);

			RendererData.Model = glm::make_mat4x4(world);
			RendererData.Overlay = overlay;

			Prepare(shader, texSet);
			glDrawArrays(GL_TRIANGLES, first, count);

			RendererData.Model = glm::identity<glm::mat4>();
			RendererData.Overlay = RGBA::TransparentBlack;
		}

		void DrawLine(const Vector3& from, const Vector3& to, const RGBA& color)
		{
			IssueVertices();

			PushVertex(InvalidResource, { from, Vector3::Zero, Vector2::Zero, Vector2::Zero, color, RGBA::TransparentBlack });
			PushVertex(InvalidResource, { to, Vector3::Zero, Vector2::Zero, Vector2::Zero, color, RGBA::TransparentBlack });

			glBindBuffer(GL_ARRAY_BUFFER, RendererData.BatchVBOID);
			glBufferSubData(GL_ARRAY_BUFFER, 0, VerticesCount * sizeof(Vertex), &RendererData.Vertices);

			Prepare(RendererData.DefaultShader, { InvalidResource, InvalidResource, InvalidResource });
			glDrawArrays(GL_LINES, 0, RendererData.BatchInfo.VertexCount);
			memset(&RendererData.BatchInfo, 0, sizeof(RendererData.BatchInfo));
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
			RendererData.CameraUp = Vector3::Up;
			RendererData.CameraSide = Vector3::Zero;
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
			RendererData.CameraUp = Vector3::Up;
			RendererData.CameraSide = { cameraSide.x, cameraSide.y, cameraSide.z };
			RendererData.CameraNearPlane = nearDist;
			RendererData.CameraFarPlane = farDist;

			RendererData.ViewProj = 
				glm::perspectiveFov(fov, (float)RendererData.CurrentWidth, (float)RendererData.CurrentHeight, nearDist, farDist) *
				glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.f, 1.f, 0.f));

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

			RendererData.CameraUp = Vector3::Up;
			RendererData.CameraUp = { 1.f, 0.f, 0.f };
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
			DrawTexturedRect(texture,
				{ 0, 0, w, h },
				{ 0, 0, Textures::GetWidth(texture), Textures::GetHeight(texture) },
				false, false,
				RGBA::White,
				RGBA::White,
				RGBA::White,
				RGBA::White,
				RGBA::TransparentBlack);
			IssueVertices();
		}

		void DrawBillboard(ResourceIndex tex,
			const Vector3& center,
			float width, float height,
			const Rect& src,
			bool flipX, bool flipY,
			const RGBA& color,
			const RGBA& overlay)
		{
			Vector2 uv1, uv2;
			DetermineUV(tex, src, flipX, flipY, uv1, uv2);

			float halfWidth = width * 0.5f;

			PushVertex(tex, center, { uv1.X, uv2.Y }, { -halfWidth, 0.f }, color, overlay);
			PushVertex(tex, center, { uv2.X, uv2.Y }, { +halfWidth, 0.f }, color, overlay);
			PushVertex(tex, center, { uv2.X, uv1.Y }, { +halfWidth, height }, color, overlay);
			PushVertex(tex, center, { uv1.X, uv1.Y }, { -halfWidth, height }, color, overlay);
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

		void SetFogColor(const RGB& color)
		{
			RendererData.FogColor = color;
		}

		void SetAnimation(float animation)
		{
			RendererData.Animation = animation;
		}

		void SetUserData(const Vector4& userData)
		{
			RendererData.UserData = userData;
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
		SPF::Renderer::FillRectangle(
			{ x, y, w, h }, 
			{ r, g, b, a });
	}

	DLLExport void SPF_FillVerticalGradient(
		int x, int y, int w, int h,
		float r1, float g1, float b1, float a1,
		float r2, float g2, float b2, float a2)
	{
		SPF::Renderer::FillVerticalGradient(
			{ x, y, w, h }, 
			{ r1, g1, b1, a1 }, 
			{ r2, g2, b2, a2 });
	}

	DLLExport void SPF_FillHorizontalGradient(
		int x, int y, int w, int h,
		float r1, float g1, float b1, float a1,
		float r2, float g2, float b2, float a2)
	{
		SPF::Renderer::FillHorizontalGradient(
			{ x, y, w, h }, 
			{ r1, g1, b1,  a1 }, 
			{ r2, g2, b2, a2 });
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
			{ Ax, Ay, Az },
			{ Bx, By, Bz },
			{ Cx, Cy, Cz },
			{ Dx, Dy, Dz },
			{ srcx, srcy, srcw, srch },
			flipX, flipY,
			{ Ar, Ag, Ab, Aa },
			{ Br, Bg, Bb, Ba },
			{ Cr, Cg, Cb, Ca },
			{ Dr, Dg, Db, Da },
			{ overlayR, overlayG, overlayB, overlayA });
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
			{ x, y, w, h },
			{ srcx, srcy, srcw, srch },
			flipX, flipY,
			{ r, g, b, a },
			{ overlayR, overlayG, overlayB, overlayA });
	}

	DLLExport void SPF_DrawMesh(
		int shader, int tex, int tex1, int tex2,
		int mesh, int first, int count, 
		float* world,
		float overlayR, float overlayG, float overlayB, float overlayA)
	{
		SPF::Renderer::DrawMesh(
			shader, { tex, tex1, tex2 },
			mesh, first, count, 
			world, 
			{ overlayR, overlayG, overlayB, overlayA });
	}

	DLLExport void SPF_DrawLine(
		float fromX, float fromY, float fromZ,
		float toX, float toY, float toZ,
		float r, float g, float b, float a)
	{
		SPF::Renderer::DrawLine({ fromX, fromY, fromZ }, { toX, toY, toZ }, { r,g,b,a });
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
			{ x, y, z },
			width, height,
			{ srcx, srcy, srcw, srch },
			flipX, flipY,
			{ r, g, b, a },
			{ overlayR, overlayG, overlayB, overlayA });
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
		SPF::Renderer::SetFogColor({ r, g, b });
	}

	DLLExport void SPF_SetAnimation(float animation)
	{
		SPF::Renderer::SetAnimation(animation);
	}

	DLLExport void SPF_SetUserData(float x, float y, float z, float w)
	{
		SPF::Renderer::SetUserData({ x,y,z,w });
	}

	DLLExport int SPF_GetFinalSurface()
	{
		return SPF::Renderer::GetFinalSurface();
	}
}
