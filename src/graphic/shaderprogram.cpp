
#include "shaderprogram.h"
#include <iostream>

namespace ysl
{
	namespace gpu
	{
		void GLSLShader::SetFromSource(const std::string& source)
		{

		}

		void GLSLShader::SetFromFile(const std::string& fileName)
		{

		}

		void GLSLShader::CreateShader()
		{
		}

		bool GLSLShader::Compile()
		{
			return false;
		}

		void GLSLProgram::CreateProgram()
		{

		}

		void GLSLProgram::DestroyProgram()
		{

		}

		void GLSLProgram::Link()
		{

		}

		bool GLSLProgram::Linked()
		{
			return false;
		}

		void GLSLProgram::AttachShader(Ref<GLSLShader> shader)
		{

		}

		void GLSLProgram::DetachShader(Ref<GLSLShader> shader)
		{
		}

		void GLSLProgram::DetachAllShaders()
		{
		}



		void GLSLProgram::ApplyUniformSet(Ref<UniformSet> uset)
		{
		}

		void GLSLProgram::Apply(int index, Ref<Camera> camera, RenderContext* context) const
		{
			std::cout << "GLSLProgram::Apply\n";
		}
	}
}
