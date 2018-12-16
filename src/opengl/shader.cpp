
#include "shader.h"
#include "../../lib/gl3w/GL/gl3w.h"
#include "../utility/error.h"

#include <fstream>

namespace ysl
{

	ShaderProgram::ShaderProgram() :
		m_program(0),
		m_shaderType(),
	m_created(false)
	{

	}

	void ShaderProgram::create()
	{
		// Check OpenGL Context
		if(m_program == 0)
		{
			m_program = glCreateProgram();
		}
		
		if(m_program == 0)
		{
			Warning("Shader Program can not be created\n");
			return;
		}
		m_created = true;
	}

	void ShaderProgram::addShaderFromFile(const std::string& fileName, ShaderType type)
	{
		std::ifstream sourceFile(fileName);
		if (!sourceFile.is_open())
		{
			Warning("Can not open shader source file.\n");
			return;
		}

		// Here, uniform initialization syntax is necessary. Think why
		const std::string prog {std::istreambuf_iterator<char>{sourceFile},
			std::istreambuf_iterator<char>{}};

		addShaderFromSourceCode(prog, type);
	}


	
	void ShaderProgram::addShaderFromSourceCode(const std::string & prog, ShaderType type)
	{

		auto shader = 0u;
		
		if (type == ShaderType::Vertex)
			shader = glCreateShader(GL_VERTEX_SHADER);
		else if (type == ShaderType::Fragment)
			shader = glCreateShader(GL_FRAGMENT_SHADER);

		if (shader == 0)
		{
			Warning("Creates shader failed\n");
			return;
		}

		auto code = prog.c_str();
		glShaderSource(shader, 1, &code, nullptr);
		glCompileShader(shader);
		int success;
		char info[512];
		glGetShaderiv(m_program, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(m_program, 512, NULL, info);
			Warning(info);
		}

		m_shaders.push_back(shader);
	}

	void ShaderProgram::link()
	{
		if(m_shaders.empty() || !m_program)
			return;

		for(auto shader:m_shaders)
			glAttachShader(m_program, shader);

		glLinkProgram(m_program);
		char info[512];
		int success;
		glGetProgramiv(m_program, GL_LINK_STATUS, &success);
		if(!success)
		{
			glGetProgramInfoLog(m_program, 512, nullptr, info);
			Warning(info);
			return;
		}

		for (auto shader : m_shaders)
			glDeleteShader(shader);
		m_shaders.clear();
	}

	void ShaderProgram::bind()
	{

		glUseProgram(m_program);
	}

	void ShaderProgram::unbind()
	{

		glUseProgram(0);
	}

	void ShaderProgram::destroy()
	{
		glDeleteProgram(m_program);
	}

	int ShaderProgram::uniformLocation(const char* name) const
	{
		if(m_program)
			return glGetUniformLocation(m_program, name);
	}

	void ShaderProgram::setUniformValue(const char* name, const ysl::Matrix4x4& mat)
	{
		glUniformMatrix4fv(uniformLocation(name), 1, GL_TRUE, mat.m[0]);
	}

	void ShaderProgram::setUniformValue(const char* name, const ysl::RGBSpectrum& rbg)
	{
		glUniform3fv(uniformLocation(name), 1, rbg.c);
	}

	void ShaderProgram::setUniformValue(const char* name, bool value)
	{
		glUniform1i(uniformLocation(name), value);
	}
}
