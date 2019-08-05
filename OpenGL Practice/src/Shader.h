#pragma once

#include "External.h"

class Shader
{
private:
	GLuint create_shader(const std::string& source, GLenum type)
	{
		GLuint shader = glCreateShader(type);

		auto src_ptr = source.data();
		glShaderSource(shader, 1, &src_ptr, NULL);
		glCompileShader(shader);

		return shader;
	}
	GLuint create_shader_program(const std::string& vert_source, const std::string& frag_source)
	{
		GLuint vert = create_shader(vert_source, GL_VERTEX_SHADER);
		GLuint frag = create_shader(frag_source, GL_FRAGMENT_SHADER);

		GLuint prog = glCreateProgram();
		glAttachShader(prog, vert);
		glAttachShader(prog, frag);
		glLinkProgram(prog);

		GLint success;
		GLchar log[BUFSIZ];
		glGetProgramiv(prog, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(prog, BUFSIZ, NULL, log);
			std::cerr << log;
		}

		glDeleteShader(vert);
		glDeleteShader(frag);

		return prog;
	}

public:
	const GLuint id;

public:
	Shader(const std::string& vert_source, const std::string& frag_source) :
		id(create_shader_program(vert_source, frag_source))
	{
	}

	inline void use() const
	{
		glUseProgram(id);
	}
};