#include "shader.h"
#include <GL/glcorearb.h>
#include <GL/gl3w.h>
#include <error.h>
#include <fstream>
#include "openglutils.h"

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
		else if (type == ShaderType::Compute)
			shader = glCreateShader(GL_COMPUTE_SHADER);

		if (shader == 0)
		{
			Warning("Creates shader failed\n");
			return;
		}

		auto code = prog.c_str();
		glShaderSource(shader, 1, &code, nullptr);

		GL_ERROR_REPORT
		glCompileShader(shader);
		GL_ERROR_REPORT
		
		int success;
		char info[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, info);
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
		GL_ERROR_REPORT
	}

	int ShaderProgram::uniformLocation(const std::string& name) const
	{
		return uniformLocation(name.c_str());
	}

	void ShaderProgram::setUniformValue(int location, const ysl::Matrix4x4 & mat4x4)
	{
		glUniformMatrix4fv(location, 1, GL_TRUE, mat4x4.m[0]);
		GL_ERROR_REPORT;
	}

	void ShaderProgram::setUniformValue(int location, const ysl::Matrix3x3& mat3x3)
	{
		glUniformMatrix3fv(location, 1, GL_TRUE, mat3x3.m[0]);
		GL_ERROR_REPORT;
	}

	void ShaderProgram::setUniformValue(int location, const ysl::RGBSpectrum& rgb)
	{
		glUniform3fv(location, 1,rgb.c);
		GL_ERROR_REPORT;
	}

	void ShaderProgram::setUniformValue(int location, const ysl::RGBASpectrum& rgba)
	{
		glUniform4fv(location, 1, rgba.c);
		GL_ERROR_REPORT;
	}

	void ShaderProgram::setUniformValue(int location, const ysl::Vector3f& fvec3)
	{
		glUniform3fv(location, 1, &fvec3.x);
		GL_ERROR_REPORT;
	}

	void ShaderProgram::setUniformValue(int location, const ysl::Point3f& fpos3)
	{
		glUniform3fv(location, 1, &fpos3.x);
		GL_ERROR_REPORT;
	}

	void ShaderProgram::setUniformValue(int location, const ysl::Vector3i& ivec3)
	{
		glUniform3iv(location, 1, &ivec3.x);
		GL_ERROR_REPORT;
	}

	void ShaderProgram::setUniformValue(int location, const ysl::Vector2f& fvec2)
	{
		glUniform2fv(location, 1, &fvec2.x);
		GL_ERROR_REPORT;
	}

	void ShaderProgram::setUniformValue(int location, const ysl::Vector2i& ivec2)
	{
		glUniform2iv(location, 1, &ivec2.x);
		GL_ERROR_REPORT;
	}

	void ShaderProgram::setUniformSampler(int location, OpenGLTexture::TextureUnit texUnit, OpenGLTexture::TextureTarget target, unsigned textureId)
	{
		glActiveTexture(texUnit);
		glBindTexture(target, textureId);
		glUniform1i(location, texUnit - OpenGLTexture::TextureUnit0);
	}

	void ShaderProgram::setUniformSampler(int location, OpenGLTexture::TextureUnit texUnit,
		const OpenGLTexture& texture)
	{
		setUniformSampler(location, texUnit, texture.Target(), texture.NativeTextureId());
	}

	void ShaderProgram::bindFragDataLocation(int location, int colorNumber, const char* name)
	{
		glBindFragDataLocation(m_program, colorNumber, name);
		GL_ERROR_REPORT;
	}

	void ShaderProgram::setUniformValue(int location, bool value)
	{
		glUniform1i(location, int(value));
		GL_ERROR_REPORT
	}

	void ShaderProgram::setUniformValue(int location, int value)
	{
		glUniform1i(location, value);
		GL_ERROR_REPORT
	}

	void ShaderProgram::setUniformValue(int location, float value)
	{
		glUniform1f(location, value);
		GL_ERROR_REPORT
	}

	void ShaderProgram::setUniformValue(const char* name, const ysl::Matrix4x4& mat)
	{
		setUniformValue(uniformLocation(name), mat);
	}

	void ShaderProgram::setUniformValue(const char* name, const ysl::Matrix3x3& mat)
	{
		setUniformValue(uniformLocation(name), mat);
	}


	void ShaderProgram::setUniformValue(const char* name, const ysl::RGBSpectrum& rbg)
	{
		setUniformValue(uniformLocation(name), rbg);
	}

	void ShaderProgram::setUniformValue(const char* name, const ysl::RGBASpectrum& rgba)
	{
		setUniformValue(uniformLocation(name), rgba);
	}

	void ShaderProgram::setUniformValue(const char* name, const ysl::Vector3f& fvec3)
	{
		setUniformValue(uniformLocation(name), fvec3);
	}

	void ShaderProgram::setUniformValue(const char* name, const ysl::Point3f& fpos3)
	{
		setUniformValue(uniformLocation(name), fpos3);
	}

	void ShaderProgram::setUniformValue(const char* name, const ysl::Vector3i& ivec3)
	{
		setUniformValue(uniformLocation(name), ivec3);
	}

	void ShaderProgram::setUniformValue(const char* name, const ysl::Vector2f& fvec2)
	{
		setUniformValue(uniformLocation(name), fvec2);
	}

	void ShaderProgram::setUniformValue(const char* name, const ysl::Vector2i& ivec2)
	{
		setUniformValue(uniformLocation(name), ivec2);
	}

	void ShaderProgram::setUniformSampler(const char* name, OpenGLTexture::TextureUnit texUnit, OpenGLTexture::TextureTarget target,
		unsigned textureId)
	{
		setUniformSampler(uniformLocation(name), texUnit, target, textureId);
	}

	void ShaderProgram::setUniformSampler(const char* name, OpenGLTexture::TextureUnit texUnit,
		const OpenGLTexture& texture)
	{
		setUniformSampler(uniformLocation(name), texUnit, texture);
	}


	void ShaderProgram::setUniformValue(const char* name, bool value)
	{
		setUniformValue(uniformLocation(name), value);
	}

	void ShaderProgram::setUniformValue(const char * name, int value)
	{
		setUniformValue(uniformLocation(name), value);
	}

	void ShaderProgram::setUniformValue(const char* name, float value)
	{
		setUniformValue(uniformLocation(name), value);
	}
	void ShaderProgram::setUniformValue(const std::string & name, const ysl::Matrix4x4 & mat)
	{
		setUniformValue(name.c_str(), mat);
	}

	void ShaderProgram::setUniformValue(const std::string& name, const ysl::Matrix3x3& mat)
	{
		setUniformValue(name.c_str(), mat);
	}

	void ShaderProgram::setUniformValue(const std::string& name, const ysl::RGBSpectrum& rbg)
	{
		setUniformValue(name.c_str(), rbg);
	}

	void ShaderProgram::setUniformValue(const std::string& name, const ysl::RGBASpectrum& rgba)
	{
		setUniformValue(name.c_str(), rgba);
	}

	void ShaderProgram::setUniformValue(const std::string& name, const ysl::Vector3f& fvec3)
	{
		setUniformValue(name.c_str(), fvec3);
	}

	void ShaderProgram::setUniformValue(const std::string& name, const ysl::Point3f& fpos3)
	{
		setUniformValue(name.c_str(), fpos3);
	}

	void ShaderProgram::setUniformValue(const std::string& name, const ysl::Vector3i& ivec3)
	{
		setUniformValue(name.c_str(), ivec3);
	}

	void ShaderProgram::setUniformValue(const std::string& name, const ysl::Vector2f& fvec2)
	{
		setUniformValue(name.c_str(), fvec2);
	}

	void ShaderProgram::setUniformValue(const std::string& name, const ysl::Vector2i& ivec2)
	{
		setUniformValue(name.c_str(), ivec2);
	}

	void ShaderProgram::setUniformSampler(const std::string& name, OpenGLTexture::TextureUnit texUnit, OpenGLTexture::TextureTarget target,
		unsigned textureId)
	{
		setUniformSampler(name.c_str(), texUnit, target, textureId);
	}

	void ShaderProgram::setUniformSampler(const std::string& name, OpenGLTexture::TextureUnit texUnit,
		const OpenGLTexture& texture)
	{
		setUniformSampler(name.c_str(), texUnit, texture);
	}

	void ShaderProgram::setUniformValue(const std::string& name, bool value)
	{
		setUniformValue(name.c_str(), value);
	}

	void ShaderProgram::setUniformValue(const std::string& name, int value)
	{
		setUniformValue(name.c_str(), value);
	}

	void ShaderProgram::setUniformValue(const std::string& name, float value)
	{
		setUniformValue(name.c_str(), value);
	}
}
