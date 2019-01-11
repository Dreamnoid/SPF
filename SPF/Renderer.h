#pragma once

typedef struct
{
	float X;
	float Y;
	float U;
	float V;
	float R;
	float G;
	float B;
	float A;
} Vertex;
#define MAX_SPRITES 2000
#define VERTICES_PER_SPRITE 4
#define VERTICES_COUNT MAX_SPRITES * VERTICES_PER_SPRITE
Vertex Vertices[VERTICES_COUNT];
int VertexCount = 0;

GLuint _currentTexture;

GLuint _batchVBOID;
GLuint _emptyTexture;
GLuint _program;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))


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

DLLExport void DeleteShader(unsigned int id)
{
	glDeleteProgram(id);
}

void InitRenderer(int w, int h)
{
	GLuint ids[1];
	glGenBuffers(1, ids);
	_batchVBOID = ids[0];
	glBindBuffer(GL_ARRAY_BUFFER, _batchVBOID);
	glBufferData(GL_ARRAY_BUFFER, VERTICES_COUNT * sizeof(Vertex), NULL, GL_STREAM_DRAW);

	glGenTextures(1, ids);
	_emptyTexture = ids[0];

	glBindTexture(GL_TEXTURE_2D, _emptyTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	uint32_t pixels[1];
	pixels[0] = 0xFFFFFFFF;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)pixels);

	_program = CompileShader("#version 330 core\n"
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

	glUseProgram(_program);
	glUniform1i(glGetUniformLocation(_program, "Texture"), 0);
	float m[16];
	SetupOrthographic(m, 0, (float)w, (float)h, 0, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(_program, "MVP"), 1, GL_FALSE, m);

}

void IssueVertices()
{
	glBindBuffer(GL_ARRAY_BUFFER, _batchVBOID);
	glBufferData(GL_ARRAY_BUFFER, VERTICES_COUNT * sizeof(Vertex), &Vertices, GL_STREAM_DRAW);

	glActiveTexture2(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _currentTexture);

	glUseProgram(_program);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(8));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(16));

	glDrawArrays(GL_QUADS, 0, VertexCount);
	VertexCount = 0;
}

void PushVertex(GLuint texture, int x, int y, float u, float v, float r, float g, float b, float a)
{
	if (VertexCount == VERTICES_COUNT || texture != _currentTexture)
	{
		IssueVertices();
		_currentTexture = texture;
	}
	Vertex* vertex = &Vertices[VertexCount];
	vertex->X = (float)x;
	vertex->Y = (float)y;
	vertex->U = u;
	vertex->V = v;
	vertex->R = r;
	vertex->G = g;
	vertex->B = b;
	vertex->A = a;
	VertexCount++;
}

DLLExport void FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a)
{
	PushVertex(_emptyTexture, x, y, 0, 0, r, g, b, a);
	PushVertex(_emptyTexture, x+w, y,1, 0, r, g, b, a);
	PushVertex(_emptyTexture, x+w, y+h, 1, 1, r, g, b, a);
	PushVertex(_emptyTexture, x, y+h, 0, 1, r, g, b, a);
}

DLLExport void DrawTexture(int tex, int x, int y, int w, int h, float r, float g, float b, float a)
{
	unsigned int id = Textures[tex].GLID;
	PushVertex(id, x, y, 0, 0, r, g, b, a);
	PushVertex(id, x + w, y, 1, 0, r, g, b, a);
	PushVertex(id, x + w, y + h, 1, 1, r, g, b, a);
	PushVertex(id, x, y + h, 0, 1, r, g, b, a);
}

