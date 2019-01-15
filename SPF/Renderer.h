#pragma once


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

void InitRenderer(int w, int h)
{
	GLuint ids[1];
	glGenBuffers(1, ids);
	Data.BatchVBOID = ids[0];
	glBindBuffer(GL_ARRAY_BUFFER, Data.BatchVBOID);
	glBufferData(GL_ARRAY_BUFFER, VERTICES_COUNT * sizeof(Vertex), NULL, GL_STREAM_DRAW);

	glGenTextures(1, ids);
	Data.EmptyTexture = ids[0];

	glBindTexture(GL_TEXTURE_2D, Data.EmptyTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	uint32_t pixels[1];
	pixels[0] = 0xFFFFFFFF;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)pixels);

	Data.Program = CompileShader("#version 330 core\n"
		"layout (location = 0) in vec2 position;\n"
		"layout (location = 1) in vec2 uv;\n"
		"layout (location = 2) in vec4 color;\n"
		"uniform mat4 MVP;\n"
		"out vec2 tex_uv;\n"
		"out vec4 tex_color;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = MVP * vec4(position,0.0,1.0);\n"
		"   tex_uv = uv;\n"
		"	tex_color = color;\n"
		"}\n"
		,
		"#version 330 core\n"
		"uniform sampler2D Texture;\n"
		"in vec2 tex_uv;\n"
		"in vec4 tex_color;\n"
		"out vec4 out_color;\n"
		"void main()\n"
		"{\n"
		"	vec4 texColor = texture2D(Texture, tex_uv);\n"
		"	if (texColor.a <= 0) discard;\n"
		"	out_color = texColor * tex_color;\n"
		"}\n");

	glUseProgram(Data.Program);
	glUniform1i(glGetUniformLocation(Data.Program, "Texture"), 0);
}

void IssueVertices()
{
	glBindBuffer(GL_ARRAY_BUFFER, Data.BatchVBOID);
	glBufferData(GL_ARRAY_BUFFER, VERTICES_COUNT * sizeof(Vertex), &Data.Vertices, GL_STREAM_DRAW);

	glActiveTexture2(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Data.BatchInfo.CurrentTexture);

	glUseProgram(Data.Program);
	float m[16];
	SetupOrthographic(m, 0, (float)Data.CurrentWidth, (float)Data.CurrentHeight, 0, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(Data.Program, "MVP"), 1, GL_FALSE, m);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(8));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(16));

	glDrawArrays(GL_QUADS, 0, Data.BatchInfo.VertexCount);
	memset(&Data.BatchInfo, 0, sizeof(Data.BatchInfo));
}

void PushVertex(GLuint texture, int x, int y, float u, float v, float r, float g, float b, float a)
{
	if (Data.BatchInfo.VertexCount == VERTICES_COUNT || texture != Data.BatchInfo.CurrentTexture)
	{
		IssueVertices();
		Data.BatchInfo.CurrentTexture = texture;
	}
	Vertex* vertex = &Data.Vertices[Data.BatchInfo.VertexCount];
	vertex->X = (float)x;
	vertex->Y = (float)y;
	vertex->U = u;
	vertex->V = v;
	vertex->R = r;
	vertex->G = g;
	vertex->B = b;
	vertex->A = a;
	Data.BatchInfo.VertexCount++;
}

DLLExport void FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a)
{
	PushVertex(Data.EmptyTexture, x, y, 0, 0, r, g, b, a);
	PushVertex(Data.EmptyTexture, x+w, y,1, 0, r, g, b, a);
	PushVertex(Data.EmptyTexture, x+w, y+h, 1, 1, r, g, b, a);
	PushVertex(Data.EmptyTexture, x, y+h, 0, 1, r, g, b, a);
}

DLLExport void DrawTexture(int tex, int x, int y, int w, int h, int srcx, int srcy, int srcw, int srch, bool flipX, bool flipY, float r, float g, float b, float a)
{
	unsigned int id = Data.Textures[tex].GLID;
	int texW = Data.Textures[tex].Width;
	int texH = Data.Textures[tex].Height;
	float u1 = srcx / (float)texW;
	float u2 = (srcx+srcw) / (float)texW;
	float v1 = srcy / (float)texH;
	float v2 = (srcy+srch) / (float)texH;
	if (flipX)
	{
		float t = u1;
		u1 = u2;
		u2 = t;
	}
	if (Data.Textures[tex].Flipped)
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
	PushVertex(id, x, y, u1, v1, r, g, b, a);
	PushVertex(id, x + w, y, u2, v1, r, g, b, a);
	PushVertex(id, x + w, y + h, u2, v2, r, g, b, a);
	PushVertex(id, x, y + h, u1, v2, r, g, b, a);
}