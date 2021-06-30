#pragma once
#include "GL4.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace SPF
{
	void SetOpenGLCapability(GLenum cap, bool state)
	{
		if (state) glEnable(cap);
		else glDisable(cap);
	}

	GLenum TranslateComparison(Comparison comparison)
	{
		switch (comparison)
		{
		case Comparison::Always: return GL_ALWAYS;
		case Comparison::Never: return GL_NEVER;
		case Comparison::Less: return GL_LESS;
		case Comparison::LessOrEqual: return GL_LEQUAL;
		case Comparison::Greater: return GL_GREATER;
		case Comparison::GreaterOrEqual: return GL_GEQUAL;
		case Comparison::Equal: return GL_EQUAL;
		case Comparison::NotEqual: return GL_NOTEQUAL;
		default: return GL_ALWAYS;
		}
	}

	GLenum TranslateStencilAction(StencilAction action)
	{
		switch (action)
		{
		case StencilAction::Keep: return GL_KEEP;
		case StencilAction::Replace: return GL_REPLACE;
		case StencilAction::Increase: return GL_INCR;
		default: return GL_KEEP;
		}
	}

	void Bind(HardwareID programID, const char* name, const Vector3& data)
	{
		glUniform3f(glGetUniformLocation(programID, name), data.X, data.Y, data.Z);
	}

	void Bind(HardwareID programID, const char* name, const Vector4& vector)
	{
		glUniform4f(glGetUniformLocation(programID, name), vector.X, vector.Y, vector.Z, vector.W);
	}

	void Bind(HardwareID programID, const char* name, const RGBA& data)
	{
		glUniform4f(glGetUniformLocation(programID, name), data.R, data.G, data.B, data.A);
	}

	void Bind(HardwareID programID, const char* name, const RGB& data)
	{
		glUniform3f(glGetUniformLocation(programID, name), data.R, data.G, data.B);
	}

	void Bind(HardwareID programID, const char* name, float data)
	{
		glUniform1f(glGetUniformLocation(programID, name), data);
	}

	void Bind(HardwareID programID, const char* name, const glm::mat4x4& data)
	{
		glUniformMatrix4fv(glGetUniformLocation(programID, name), 1, GL_FALSE, glm::value_ptr(data));
	}
}