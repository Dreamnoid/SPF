#include "Renderer.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include "gl4.h"
#include <cstdio>
#include "Surfaces.h"
#include "Textures.h"

Renderer mRenderer;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

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
		"layout (location = 0) in vec2 in_Position;\n"
		"layout (location = 1) in vec2 in_UV;\n"
		"layout (location = 2) in vec4 in_Color;\n"
		"layout (location = 3) in vec4 in_OverlayColor;\n"
		"uniform mat4 MVP;\n"
		"out vec2 share_UV;\n"
		"out vec4 share_Color;\n"
		"out vec4 share_OverlayColor;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = MVP * vec4(in_Position,0.0,1.0);\n"
		"   share_UV = in_UV;\n"
		"	share_Color = in_Color;\n"
		"	share_OverlayColor = in_OverlayColor;\n"
		"}\n"
		,
		"#version 330 core\n"
		"uniform sampler2D Texture;\n"
		"in vec2 share_UV;\n"
		"in vec4 share_Color;\n"
		"in vec4 share_OverlayColor;\n"
		"out vec4 out_Color;\n"
		"void main()\n"
		"{\n"
		"	vec4 texColor = texture2D(Texture, share_UV) * share_Color;\n"
		"	if (texColor.a <= 0) discard;\n"
		"	out_Color = mix(texColor, vec4(share_OverlayColor.rgb, texColor.a), share_OverlayColor.a);\n"
		"}\n");

	glUseProgram(mProgram);
	glUniform1i(glGetUniformLocation(mProgram, "Texture"), 0);

	mFinalSurface = mSurfaces.Create(w, h);
}

void Renderer::IssueVertices()
{
	if (mBatchInfo.VertexCount == 0) return;

	glBindBuffer(GL_ARRAY_BUFFER, mBatchVBOID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, VerticesCount * sizeof(Vertex), &mVertices);

	glActiveTexture2(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mBatchInfo.CurrentTexture);

	glUseProgram(mProgram);
	float m[16];
	SetupOrthographic(m, 0, (float)mCurrentWidth, (float)mCurrentHeight, 0, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(mProgram, "MVP"), 1, GL_FALSE, m);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0 * sizeof(float)));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(2 * sizeof(float)));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(4 * sizeof(float)));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(8 * sizeof(float)));

	glDrawArrays(GL_QUADS, 0, mBatchInfo.VertexCount);
	memset(&mBatchInfo, 0, sizeof(mBatchInfo));
}

void Renderer::PushVertex(GLuint texture, int x, int y, float u, float v,
	float r, float g, float b, float a,
	float overlayR, float overlayG, float overlayB, float overlayA)
{
	if (mBatchInfo.VertexCount == VerticesCount || texture != mBatchInfo.CurrentTexture)
	{
		IssueVertices();
		mBatchInfo.CurrentTexture = texture;
	}
	Vertex* vertex = &mVertices[mBatchInfo.VertexCount];
	vertex->X = (float)x;
	vertex->Y = (float)y;
	vertex->U = u;
	vertex->V = v;
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
	PushVertex(mEmptyTexture, x, y, 0, 0, r, g, b, a, 0.f, 0.f, 0.f, 0.f);
	PushVertex(mEmptyTexture, x + w, y, 1, 0, r, g, b, a, 0.f, 0.f, 0.f, 0.f);
	PushVertex(mEmptyTexture, x + w, y + h, 1, 1, r, g, b, a, 0.f, 0.f, 0.f, 0.f);
	PushVertex(mEmptyTexture, x, y + h, 0, 1, r, g, b, a, 0.f, 0.f, 0.f, 0.f);
}

void Renderer::DrawTexturedQuad(
	int tex,
	float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Dx, float Dy,
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
	PushVertex(id, Ax, Ay, u1, v1, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
	PushVertex(id, Bx, By, u2, v1, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
	PushVertex(id, Cx, Cy, u2, v2, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
	PushVertex(id, Dx, Dy, u1, v2, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
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
	surface = (surface < 0) ? mFinalSurface : surface;
	ResourceIndex texture = mSurfaces.GetTexture(surface);
	glBindFramebuffer(GL_FRAMEBUFFER, mSurfaces.Get(surface).GLID);
	mCurrentWidth = mTextures.Get(texture).Width;
	mCurrentHeight = mTextures.Get(texture).Height;
	glViewport(0, 0, (GLsizei)mCurrentWidth, (GLsizei)mCurrentHeight);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::DrawFinalSurface(int w, int h)
{
	IssueVertices();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	mCurrentWidth = w;
	mCurrentHeight = h;
	glViewport(0, 0, (GLsizei)mCurrentWidth, (GLsizei)mCurrentHeight);
	glClear(GL_COLOR_BUFFER_BIT);

	ResourceIndex texture = mSurfaces.GetTexture(mFinalSurface);
	DrawTexturedQuad(texture,
		0, 0,
		w, 0, 
		w, h, 
		0, h, 
		0, 0, mTextures.GetWidth(texture), mTextures.GetHeight(texture),
		false, false,
		1, 1, 1, 1,
		0, 0, 0, 0);
	IssueVertices();
}

void Renderer::End()
{
	IssueVertices();
	Begin(mFinalSurface);
}

extern "C"
{
	DLLExport void FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a)
	{
		mRenderer.FillRectangle(x, y, w, h, r, g, b, a);
	}

	DLLExport void DrawTexturedQuad(
		int tex,
		float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Dx, float Dy,
		int srcx, int srcy, int srcw, int srch,
		bool flipX, bool flipY,
		float r, float g, float b, float a,
		float overlayR, float overlayG, float overlayB, float overlayA)
	{
		mRenderer.DrawTexturedQuad(tex,
			Ax, Ay, Bx, By, Cx, Cy, Dx, Dy,
			srcx, srcy, srcw, srch,
			flipX, flipY,
			r, g, b, a,
			overlayR, overlayG, overlayB, overlayA);
	}

	DLLExport void DrawTexture(int tex,
		int x, int y, int w, int h,
		int srcx, int srcy, int srcw, int srch,
		bool flipX, bool flipY,
		float r, float g, float b, float a,
		float overlayR, float overlayG, float overlayB, float overlayA)
	{
		mRenderer.DrawTexturedQuad(tex,
			x, y,
			x + w, y,
			x + w, y + h,
			x, y + h,
			srcx, srcy, srcw, srch,
			flipX, flipY,
			r, g, b, a,
			overlayR, overlayG, overlayB, overlayA);
	}

	DLLExport void SetBlending(int blendMode)
	{
		mRenderer.SetBlending((BlendMode)blendMode);
	}

	DLLExport void BeginSurface(ResourceIndex surface)
	{
		mRenderer.Begin(surface);
	}

	DLLExport void EndSurface()
	{
		mRenderer.End();
	}
}