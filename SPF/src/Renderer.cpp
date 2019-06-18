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

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace SPF
{
	Renderer mRenderer;

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

	void Renderer::Init(int w, int h)
	{
		GLuint VAO;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		GLuint ids[1];
		glGenBuffers(1, ids);
		mBatchVBOID = ids[0];
		glBindBuffer(GL_ARRAY_BUFFER, mBatchVBOID);
		glBufferData(GL_ARRAY_BUFFER, VerticesCount * sizeof(Vertex), NULL, GL_STREAM_DRAW);

		glGenTextures(1, ids);
		mEmptyTexture = ids[0];

		glBindTexture(GL_TEXTURE_2D, mEmptyTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		uint32_t pixels[1];
		pixels[0] = 0xFFFFFFFF;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)pixels);

		mProgram = CompileShader("#version 330 core\n"
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

		glUseProgram(mProgram);
		glUniform1i(glGetUniformLocation(mProgram, "Texture"), 0);

		mFinalSurface = mSurfaces.Create(w, h, true);
	}

	void Renderer::Prepare()
	{
		glViewport(0, 0, (GLsizei)mCurrentWidth, (GLsizei)mCurrentHeight);

		glUseProgram(mProgram);
		glUniformMatrix4fv(glGetUniformLocation(mProgram, "MVP"), 1, GL_FALSE, mMVP);
		glUniform3f(glGetUniformLocation(mProgram, "CameraSide"), mCameraSideX, mCameraSideY, mCameraSideZ);
		glUniform1f(glGetUniformLocation(mProgram, "FogIntensity"), mFogIntensity);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0 * sizeof(float)));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(3 * sizeof(float)));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(7 * sizeof(float)));
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(11 * sizeof(float)));
	}

	void Renderer::IssueVertices()
	{
		if (mBatchInfo.VertexCount == 0) return;

		glBindBuffer(GL_ARRAY_BUFFER, mBatchVBOID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, VerticesCount * sizeof(Vertex), &mVertices);

		glActiveTexture2(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mBatchInfo.CurrentTexture);

		Prepare();
		glDrawArrays(GL_QUADS, 0, mBatchInfo.VertexCount);
		memset(&mBatchInfo, 0, sizeof(mBatchInfo));
	}

	void Renderer::PushVertex(HardwareID texture,
		float x, float y, float z,
		float u, float v, float bu, float bv,
		float r, float g, float b, float a,
		float overlayR, float overlayG, float overlayB, float overlayA)
	{
		if (mBatchInfo.VertexCount == VerticesCount || texture != mBatchInfo.CurrentTexture)
		{
			IssueVertices();
			mBatchInfo.CurrentTexture = texture;
		}
		Vertex* vertex = &mVertices[mBatchInfo.VertexCount];
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
		mBatchInfo.VertexCount++;
	}


	void Renderer::FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a)
	{
		PushVertex(mEmptyTexture, (float)x, (float)y, 0.f, 0.f, 0.f, 0.f, 0.f, r, g, b, a, 0.f, 0.f, 0.f, 0.f);
		PushVertex(mEmptyTexture, (float)(x + w), (float)y, 0.f, 1.f, 0.f, 0.f, 0.f, r, g, b, a, 0.f, 0.f, 0.f, 0.f);
		PushVertex(mEmptyTexture, (float)(x + w), (float)(y + h), 0.f, 1.f, 1.f, 0.f, 0.f, r, g, b, a, 0.f, 0.f, 0.f, 0.f);
		PushVertex(mEmptyTexture, (float)x, (float)(y + h), 0.f, 0.f, 1.f, 0.f, 0.f, r, g, b, a, 0.f, 0.f, 0.f, 0.f);
	}

	void Renderer::DrawTexturedQuad(
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
		unsigned int id = mTextures.Get(tex).GLID;
		int texW = mTextures.Get(tex).Width;
		int texH = mTextures.Get(tex).Height;
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
		if (mTextures.Get(tex).Flipped)
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

	void Renderer::DrawMesh(ResourceIndex tex, ResourceIndex mesh)
	{
		IssueVertices();

		glBindBuffer(GL_ARRAY_BUFFER, mMeshes.Get(mesh).GLID);

		glActiveTexture2(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mTextures.Get(tex).GLID);

		Prepare();
		glDrawArrays(GL_TRIANGLES, 0, mMeshes.Get(mesh).VerticesCount);
	}

	void Renderer::SetBlending(BlendMode blendMode)
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

	void Renderer::Begin(ResourceIndex surface)
	{
		IssueVertices();

		surface = (surface < 0) ? mFinalSurface : surface;
		ResourceIndex texture = mSurfaces.GetTexture(surface);
		glBindFramebuffer(GL_FRAMEBUFFER, mSurfaces.Get(surface).GLID);

		mCurrentWidth = mTextures.Get(texture).Width;
		mCurrentHeight = mTextures.Get(texture).Height;
		SetupOrthographic(mMVP, 0, (float)mCurrentWidth, (float)mCurrentHeight, 0, -1.0f, 1.0f);
		mCameraSideX = 0.f;
		mCameraSideY = 0.f;
		mCameraSideZ = 0.f;

		glDisable(GL_DEPTH_TEST);
		mFogIntensity = 0.f;
	}

	void Renderer::BeginLookAtPerspective(ResourceIndex surface,
		float cameraX, float cameraY, float cameraZ,
		float cameraTargetX, float cameraTargetY, float cameraTargetZ,
		float fov, float nearDist, float farDist, float fogIntensity)
	{
		IssueVertices();

		surface = (surface < 0) ? mFinalSurface : surface;
		ResourceIndex texture = mSurfaces.GetTexture(surface);
		glBindFramebuffer(GL_FRAMEBUFFER, mSurfaces.Get(surface).GLID);

		mCurrentWidth = mTextures.Get(texture).Width;
		mCurrentHeight = mTextures.Get(texture).Height;

		auto cameraPos = glm::vec3(cameraX, cameraY, cameraZ);
		auto cameraTarget = glm::vec3(cameraTargetX, cameraTargetY, cameraTargetZ);
		auto cameraForward = glm::normalize(cameraTarget - cameraPos);
		auto cameraSide = glm::cross(cameraForward, glm::vec3(0.f, 1.f, 0.f));
		mCameraSideX = cameraSide.x;
		mCameraSideY = cameraSide.y;
		mCameraSideZ = cameraSide.z;

		auto mvp = glm::perspectiveFov(fov, (float)mCurrentWidth, (float)mCurrentHeight, nearDist, farDist) * glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.f, 1.f, 0.f));

		float* ptrMVP = glm::value_ptr(mvp);
		for (int i = 0; i < 16; ++i)
		{
			mMVP[i] = ptrMVP[i];
		}

		glEnable(GL_DEPTH_TEST);
		mFogIntensity = fogIntensity;
	}

	void Renderer::DrawFinalSurface(int w, int h)
	{
		IssueVertices();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		mCurrentWidth = w;
		mCurrentHeight = h;
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		SetupOrthographic(mMVP, 0, (float)mCurrentWidth, (float)mCurrentHeight, 0, -1.0f, 1.0f);

		ResourceIndex texture = mSurfaces.GetTexture(mFinalSurface);
		DrawTexturedQuad(texture,
			0.f, 0.f, 0.f,
			(float)w, 0.f, 0.f,
			(float)w, (float)h, 0.f,
			0.f, (float)h, 0.f,
			0, 0, mTextures.GetWidth(texture), mTextures.GetHeight(texture),
			false, false,
			1.f, 1.f, 1.f, 1.f,
			0.f, 0.f, 0.f, 0.f);
		IssueVertices();
	}

	void Renderer::DrawBillboard(ResourceIndex tex,
		float x, float y, float z, float radius,
		int srcx, int srcy, int srcw, int srch,
		bool flipX, bool flipY,
		float r, float g, float b, float a,
		float overlayR, float overlayG, float overlayB, float overlayA)
	{
		unsigned int id = mTextures.Get(tex).GLID;
		int texW = mTextures.Get(tex).Width;
		int texH = mTextures.Get(tex).Height;
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
		if (mTextures.Get(tex).Flipped)
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

		float halfRadius = radius * 0.5f;

		PushVertex(id, x, y, z, u1, v1, -halfRadius, radius, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
		PushVertex(id, x, y, z, u2, v1, +halfRadius, radius, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
		PushVertex(id, x, y, z, u2, v2, +halfRadius, 0.f, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
		PushVertex(id, x, y, z, u1, v2, -halfRadius, 0.f, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
	}
}

extern "C"
{
	DLLExport void FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a)
	{
		SPF::mRenderer.FillRectangle(x, y, w, h, r, g, b, a);
	}

	DLLExport void DrawTexturedQuad(
		SPF::ResourceIndex tex,
		float Ax, float Ay, float Az,
		float Bx, float By, float Bz,
		float Cx, float Cy, float Cz,
		float Dx, float Dy, float Dz,
		int srcx, int srcy, int srcw, int srch,
		bool flipX, bool flipY,
		float r, float g, float b, float a,
		float overlayR, float overlayG, float overlayB, float overlayA)
	{
		SPF::mRenderer.DrawTexturedQuad(tex,
			Ax, Ay, Az,
			Bx, By, Bz,
			Cx, Cy, Cz,
			Dx, Dy, Dz,
			srcx, srcy, srcw, srch,
			flipX, flipY,
			r, g, b, a,
			overlayR, overlayG, overlayB, overlayA);
	}

	DLLExport void DrawTexture(SPF::ResourceIndex tex,
		int x, int y, int w, int h,
		int srcx, int srcy, int srcw, int srch,
		bool flipX, bool flipY,
		float r, float g, float b, float a,
		float overlayR, float overlayG, float overlayB, float overlayA)
	{
		SPF::mRenderer.DrawTexturedQuad(tex,
			(float)x, (float)y, 0.f,
			(float)(x + w), (float)y, 0.f,
			(float)(x + w), (float)(y + h), 0.f,
			(float)x, (float)(y + h), 0.f,
			srcx, srcy, srcw, srch,
			flipX, flipY,
			r, g, b, a,
			overlayR, overlayG, overlayB, overlayA);
	}

	DLLExport void DrawMesh(SPF::ResourceIndex tex, SPF::ResourceIndex mesh)
	{
		SPF::mRenderer.DrawMesh(tex, mesh);
	}

	DLLExport void SetBlending(int blendMode)
	{
		SPF::mRenderer.SetBlending((SPF::BlendMode)blendMode);
	}

	DLLExport void Begin(SPF::ResourceIndex surface)
	{
		SPF::mRenderer.Begin(surface);
	}

	DLLExport void BeginLookAtPerspective(SPF::ResourceIndex surface,
		float cameraX, float cameraY, float cameraZ,
		float cameraTargetX, float cameraTargetY, float cameraTargetZ,
		float fov, float nearDist, float farDist, float fogIntensity)
	{
		SPF::mRenderer.BeginLookAtPerspective(surface,
			cameraX, cameraY, cameraZ, 
			cameraTargetX, cameraTargetY, cameraTargetZ,
			fov, nearDist, farDist, fogIntensity);
	}

	DLLExport void DrawBillboard(SPF::ResourceIndex tex,
		float x, float y, float z, float radius,
		int srcx, int srcy, int srcw, int srch,
		bool flipX, bool flipY,
		float r, float g, float b, float a,
		float overlayR, float overlayG, float overlayB, float overlayA)
	{
		SPF::mRenderer.DrawBillboard(tex,
			x, y, z, radius, 
			srcx, srcy, srcw, srch,
			flipX, flipY, 
			r, g, b, a,
			overlayR, overlayG, overlayB, overlayA);
	}
}