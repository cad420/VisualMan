
#include "shaderprogram.h"
#include <iostream>

#include "../opengl/openglutils.h"

#include "GL/gl3w.h"

#include "rendercontext.h"

namespace ysl
{
	namespace graphics
	{
		void GLSLShader::SetFromSource(const std::string& source)
		{
			this->source = source;
			CreateShader();
			Compile();
		}

		void GLSLShader::SetFromFile(const std::string& fileName)
		{
			assert(false);
			CreateShader();
			Compile();
		}

		void GLSLShader::CreateShader()
		{
			if (handle == 0)
			{
				GL(handle = glCreateShader(Type()))

				// Check Create Shader Errors
				compiled = false;
			}
		}

		bool GLSLShader::Compile()
		{
			CreateShader();
			if (compiled == false)
			{
				auto p = source.c_str();
				GL(glShaderSource(handle, 1, &p, nullptr));
				GL(glCompileShader(handle))

				int success;
				char infoLog[512];
				GL(glGetShaderiv(handle, GL_COMPILE_STATUS, &success));
				if (!success)
				{
					GL(glGetShaderInfoLog(handle, 512, NULL, infoLog));
					Log("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED, %s\n", infoLog);
					return compiled = false;
				}
				compiled = true;
			}
			return compiled;
		}

		void GLSLShader::DestroyShader()
		{
			if (handle != 0)
			{
				GL(glDeleteShader(handle));
				handle = 0;
				compiled = false;
			}
		}

		GLSLShader::~GLSLShader()
		{
			DestroyShader();
		}





		GLSLProgram::~GLSLProgram()
		{
			DestroyProgram();
		}

		void GLSLProgram::CreateProgram()
		{
			if (handle == 0)
			{
				GL(handle = glCreateProgram());
				linked = false;
			}
		}

		void GLSLProgram::DestroyProgram()
		{
			if (handle != 0)
			{
				GL(glDeleteProgram(handle));
				handle = 0;
				linked = false;
				//compiled = false;
			}
		}

		bool GLSLProgram::Link()
		{
			if (handle != 0)
			{
				if (linked == false)
				{
					GL(glLinkProgram(handle));

					// check for link errors
					int success;
					char infoLog[512];
					GL(glGetProgramiv(handle, GL_LINK_STATUS, &success));
					if (!success) 
					{
						glGetProgramInfoLog(handle, 512, NULL, infoLog);
						Log("ERROR::SHADER::PROGRAM::LINKING_FAILED\n:%s", infoLog);
						return false;
					}

					linked = true;
				}
			}
			return linked;
		}

		bool GLSLProgram::Linked()
		{
			return linked;
		}

		bool GLSLProgram::Reload()
		{
			DestroyProgram();
			CreateProgram();
			for (const auto& each : shaders)
				AttachShader(each);
			return true;
		}

		void GLSLProgram::AttachShader(Ref<GLSLShader> shader)
		{
			// We need implement a state-independent operation
			if (shader == nullptr)
				return;
			CreateProgram();
			DetachShader(shader);
			shader->Compile();
			GL(glAttachShader(handle, shader->Handle()));
			shaders.push_back(std::move(shader));
			linked = false;
			Link();
		}

		void GLSLProgram::DetachShader(Ref<GLSLShader> shader)
		{
			if (handle == 0)
				return;
			if (shader == nullptr)
				return;
			for (auto it = shaders.begin(); it != shaders.end();)
			{
				if ((*it)->Handle() == shader->Handle())
				{
					GL(glDetachShader(handle, shader->Handle()));
					it = shaders.erase(it);
					//return;
				}else
				{
					++it;
				}
			}
		}

		void GLSLProgram::DetachAllShaders()
		{
			//if (handle == 0)
			//	return;
			for (auto it = shaders.begin(); it != shaders.end();)
			{
				GL(glDetachShader(handle, (*it)->Handle()));
				it = shaders.erase(it);
			}
		}

		void GLSLProgram::ApplyUniformSet(Ref<UniformSet> uset)
		{

		}

		void GLSLProgram::Apply(int index, const Camera * camera, RenderContext* context) const
		{
			assert(context);
			context->UseProgram(this);
			GL_CHECK
		}
	}
}
