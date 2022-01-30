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
				FatalError(log);
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
				FatalError(log);
			}
			return program;
		}

		void Shaders::Init()
		{
			VertexShaderID = CompileShader(GL_VERTEX_SHADER,
				"#version 330 core\n"
				"layout (location = 0) in vec3 in_Position;\n"
				"layout (location = 1) in vec3 in_Normal;\n"
				"layout (location = 2) in vec4 in_UV;\n"
				"layout (location = 3) in vec4 in_Color;\n"
				"layout (location = 4) in vec4 in_Overlay;\n"
				"uniform mat4 WorldMatrix;\n"
				"uniform mat4 ViewProjectionMatrix;\n"
				"uniform vec3 CameraUp;\n"
				"uniform vec3 CameraSide;\n"
				"uniform float FarPlane;\n"
				"uniform vec2 ViewportSize;\n"
				"out vec2 share_UV;\n"
				"out vec4 share_Color;\n"
				"out vec4 share_Overlay;\n"
				"out vec3 share_Position;\n"
				"out vec3 share_Normal;\n"
				"void main()\n"
				"{\n"
				"	vec3 actualPosition = in_Position + (in_UV.z * CameraSide) + (in_UV.w * CameraUp);\n"
				"	vec4 worldPosition = WorldMatrix * vec4(actualPosition, 1.0);\n"
				"	gl_Position = ViewProjectionMatrix * worldPosition;\n"
				"	share_Normal = (WorldMatrix * vec4(in_Normal, 0.0)).xyz;\n"
				"   share_UV = in_UV.xy;\n"
				"	share_Color = in_Color;\n"
				"	share_Overlay = in_Overlay;\n"
				"	share_Position = worldPosition.xyz;\n"
				"}\n");
		}

		ResourceIndex Shaders::Create(const char* sourceCode)
		{
			GLuint pixelShader = CompileShader(GL_FRAGMENT_SHADER, sourceCode);
			GLuint program = LinkProgram(VertexShaderID, pixelShader);
			glDeleteShader(pixelShader);

			glUseProgram(program);
			glUniform1i(glGetUniformLocation(program, "Texture1"), 0);
			glUniform1i(glGetUniformLocation(program, "Texture2"), 1);
			glUniform1i(glGetUniformLocation(program, "Texture3"), 2);
			glUniform1i(glGetUniformLocation(program, "Texture4"), 3);
			glUniform1i(glGetUniformLocation(program, "Texture5"), 4);
			glUniform1i(glGetUniformLocation(program, "Texture6"), 5);
			glUniform1i(glGetUniformLocation(program, "Texture7"), 6);
			glUniform1i(glGetUniformLocation(program, "Texture8"), 7);

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
