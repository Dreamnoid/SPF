#include <Shaders.h>
#include <vector>
#include "gl4.h"
#include "Resources.h"

namespace SPF
{
	namespace Shaders
	{
		GLuint VertexShaderID;

		GLuint CompileShader(GLenum type, const char* source)
		{
			GLuint shader = glCreateShader(type);
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

		GLuint LinkProgram(GLuint vert, GLuint frag)
		{
			GLuint program = glCreateProgram();
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

		void Shaders::Init()
		{
			VertexShaderID = CompileShader(GL_VERTEX_SHADER,
				"#version 330 core\n"
				"layout (location = 0) in vec3 in_Position;\n"
				"layout (location = 1) in vec4 in_UV;\n"
				"layout (location = 2) in vec4 in_Color;\n"
				"layout (location = 3) in vec4 in_Overlay;\n"
				"uniform mat4 MVP;\n"
				"uniform vec3 CameraUp;\n"
				"uniform vec3 CameraSide;\n"
				"uniform float FarPlane;\n"
				"out float share_Distance;\n"
				"out vec2 share_UV;\n"
				"out vec4 share_Color;\n"
				"out vec4 share_Overlay;\n"
				"void main()\n"
				"{\n"
				"	vec3 actualPosition = in_Position + (in_UV.z * CameraSide) + (in_UV.w * CameraUp);\n"
				"	gl_Position = MVP * vec4(actualPosition,1.0);\n"
				"	share_Distance = min(gl_Position.z / FarPlane,1);\n"
				"   share_UV = in_UV.xy;\n"
				"	share_Color = in_Color;\n"
				"	share_Overlay = in_Overlay;\n"
				"}\n");
		}

		ResourceIndex Shaders::Create(const char* sourceCode)
		{
			GLuint pixelShader = CompileShader(GL_FRAGMENT_SHADER, sourceCode);
			GLuint program = LinkProgram(VertexShaderID, pixelShader);
			glDeleteShader(pixelShader);

			glUseProgram(program);
			glUniform1i(glGetUniformLocation(program, "Texture"), 0);
			glUniform1i(glGetUniformLocation(program, "Texture1"), 1);

			return CreateResource(Resources.Shaders, { true, program });
		}

		void Shaders::Delete(ResourceIndex shader)
		{
			glDeleteProgram(Resources.Shaders[shader].GLID);
			DeleteResource(Resources.Shaders, shader);
		}
	}
}

extern "C"
{
	DLLExport int SPF_CreateShader(const char* sourceCode)
	{
		return SPF::Shaders::Create(sourceCode);
	}

	DLLExport void SPF_DeleteShader(int shader)
	{
		SPF::Shaders::Delete(shader);
	}
}
