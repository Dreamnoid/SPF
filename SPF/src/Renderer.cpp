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
#include "OpenGLHelpers.h"

namespace SPF
{

	constexpr uint32_t MaxSprites = 2000;
	constexpr uint32_t VerticesPerSprite = 4;
	constexpr uint32_t MaxVerticesCount = MaxSprites * VerticesPerSprite;

	struct
	{
		Vertex Vertices[MaxVerticesCount];
		HardwareID BatchVBOID;
		int CurrentVertexCount = 0;
		PrimitiveType CurrentPrimitiveType = PrimitiveType::Quad;

		HardwareID EmptyTexture;
		HardwareID VertexShader;
		HardwareID CurrentProgram;
		ResourceIndex DefaultShader;
		ResourceIndex FinalSurface;

		RenderState::States CurrentState;
	} RendererData;

	namespace Renderer
	{
		void IssueVertices();

		void Begin(ResourceIndex surface, bool clear)
		{
			IssueVertices();

			surface = (surface < 0) ? RendererData.FinalSurface : surface;

			glBindFramebuffer(GL_FRAMEBUFFER, Resources.Surfaces[surface].GLID);
			if (clear)
			{
				SetBuffers({ true, true, Comparison::Less }); // Depth Writing must not be deactivated for clear to work
				Surfaces::Clear(surface);
			}

			ResourceIndex texture = Surfaces::GetTexture(surface);
			Vector2 size = { Textures::GetWidth(texture), Textures::GetHeight(texture) };

			glViewport(0, 0, (GLsizei)size.X, (GLsizei)size.Y);

			SetCamera({ glm::value_ptr(glm::ortho(0.f, (float)size.X, (float)size.Y, 0.f, -1.0f, 1.0f)), -1.0f, 1.0f , Vector3::Up, Vector3::Zero });
			SetBuffers({ true, false, Comparison::Always });
			SetFog({ 0, RGB::Black });
		}

		void SetCamera(const RenderState::Camera& camera)
		{
			IssueVertices();
			RendererData.CurrentState.Camera = camera;
		}

		void SetMaterial(const RenderState::Material& material)
		{
			const RenderState::States& state = RendererData.CurrentState;
			const RenderState::Material& current = state.Material;

			bool shaderChanged = (material.Shader != current.Shader);
			bool tex1Changed = (material.Texture1 != current.Texture1);
			bool tex2Changed = (material.Texture2 != current.Texture2);
			bool tex3Changed = (material.Texture3 != current.Texture3);

			if (shaderChanged || tex1Changed || tex2Changed || tex3Changed)
			{
				IssueVertices();
			}

			if (shaderChanged)
			{
				ResourceIndex shader = (material.Shader < 0) ? RendererData.DefaultShader : material.Shader;
				HardwareID programID = Resources.Shaders[shader].GLID;

				glUseProgram(programID);
				RendererData.CurrentProgram = programID;
			}

			if (tex1Changed)
			{
				glActiveTexture2(GL_TEXTURE0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, (material.Texture1 < 0) ? RendererData.EmptyTexture : Resources.Textures[material.Texture1].GLID);
			}

			if (tex2Changed)
			{
				glActiveTexture2(GL_TEXTURE1);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, (material.Texture2 < 0) ? RendererData.EmptyTexture : Resources.Textures[material.Texture2].GLID);
			}

			if (tex3Changed)
			{
				glActiveTexture2(GL_TEXTURE2);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, (material.Texture3 < 0) ? RendererData.EmptyTexture : Resources.Textures[material.Texture3].GLID);
			}

			RendererData.CurrentState.Material = material;
		}

		void SetFog(const RenderState::Fog& fog)
		{
			const RenderState::Fog& current = RendererData.CurrentState.Fog;
			if (fog.Intensity != current.Intensity || fog.Color.R != current.Color.R || fog.Color.G != current.Color.G || fog.Color.B != current.Color.B)
			{
				IssueVertices();
				RendererData.CurrentState.Fog = fog;
			}
		}

		void SetUserData(const RenderState::UserData& userData)
		{
			IssueVertices();
			RendererData.CurrentState.UserData = userData;
		}

		void SetRasterization(const RenderState::Rasterization& rasterization)
		{
			const RenderState::Rasterization& current = RendererData.CurrentState.Rasterization;

			if (rasterization.Blending != current.Blending)
			{
				IssueVertices();
				if (rasterization.Blending == BlendMode::Alpha)
				{
					glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				}
				else if (rasterization.Blending == BlendMode::Additive)
				{
					glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				}
			}

			if (rasterization.BackfaceCulling != current.BackfaceCulling)
			{
				IssueVertices();
				glCullFace(GL_BACK);
				SetOpenGLCapability(GL_CULL_FACE, (rasterization.BackfaceCulling != BackfaceCulling::Disabled));
				if (rasterization.BackfaceCulling == BackfaceCulling::CW)
				{
					glFrontFace(GL_CCW); // TODO: wait what
				}
				else if (rasterization.BackfaceCulling == BackfaceCulling::CCW)
				{
					glFrontFace(GL_CW);
				}
			}

			if (rasterization.Wireframe != current.Wireframe)
			{
				IssueVertices();
				glPolygonMode(GL_FRONT_AND_BACK, rasterization.Wireframe ? GL_LINE : GL_FILL);
			}

			if (rasterization.LineWidth != current.LineWidth)
			{
				IssueVertices();
				glLineWidth(rasterization.LineWidth);
			}

			RendererData.CurrentState.Rasterization = rasterization;
		}

		void SetBuffers(const RenderState::Buffers& buffers)
		{
			const RenderState::Buffers& current = RendererData.CurrentState.Buffers;

			if (buffers.ColorWrite != current.ColorWrite)
			{
				IssueVertices();
				GLboolean colorWriting = buffers.ColorWrite ? GL_TRUE : GL_FALSE;
				glColorMask(colorWriting, colorWriting, colorWriting, colorWriting);
			}

			if (buffers.DepthWrite != current.DepthWrite)
			{
				IssueVertices();
				glDepthMask(buffers.DepthWrite ? GL_TRUE : GL_FALSE);
			}

			if (buffers.DepthTest != current.DepthTest)
			{
				IssueVertices();
				SetOpenGLCapability(GL_DEPTH_TEST, (buffers.DepthTest != Comparison::Always));
				glDepthFunc(TranslateComparison(buffers.DepthTest));
			}

			RendererData.CurrentState.Buffers = buffers;
		}

		void SetStencil(const RenderState::Stencil& stencil)
		{
			const RenderState::Stencil& current = RendererData.CurrentState.Stencil;

			if (stencil.Write != current.Write || stencil.Test != current.Test)
			{
				IssueVertices();
				if (!stencil.Write && (stencil.Test == Comparison::Always))
				{
					glStencilMask(0xFF);
					glClear(GL_STENCIL_BUFFER_BIT);
				}

				glStencilMask(stencil.Write ? 0xFF : 0x00);
				SetOpenGLCapability(GL_STENCIL_TEST, stencil.Write || (stencil.Test != Comparison::Always));
			}

			if (stencil.Test != current.Test || stencil.Reference != current.Reference)
			{
				IssueVertices();
				glStencilFunc(TranslateComparison(stencil.Test), stencil.Reference, 0xFF);
			}

			if (stencil.StencilFail != current.StencilFail || stencil.DepthFail != current.DepthFail || stencil.DepthPass != current.DepthPass)
			{
				IssueVertices();
				glStencilOp(
					TranslateStencilAction(stencil.StencilFail),
					TranslateStencilAction(stencil.DepthFail),
					TranslateStencilAction(stencil.DepthPass));
			}

			RendererData.CurrentState.Stencil = stencil;
		}

		void SendDrawCall(GLenum mode, GLint first, GLsizei count)
		{
			const RenderState::States& state = RendererData.CurrentState;

			// Camera
			Bind(RendererData.CurrentProgram, "MVP", glm::make_mat4x4(state.Camera.ViewProjectionMatrix.M) * glm::make_mat4x4(state.ModelData.WorldMatrix.M));
			Bind(RendererData.CurrentProgram, "CameraUp", state.Camera.Up);
			Bind(RendererData.CurrentProgram, "CameraSide", state.Camera.Side);
			Bind(RendererData.CurrentProgram, "NearPlane", state.Camera.NearPlane);
			Bind(RendererData.CurrentProgram, "FarPlane", state.Camera.FarPlane);

			// Fog
			Bind(RendererData.CurrentProgram, "FogIntensity", state.Fog.Intensity);
			Bind(RendererData.CurrentProgram, "FogColor", state.Fog.Color);

			// User Data
			Bind(RendererData.CurrentProgram, "Animation", state.UserData.Animation);
			Bind(RendererData.CurrentProgram, "UserData", state.UserData.UserData);
			Bind(RendererData.CurrentProgram, "UserMatrix", glm::make_mat4x4(state.UserData.UserMatrix.M));

			// Model Data
			Bind(RendererData.CurrentProgram, "Overlay", state.ModelData.Overlay);

			// Bind the vertex attributes
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			glEnableVertexAttribArray(4);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0 * sizeof(float)));
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), BUFFER_OFFSET(3 * sizeof(float)));
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(6 * sizeof(float)));
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(10 * sizeof(float)));
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(14 * sizeof(float)));

			glDrawArrays(mode, first, count);
		}

		void DrawMesh(ResourceIndex mesh, int first, int count, const RenderState::ModelData& modelData)
		{
			IssueVertices();

			glBindBuffer(GL_ARRAY_BUFFER, Resources.Meshes[mesh].GLID);

			RendererData.CurrentState.ModelData = modelData;
			SendDrawCall(GL_TRIANGLES, first, count);
			RendererData.CurrentState.ModelData = { Matrix::Identity, RGBA::TransparentBlack };
		}

		void IssueVertices()
		{
			if (RendererData.CurrentVertexCount == 0) return;

			glBindBuffer(GL_ARRAY_BUFFER, RendererData.BatchVBOID);
			glBufferSubData(GL_ARRAY_BUFFER, 0, RendererData.CurrentVertexCount * sizeof(Vertex), &RendererData.Vertices);

			GLenum primitiveType = GL_QUADS;
			if (RendererData.CurrentPrimitiveType == PrimitiveType::Triangle)
			{
				primitiveType = GL_TRIANGLES;
			}
			else if (RendererData.CurrentPrimitiveType == PrimitiveType::Line)
			{
				primitiveType = GL_LINES;
			}

			SendDrawCall(primitiveType, 0, RendererData.CurrentVertexCount);
			RendererData.CurrentVertexCount = 0;
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
			glBufferData(GL_ARRAY_BUFFER, MaxVerticesCount * sizeof(Vertex), NULL, GL_STREAM_DRAW);

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
				"uniform mat4 UserMatrix;\n"
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

			glEnable(GL_POLYGON_OFFSET_LINE);
			glPolygonOffset(-1.0f, -1.0f);
		}

		void Resize(int w, int h)
		{
			Surfaces::Delete(RendererData.FinalSurface);
			RendererData.FinalSurface = Surfaces::Create(w, h, true);
		}

		void DrawFinalSurface(int w, int h)
		{
			IssueVertices();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glViewport(0, 0, (GLsizei)w, (GLsizei)h);

			SetCamera({ Matrix(glm::value_ptr(glm::ortho(0.f, (float)w, (float)h, 0.f, -1.f, 1.f))), -1, 1, Vector3::Zero, Vector3::Zero });
			SetBuffers({ true, false, Comparison::Always });

			ResourceIndex texture = Surfaces::GetTexture(GetFinalSurface());

			SetMaterial({ InvalidResource, texture, InvalidResource, InvalidResource });
			PushVertex({ { 0.f, (float)h, 0.f }, Vector3::Zero, { 0.f, 0.f }, Vector2::Zero, RGBA::White, RGBA::TransparentBlack });
			PushVertex({ { (float)w, (float)h, 0.f }, Vector3::Zero, { 1.f, 0.f }, Vector2::Zero, RGBA::White, RGBA::TransparentBlack });
			PushVertex({ { (float)w, 0.f, 0.f }, Vector3::Zero, { 1.f, 1.f }, Vector2::Zero, RGBA::White, RGBA::TransparentBlack });
			PushVertex({ { 0.f, 0.f, 0.f }, Vector3::Zero, { 0.f, 1.f }, Vector2::Zero, RGBA::White, RGBA::TransparentBlack });
			IssueVertices();
		}

		void SetPrimitiveType(PrimitiveType type)
		{
			if (RendererData.CurrentPrimitiveType != type)
			{
				IssueVertices();
				RendererData.CurrentPrimitiveType = type;
			}
		}

		void PushVertex(const Vertex& v)
		{
			int maxVerticesCount = MaxVerticesCount;
			if (RendererData.CurrentPrimitiveType == PrimitiveType::Triangle)
			{
				maxVerticesCount--; // Make it a multiple of 3
			}
			if (RendererData.CurrentVertexCount == maxVerticesCount)
			{
				IssueVertices();
			}
			RendererData.Vertices[RendererData.CurrentVertexCount] = v;
			RendererData.CurrentVertexCount++;
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
	DLLExport void SPF_Begin(int surface, int clear)
	{
		SPF::Renderer::Begin((SPF::ResourceIndex)surface, (bool)clear);
	}

	DLLExport void SPF_SetCamera(
		float* viewProjectionMatrix,
		float nearPlane, float farPlane,
		float upX, float upY, float upZ,
		float sideX, float sideY, float sideZ)
	{
		SPF::Renderer::SetCamera({ viewProjectionMatrix, nearPlane, farPlane, {upX, upY, upZ}, {sideX, sideY, sideZ} });
	}

	DLLExport void SPF_SetMaterial(int shader, int texture1, int texture2, int texture3)
	{
		SPF::Renderer::SetMaterial({ (SPF::ResourceIndex)shader, (SPF::ResourceIndex)texture1, (SPF::ResourceIndex)texture2, (SPF::ResourceIndex)texture3 });
	}

	DLLExport void SPF_SetFog(float intensity, float r, float g, float b)
	{
		SPF::Renderer::SetFog({ intensity, {r,g,b} });
	}

	DLLExport void SPF_SetUserData(float animation, float x, float y, float z, float w, float* userMatrix)
	{
		SPF::Renderer::SetUserData({ animation, {x,y,z,w}, userMatrix });
	}

	DLLExport void SPF_SetRasterization(int blending, int wireframe, int backfaceCulling, float lineWidth)
	{
		SPF::Renderer::SetRasterization({ (SPF::BlendMode)blending, (bool)wireframe, (SPF::BackfaceCulling)backfaceCulling, lineWidth });
	}

	DLLExport void SPF_SetBuffers(int colorWrite, int depthWrite, int depthTest)
	{
		SPF::Renderer::SetBuffers({ (bool)colorWrite, (bool)depthWrite, (SPF::Comparison)depthTest });
	}

	DLLExport void SPF_SetStencil(bool write, int test, float reference, int stencilFail, int depthFail, int depthPass)
	{
		SPF::Renderer::SetStencil({ (bool)write, (SPF::Comparison)test, reference, (SPF::StencilAction)stencilFail, (SPF::StencilAction)depthFail, (SPF::StencilAction)depthPass });
	}

	DLLExport int SPF_GetFinalSurface()
	{
		return SPF::Renderer::GetFinalSurface();
	}

	DLLExport void SPF_DrawMesh(
		int mesh, int first, int count, float* world,
		float overlayR, float overlayG, float overlayB, float overlayA)
	{
		SPF::Renderer::DrawMesh(mesh, first, count, { world, { overlayR, overlayG, overlayB, overlayA } });
	}

	DLLExport void SPF_SetPrimitiveType(int primitiveType)
	{
		SPF::Renderer::SetPrimitiveType((SPF::PrimitiveType)primitiveType);
	}

	DLLExport void SPF_PushVertex(
		float x, float y, float z,
		float normalX, float normalY, float normalZ,
		float u, float v,
		float bu, float bv,
		float r, float g, float b, float a,
		float overlayR, float overlayG, float overlayB, float overlayA)
	{
		SPF::Renderer::PushVertex({
			{ x, y, z },
			{ normalX, normalY, normalZ },
			{ u, v }, { bu, bv },
			{ r, g, b, a },
			{ overlayR, overlayG, overlayB, overlayA } });
	}
}