#include <cstring>
#include <cstdio>
#include <Shaders.h>
#include <vector>
#include "GL4.h"
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
				printf("%s", log);
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
				printf("%s", log);
				FatalError(log);
			}
			return program;
		}

		void Init()
		{
			VertexShaderID = CompileShader(GL_VERTEX_SHADER,
				"#version 430 core\n"
				"layout (location = 0) in vec3 in_Position;\n"
				"layout (location = 1) in vec3 in_Normal;\n"
				"layout (location = 2) in vec4 in_UV;\n"
				"layout (location = 3) in vec4 in_Color;\n"
				"layout (location = 4) in vec4 in_Overlay;\n"
				"layout (std430, binding = 0) buffer ModelDataBuffer { mat4 transforms[5000]; vec4 overlays[5000]; };\n"
				"uniform mat4 ViewMatrix;\n"
				"uniform mat4 ProjectionMatrix;\n"
				"uniform vec3 CameraUp;\n"
				"uniform vec3 CameraSide;\n"
				"uniform vec2 ViewportSize;\n"
				"uniform float NearPlane;\n"
				"uniform float FarPlane;\n"
				"out vec2 share_UV;\n"
				"out vec4 share_Color;\n"
				"out vec4 share_Overlay;\n"
				"out vec4 share_ModelOverlay;\n"
				"out vec3 share_Position;\n"
				"out vec3 share_Normal;\n"
				"out float share_Distance;\n"
				"void main()\n"
				"{\n"
				"	vec3 actualPosition = in_Position + (in_UV.z * CameraSide) + (in_UV.w * CameraUp);\n"
				"	vec4 worldPosition = transforms[gl_InstanceID] * vec4(actualPosition, 1.0);\n"
				"   mat3 normalMatrix = mat3(transpose(inverse(transforms[gl_InstanceID])));\n"
				"	vec4 cameraPosition = ViewMatrix * worldPosition;\n"
				"	gl_Position = ProjectionMatrix * cameraPosition;\n"
				"	share_Normal = normalize(normalMatrix * in_Normal);\n"
				"   share_UV = in_UV.xy;\n"
				"	share_Color = in_Color;\n"
				"	share_ModelOverlay = overlays[gl_InstanceID];\n"
				"	share_Overlay = in_Overlay;\n"
				"	share_Position = worldPosition.xyz;\n"
				"	share_Distance = ((-cameraPosition.z * 2.0) - NearPlane) / (FarPlane - NearPlane);\n"
				"}\n");
		}

		ResourceIndex Create(const char* sourceCode)
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

		void Delete(ResourceIndex shader)
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
