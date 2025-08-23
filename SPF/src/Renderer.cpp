#include <Renderer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "GL4.h"
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
	constexpr uint32_t VerticesPerSprite = 6;
	constexpr uint32_t MaxVerticesCount = MaxSprites * VerticesPerSprite;

	constexpr uint32_t MaxInstances = 5000;
	constexpr uint32_t FloatsPerTransform = 16;
	constexpr uint32_t FloatsPerOverlay = 4;

	struct
	{
		Vertex Vertices[MaxVerticesCount];
		ResourceIndex BatchMesh;
		int CurrentVertexCount = 0;
		PrimitiveType CurrentPrimitiveType = PrimitiveType::Triangle;

		float TransformsSSBO[FloatsPerTransform * MaxInstances];
		float OverlaysSSBO[FloatsPerOverlay * MaxInstances];

		RenderState::ModelData Instances[MaxInstances];
		int CurrentInstanceCount = 0;
		ResourceIndex CurrentInstanceMesh = InvalidResource;
		int CurrentInstanceMeshFirst = 0;
		int CurrentInstanceMeshCount = 0;

		HardwareID EmptyTexture;
		HardwareID VertexShader;
		HardwareID CurrentProgram;
		HardwareID WorldMatrixSSBO;
		ResourceIndex DefaultShader;
		ResourceIndex FinalSurface;

		RenderState::States CurrentState;
	} RendererData;

	namespace Renderer
	{
		void FlushDrawCalls();

		void CopyModelData(int index, const RenderState::ModelData& modelData)
		{
			int start = index * FloatsPerTransform;
			for (int j = 0; j < FloatsPerTransform; ++j)
			{
				RendererData.TransformsSSBO[start + j] = modelData.WorldMatrix.M[j];
			}

			start = index * FloatsPerOverlay;
			RendererData.OverlaysSSBO[start + 0] = modelData.Overlay.R;
			RendererData.OverlaysSSBO[start + 1] = modelData.Overlay.G;
			RendererData.OverlaysSSBO[start + 2] = modelData.Overlay.B;
			RendererData.OverlaysSSBO[start + 3] = modelData.Overlay.A;
		}

		void Begin(ResourceIndex surface, bool clear)
		{
			FlushDrawCalls();

			surface = (surface < 0) ? RendererData.FinalSurface : surface;

			if (clear)
			{
				SetBuffers({ true, true, Comparison::Less }); // Depth Writing must not be deactivated for clear to work
				Surfaces::Clear(surface);
			}
			else // Clear already binds it
			{
				glBindFramebuffer(GL_FRAMEBUFFER, Resources.Surfaces[surface].GLID);
			}

			ResourceIndex texture = Surfaces::GetTexture(surface);
			Vector2 size = { (float)Textures::GetWidth(texture), (float)Textures::GetHeight(texture) };

			glViewport(0, 0, (GLsizei)size.X, (GLsizei)size.Y);

			SetCamera({ Matrix(), glm::value_ptr(glm::ortho(0.f, (float)size.X, (float)size.Y, 0.f, -1.0f, 1.0f)), -1.0f, 1.0f , Vector3::Up, Vector3::Zero });
			SetBuffers({ true, false, Comparison::Always });
			SetFog({ 0, RGB::Black });
		}

		void SetCamera(const RenderState::Camera& camera)
		{
			FlushDrawCalls();
			RendererData.CurrentState.CurrentCamera = camera;
		}

		void SetMaterial(const RenderState::Material& material)
		{
			const RenderState::States& state = RendererData.CurrentState;
			const RenderState::Material& current = state.CurrentMaterial;

			const int texturesCount = 8;

			bool shaderChanged = (material.Shader != current.Shader);
			if (shaderChanged)
			{
				FlushDrawCalls();
			}
			else
			{
				for (int i = 0; i < texturesCount; ++i)
				{
					if (material.GetTexture(i) != current.GetTexture(i))
					{
						FlushDrawCalls();
						break;
					}
				}
			}

			if (shaderChanged)
			{
				ResourceIndex shader = (material.Shader < 0) ? RendererData.DefaultShader : material.Shader;
				HardwareID programID = Resources.Shaders[shader].GLID;

				glUseProgram(programID);
				RendererData.CurrentProgram = programID;
			}

			for (int i = 0; i < texturesCount; ++i)
			{
				ResourceIndex textureId = material.GetTexture(i);
				if (textureId != current.GetTexture(i))
				{
					glActiveTexture2(GL_TEXTURE0 + i);
					glEnable(GL_TEXTURE_2D);
					if (textureId < 0)
					{
						glBindTexture(GL_TEXTURE_2D, RendererData.EmptyTexture);
					}
					else
					{
						const Texture& tex = Resources.Textures[textureId];
						glBindTexture(tex.GLType, tex.GLID);
					}
				}
			}

			RendererData.CurrentState.CurrentMaterial = material;
		}

		void SetFog(const RenderState::Fog& fog)
		{
			const RenderState::Fog& current = RendererData.CurrentState.CurrentFog;
			if (fog.Intensity != current.Intensity || fog.Color.R != current.Color.R || fog.Color.G != current.Color.G || fog.Color.B != current.Color.B)
			{
				FlushDrawCalls();
				RendererData.CurrentState.CurrentFog = fog;
			}
		}

		void SetUserData(const RenderState::UserData& userData)
		{
			const RenderState::UserData& current = RendererData.CurrentState.CurrentUserData;
			if (userData.Animation != current.Animation || userData.UserData != current.UserData || userData.UserMatrix != current.UserMatrix)
			{
				FlushDrawCalls();
			}

			RendererData.CurrentState.CurrentUserData = userData;
		}

		void SetRasterization(const RenderState::Rasterization& rasterization)
		{
			const RenderState::Rasterization& current = RendererData.CurrentState.CurrentRasterization;

			if (rasterization.Blending != current.Blending)
			{
				FlushDrawCalls();
				if (rasterization.Blending == BlendMode::None)
				{
					glDisable(GL_BLEND);
				}
				else
				{
					glEnable(GL_BLEND);
					if (rasterization.Blending == BlendMode::Alpha)
					{
						glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
					}
					else if (rasterization.Blending == BlendMode::Additive)
					{
						glBlendFunc(GL_SRC_ALPHA, GL_ONE);
					}
					else if (rasterization.Blending == BlendMode::Multiply)
					{
						// (sFactor * S + dFactor * D) = > D * S + 0 * D = > D * S
						glBlendFuncSeparate(GL_DST_COLOR, GL_ZERO, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
					}
				}
			}

			if (rasterization.BackfaceCulling != current.BackfaceCulling)
			{
				FlushDrawCalls();
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
				FlushDrawCalls();
				glPolygonMode(GL_FRONT_AND_BACK, rasterization.Wireframe ? GL_LINE : GL_FILL);
			}

			if (rasterization.LineWidth != current.LineWidth)
			{
				FlushDrawCalls();
				glLineWidth(rasterization.LineWidth);
			}

			RendererData.CurrentState.CurrentRasterization = rasterization;
		}

		void SetBuffers(const RenderState::Buffers& buffers)
		{
			const RenderState::Buffers& current = RendererData.CurrentState.CurrentBuffers;

			if (buffers.ColorWrite != current.ColorWrite)
			{
				FlushDrawCalls();
				GLboolean colorWriting = buffers.ColorWrite ? GL_TRUE : GL_FALSE;
				glColorMask(colorWriting, colorWriting, colorWriting, colorWriting);
			}

			if (buffers.DepthWrite != current.DepthWrite)
			{
				FlushDrawCalls();
				glDepthMask(buffers.DepthWrite ? GL_TRUE : GL_FALSE);
			}

			if (buffers.DepthTest != current.DepthTest)
			{
				FlushDrawCalls();
				SetOpenGLCapability(GL_DEPTH_TEST, (buffers.DepthTest != Comparison::Always));
				glDepthFunc(TranslateComparison(buffers.DepthTest));
			}

			RendererData.CurrentState.CurrentBuffers = buffers;
		}

		void SetStencil(const RenderState::Stencil& stencil)
		{
			const RenderState::Stencil& current = RendererData.CurrentState.CurrentStencil;

			if (stencil.Write != current.Write || stencil.Test != current.Test)
			{
				FlushDrawCalls();
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
				FlushDrawCalls();
				glStencilFunc(TranslateComparison(stencil.Test), stencil.Reference, 0xFF);
			}

			if (stencil.StencilFail != current.StencilFail || stencil.DepthFail != current.DepthFail || stencil.DepthPass != current.DepthPass)
			{
				FlushDrawCalls();
				glStencilOp(
					TranslateStencilAction(stencil.StencilFail),
					TranslateStencilAction(stencil.DepthFail),
					TranslateStencilAction(stencil.DepthPass));
			}

			RendererData.CurrentState.CurrentStencil = stencil;
		}

		void SetupDrawCall()
		{
			const RenderState::States& state = RendererData.CurrentState;

			// Camera
			Bind(RendererData.CurrentProgram, "ViewMatrix", glm::make_mat4x4(state.CurrentCamera.ViewMatrix.M));
			Bind(RendererData.CurrentProgram, "ProjectionMatrix", glm::make_mat4x4(state.CurrentCamera.ProjectionMatrix.M));
			Bind(RendererData.CurrentProgram, "CameraUp", state.CurrentCamera.Up);
			Bind(RendererData.CurrentProgram, "CameraSide", state.CurrentCamera.Side);
			Bind(RendererData.CurrentProgram, "NearPlane", state.CurrentCamera.NearPlane);
			Bind(RendererData.CurrentProgram, "FarPlane", state.CurrentCamera.FarPlane);

			// Viewport
			static GLint viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport);
			Bind(RendererData.CurrentProgram, "ViewportSize", Vector2{ (float)viewport[2], (float)viewport[3] });

			// Fog
			Bind(RendererData.CurrentProgram, "FogIntensity", state.CurrentFog.Intensity);
			Bind(RendererData.CurrentProgram, "FogColor", state.CurrentFog.Color);

			// User Data
			Bind(RendererData.CurrentProgram, "Animation", state.CurrentUserData.Animation);
			Bind(RendererData.CurrentProgram, "UserData", state.CurrentUserData.UserData);
			Bind(RendererData.CurrentProgram, "UserMatrix", glm::make_mat4x4(state.CurrentUserData.UserMatrix.M));
		}

		void UploadSSBO(int instancesCount)
		{
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (uint64_t)instancesCount * FloatsPerTransform * sizeof(float), &RendererData.TransformsSSBO[0]);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, MaxInstances * FloatsPerTransform * sizeof(float), (uint64_t)instancesCount * FloatsPerOverlay * sizeof(float), &RendererData.OverlaysSSBO[0]);
		}

		void SendDrawCall(GLenum mode, GLint first, GLsizei count)
		{
			SetupDrawCall();

			static RenderState::ModelData emptyModelData = { Matrix::Identity, RGBA::TransparentBlack };
			CopyModelData(0, emptyModelData);
			UploadSSBO(1);

			glDrawArrays(mode, first, count);
		}

		void SendInstancedDrawCall(ResourceIndex mesh, int first, int count, int instancesCount)
		{
			SetupDrawCall();

			for (int i = 0; i < instancesCount; ++i)
			{
				CopyModelData(i, RendererData.Instances[i]);
			}
			UploadSSBO(instancesCount);

			glDrawArraysInstanced(GL_TRIANGLES, first, count, instancesCount);
		}

		void DrawMesh(ResourceIndex mesh, int first, int count, const RenderState::ModelData& modelData)
		{
			if (mesh != RendererData.CurrentInstanceMesh
				|| first != RendererData.CurrentInstanceMeshFirst
				|| count != RendererData.CurrentInstanceMeshCount
				|| RendererData.CurrentInstanceCount == MaxInstances)
			{
				FlushDrawCalls();
			}

			RendererData.CurrentInstanceMesh = mesh;
			RendererData.CurrentInstanceMeshFirst = first;
			RendererData.CurrentInstanceMeshCount = count;

			//if (RendererData.CurrentInstanceCount < MaxInstances)
			{
				RendererData.Instances[RendererData.CurrentInstanceCount++] = modelData;
			}
		}

		void FlushDrawCalls()
		{
			if (RendererData.CurrentVertexCount > 0)
			{
				Meshes::Bind(RendererData.BatchMesh);
				Meshes::Update(RendererData.BatchMesh, RendererData.Vertices, RendererData.CurrentVertexCount);

				GLenum primitiveType = GL_TRIANGLES;
				if (RendererData.CurrentPrimitiveType == PrimitiveType::Line)
				{
					primitiveType = GL_LINES;
				}

				SendDrawCall(primitiveType, 0, RendererData.CurrentVertexCount);
				RendererData.CurrentVertexCount = 0;
			}

			if (RendererData.CurrentInstanceCount > 0)
			{
				if (RendererData.CurrentInstanceMesh != InvalidResource && RendererData.CurrentInstanceMeshCount > 0)
				{
					Meshes::Bind(RendererData.CurrentInstanceMesh);

					SendInstancedDrawCall(GL_TRIANGLES, RendererData.CurrentInstanceMeshFirst, RendererData.CurrentInstanceMeshCount, RendererData.CurrentInstanceCount);
				}
				RendererData.CurrentInstanceCount = 0;
			}
		}

		void Init(int w, int h)
		{
			RendererData.BatchMesh = Meshes::Load(nullptr, MaxVerticesCount, true);

			GLuint ids[1];
			glGenTextures(1, ids);
			RendererData.EmptyTexture = ids[0];
			Resources.Textures.insert(Resources.Textures.begin(), { true, RendererData.EmptyTexture, 1, 1, TextureFlags::None });

			glBindTexture(GL_TEXTURE_2D, RendererData.EmptyTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			uint32_t pixels[1];
			pixels[0] = 0xFFFFFFFF;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)pixels);

			GLuint ssbo;
			glGenBuffers(1, &ssbo);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
			glBufferData(GL_SHADER_STORAGE_BUFFER, MaxInstances * (FloatsPerTransform + FloatsPerOverlay) * sizeof(float), nullptr, GL_STREAM_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
			RendererData.WorldMatrixSSBO = ssbo;

			RendererData.DefaultShader = Shaders::Create(
				"#version 330 core\n"
				"uniform sampler2D Texture1;\n"
				"uniform sampler2D Texture2;\n"
				"uniform sampler2D Texture3;\n"
				"uniform sampler2D Texture4;\n"
				"uniform sampler2D Texture5;\n"
				"uniform sampler2D Texture6;\n"
				"uniform sampler2D Texture7;\n"
				"uniform sampler2D Texture8;\n"
				"uniform float FogIntensity;\n"
				"uniform vec3 FogColor;\n"
				"uniform float Animation;\n"
				"uniform vec4 UserData;\n"
				"uniform mat4 UserMatrix;\n"
				"uniform float NearPlane;\n"
				"uniform float FarPlane;\n"
				"in vec2 share_UV;\n"
				"in vec4 share_Color;\n"
				"in vec4 share_Overlay;\n"
				"in vec4 share_ModelOverlay;\n"
				"in vec3 share_Position;\n"
				"in vec3 share_Normal;\n"
				"in float share_Distance;\n"
				"out vec4 out_Color;\n"
				"void main()\n"
				"{\n"
				"	vec4 texColor = texture2D(Texture1, share_UV) * share_Color;\n"
				"	if (texColor.a <= 0) discard;\n"
				"	out_Color = mix(texColor, vec4(share_Overlay.xyz, texColor.a), share_Overlay.a);\n"
				"	out_Color = mix(out_Color, vec4(FogColor,texColor.a), clamp(FogIntensity * share_Distance, 0.0, 1.0));\n"
				"	out_Color = mix(out_Color, vec4(share_ModelOverlay.xyz, 1.0), share_ModelOverlay.a);\n"
				"}\n");

			RendererData.FinalSurface = Surfaces::Create(w, h, SurfaceFlags::All);

			glEnable(GL_POLYGON_OFFSET_LINE);
			glPolygonOffset(-1.0f, -1.0f);
		}

		void Resize(int w, int h)
		{
			Surfaces::Delete(RendererData.FinalSurface);
			RendererData.FinalSurface = Surfaces::Create(w, h, SurfaceFlags::All);
		}

		void DrawFinalSurface(int w, int h)
		{
			FlushDrawCalls();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glViewport(0, 0, (GLsizei)w, (GLsizei)h);

			SetCamera({ Matrix(), Matrix(glm::value_ptr(glm::ortho(0.f, (float)w, (float)h, 0.f, -1.f, 1.f))), -1, 1, Vector3::Zero, Vector3::Zero });
			SetBuffers({ true, false, Comparison::Always });

			ResourceIndex texture = Surfaces::GetTexture(GetFinalSurface());
			int texWidth = Textures::GetWidth(texture);
			int texHeight = Textures::GetHeight(texture);
			float ratio = (float)w / (float)texWidth;

			int scaledW = (int)(texWidth * ratio);
			int scaledH = (int)(texHeight * ratio);
			float x = ((w - scaledW) * 0.5f);
			float y = ((h - scaledH) * 0.5f);
			w = scaledW;
			h = scaledH;

			SetMaterial({ InvalidResource, texture, InvalidResource, InvalidResource, InvalidResource, InvalidResource, InvalidResource, InvalidResource, InvalidResource });

			PushVertex({ { x, y + (float)h, 0.f }, Vector3::Zero, { 0.f, 0.f }, Vector2::Zero, RGBA::White, RGBA::TransparentBlack });
			PushVertex({ { x + (float)w, y + (float)h, 0.f }, Vector3::Zero, { 1.f, 0.f }, Vector2::Zero, RGBA::White, RGBA::TransparentBlack });
			PushVertex({ { x + (float)w, y, 0.f }, Vector3::Zero, { 1.f, 1.f }, Vector2::Zero, RGBA::White, RGBA::TransparentBlack });

			PushVertex({ { x, y + (float)h, 0.f }, Vector3::Zero, { 0.f, 0.f }, Vector2::Zero, RGBA::White, RGBA::TransparentBlack });
			PushVertex({ { x + (float)w, y, 0.f }, Vector3::Zero, { 1.f, 1.f }, Vector2::Zero, RGBA::White, RGBA::TransparentBlack });
			PushVertex({ { x, y, 0.f }, Vector3::Zero, { 0.f, 1.f }, Vector2::Zero, RGBA::White, RGBA::TransparentBlack });

			FlushDrawCalls();
		}

		void SetPrimitiveType(PrimitiveType type)
		{
			if (RendererData.CurrentPrimitiveType != type)
			{
				FlushDrawCalls();
				RendererData.CurrentPrimitiveType = type;
			}
		}

		void PushVertex(const Vertex& v)
		{
			int maxVerticesCount = MaxVerticesCount;
			if (RendererData.CurrentPrimitiveType == PrimitiveType::Line)
			{
				maxVerticesCount--; // Make it a multiple of 2
			}
			if (RendererData.CurrentVertexCount == maxVerticesCount)
			{
				FlushDrawCalls();
			}
			RendererData.Vertices[RendererData.CurrentVertexCount++] = v;
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
		float* viewMatrix,
		float* projectionMatrix,
		float nearPlane, float farPlane,
		float upX, float upY, float upZ,
		float sideX, float sideY, float sideZ)
	{
		SPF::Renderer::SetCamera({ viewMatrix, projectionMatrix, nearPlane, farPlane, {upX, upY, upZ}, {sideX, sideY, sideZ} });
	}

	DLLExport void SPF_SetMaterial(int shader, int texture1, int texture2, int texture3, int texture4, int texture5, int texture6, int texture7, int texture8)
	{
		SPF::Renderer::SetMaterial(
			{
				(SPF::ResourceIndex)shader,
				(SPF::ResourceIndex)texture1,
				(SPF::ResourceIndex)texture2,
				(SPF::ResourceIndex)texture3,
				(SPF::ResourceIndex)texture4,
				(SPF::ResourceIndex)texture5,
				(SPF::ResourceIndex)texture6,
				(SPF::ResourceIndex)texture7,
				(SPF::ResourceIndex)texture8
			});
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

	DLLExport void SPF_SetStencil(bool write, int test, int reference, int stencilFail, int depthFail, int depthPass)
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