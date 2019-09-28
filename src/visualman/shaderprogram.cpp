
#include "graphictype.h"
#include "shaderprogram.h"
#include "rendercontext.h"
#include <cassert>
#include <fstream>

namespace ysl
{
	namespace vm
	{
		void GLSLShader::SetFromSource(const std::string& source)
		{
			this->source = source;
			CreateShader();
			Compile();
		}

		void GLSLShader::SetFromFile(const std::string& fileName)
		{
			std::ifstream sourceFile(fileName,std::ios::in);
			if (!sourceFile.is_open())
			{

				::vm::Warning("Can not open shader source file. Path: %s\n",fileName.c_str());
				return;
			}
			const std::string text{std::istreambuf_iterator<char>{sourceFile},std::istreambuf_iterator<char>{} };
			SetFromSource(text);
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

				int success = 1;
				char infoLog[512];
				GL(glGetShaderiv(handle, GL_COMPILE_STATUS, &success));
				if (!success)
				{
					GL(glGetShaderInfoLog(handle, 512, NULL, infoLog));
					::vm::Log("ERROR::SHADER::COMPILATION_FAILED, %s\n", infoLog);
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
					PreLink();
					GL(glLinkProgram(handle));

					// check for link errors
					int success;
					char infoLog[512];
					GL(glGetProgramiv(handle, GL_LINK_STATUS, &success));
					if (!success)
					{
						glGetProgramInfoLog(handle, 512, NULL, infoLog);
						::vm::Log("ERROR::SHADER::PROGRAM::LINKING_FAILED\n:%s", infoLog);
						return false;
					}
					PostLink();
					linked = true;
				}
			}
			return linked;
		}

		bool GLSLProgram::Linked()
		{
			return handle && linked;
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
			//Link();
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
				}
				else
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

		void GLSLProgram::ApplyUniformSet(Ref<const UniformSet> uset)const

		{

			if (!uset)
				return;
			if (!linked)
				return;
			if (!handle)
				return;
#ifndef NDEBUG
			{
				int curProg = -1;
				GL(glGetIntegerv(GL_CURRENT_PROGRAM, &curProg));
				assert(curProg == handle);
			}
#endif

			const auto & arr = uset->Uniforms();

			for (const auto & uniform : arr)
			{
				int loc;
				GL(loc = glGetUniformLocation(handle, uniform->GetName().c_str()));

				if (loc == -1)continue;

				switch (uniform->Type())
				{
				case UT_INT:      GL(glUniform1iv(loc, uniform->Count(), uniform->Data<int>()));  break;
				case UT_INT_VEC2: GL(glUniform2iv(loc, uniform->Count(), uniform->Data<int>()));  break;
				case UT_INT_VEC3: GL(glUniform3iv(loc, uniform->Count(), uniform->Data<int>()));  break;
				case UT_INT_VEC4: GL(glUniform4iv(loc, uniform->Count(), uniform->Data<int>()));  break;

				case UT_UNSIGNED_INT:      GL(glUniform1uiv(loc, uniform->Count(), uniform->Data<unsigned int>()));  break;
				case UT_UNSIGNED_INT_VEC2: GL(glUniform2uiv(loc, uniform->Count(), uniform->Data<unsigned int>())); break;
				case UT_UNSIGNED_INT_VEC3: GL(glUniform3uiv(loc, uniform->Count(), uniform->Data<unsigned int>())); break;
				case UT_UNSIGNED_INT_VEC4: GL(glUniform4uiv(loc, uniform->Count(), uniform->Data<unsigned int>())); break;

				case UT_FLOAT:      GL(glUniform1fv(loc, uniform->Count(), uniform->Data<float>()));  break;
				case UT_FLOAT_VEC2: GL(glUniform2fv(loc, uniform->Count(), uniform->Data<float>()));  break;
				case UT_FLOAT_VEC3: GL(glUniform3fv(loc, uniform->Count(), uniform->Data<float>()));  break;
				case UT_FLOAT_VEC4: GL(glUniform4fv(loc, uniform->Count(), uniform->Data<float>()));  break;

				case UT_FLOAT_MAT2: GL(glUniformMatrix2fv(loc, uniform->Count(), GL_TRUE, uniform->Data<float>()));  break;
				case UT_FLOAT_MAT3: GL(glUniformMatrix3fv(loc, uniform->Count(), GL_TRUE, uniform->Data<float>()));  break;
				case UT_FLOAT_MAT4: GL(glUniformMatrix4fv(loc, uniform->Count(), GL_TRUE, uniform->Data<float>()));  break;

				case UT_FLOAT_MAT2x3: GL(glUniformMatrix2x3fv(loc, uniform->Count(), GL_TRUE, uniform->Data<float>()));  break;
				case UT_FLOAT_MAT3x2: GL(glUniformMatrix3x2fv(loc, uniform->Count(), GL_TRUE, uniform->Data<float>()));  break;
				case UT_FLOAT_MAT2x4: GL(glUniformMatrix2x4fv(loc, uniform->Count(), GL_TRUE, uniform->Data<float>()));  break;
				case UT_FLOAT_MAT4x2: GL(glUniformMatrix4x2fv(loc, uniform->Count(), GL_TRUE, uniform->Data<float>()));  break;
				case UT_FLOAT_MAT3x4: GL(glUniformMatrix3x4fv(loc, uniform->Count(), GL_TRUE, uniform->Data<float>()));  break;
				case UT_FLOAT_MAT4x3: GL(glUniformMatrix4x3fv(loc, uniform->Count(), GL_TRUE, uniform->Data<float>()));  break;

				case UT_DOUBLE:      GL(glUniform1dv(loc, uniform->Count(), uniform->Data<double>())); break;
				case UT_DOUBLE_VEC2: GL(glUniform2dv(loc, uniform->Count(), uniform->Data<double>())); break;
				case UT_DOUBLE_VEC3: GL(glUniform3dv(loc, uniform->Count(), uniform->Data<double>())); break;
				case UT_DOUBLE_VEC4: GL(glUniform4dv(loc, uniform->Count(), uniform->Data<double>())); break;

				case UT_DOUBLE_MAT2: GL(glUniformMatrix2dv(loc, uniform->Count(), GL_TRUE, uniform->Data<double>()));  break;
				case UT_DOUBLE_MAT3: GL(glUniformMatrix3dv(loc, uniform->Count(), GL_TRUE, uniform->Data<double>()));  break;
				case UT_DOUBLE_MAT4: GL(glUniformMatrix4dv(loc, uniform->Count(), GL_TRUE, uniform->Data<double>())); break;

				case UT_DOUBLE_MAT2x3: GL(glUniformMatrix2x3dv(loc, uniform->Count(), GL_TRUE, uniform->Data<double>())); break;
				case UT_DOUBLE_MAT3x2: GL(glUniformMatrix3x2dv(loc, uniform->Count(), GL_TRUE, uniform->Data<double>())); break;
				case UT_DOUBLE_MAT2x4: GL(glUniformMatrix2x4dv(loc, uniform->Count(), GL_TRUE, uniform->Data<double>())); break;
				case UT_DOUBLE_MAT4x2: GL(glUniformMatrix4x2dv(loc, uniform->Count(), GL_TRUE, uniform->Data<double>())); break;
				case UT_DOUBLE_MAT3x4: GL(glUniformMatrix3x4dv(loc, uniform->Count(), GL_TRUE, uniform->Data<double>())); break;
				case UT_DOUBLE_MAT4x3: GL(glUniformMatrix4x3dv(loc, uniform->Count(), GL_TRUE, uniform->Data<double>())); break;

				default:
					//assert(false);
					break;
				}
				GL_CHECK;
			}
		}

		int GLSLProgram::GetGenericUniformLocation(const char* name) const
		{
			int location = -1;
			if (handle != 0)
			{
				GL(location = glGetUniformLocation(handle, name));
			}
			return location;
		}

		int GLSLProgram::GetUniformBlockIndex(const char* name) const
		{
			int location = -1;
			if (handle != 0)
			{
				GL(location =glGetUniformBlockIndex(handle, name));
			}
			return location;
		}

		void GLSLProgram::BindFragDataLocation(int location, const char* name)
		{
			fragDataLocation[name] = location;
			linked = false;
		}

		void GLSLProgram::UnbindFragDataLocation(const char* name)
		{
			auto it = fragDataLocation.find(name);
			if (it != fragDataLocation.end())
			{
				fragDataLocation.erase(it);
			}
		}

		int GLSLProgram::FragDataLocation(const char* name)
		{
			auto it = fragDataLocation.find(name);
			if (it != fragDataLocation.end())
			{
				return it->second;
			}
			return -1;
		}

		int GLSLProgram::GetWorldMatrixUniformLocation() const
		{
			return uniformLocations.vpl_ModelMatrix;
		}

		int GLSLProgram::GetViewMatrixUniformLocation() const
		{
			return uniformLocations.vpl_ViewMatrix;
		}

		int GLSLProgram::GetProjectionMatrixUniformLocation() const
		{
			return uniformLocations.vpl_ProjectionMatrix;
		}

		int GLSLProgram::GetMVPUniformLocation() const
		{
			return uniformLocations.vpl_MVPMatrix;
		}

		int GLSLProgram::GetNormalMatrixUniformLocation() const
		{
			return uniformLocations.vpl_NormalMatrix;
		}

		int GLSLProgram::GetVertexPositionAttribLocation() const
		{
			return attribLocations.vpl_VertexPosition;
		}

		int GLSLProgram::VertexNormalAttribLocation() const
		{
			return attribLocations.vpl_VertexNormal;
		}

		int GLSLProgram::VertexTexCoordAttriLocation(int index) const
		{
			assert(index >= 0 && index < 5);
			return attribLocations.vpl_VertexTexCoord[index];
		}

		int GLSLProgram::VertexColorAttribLocation() const
		{
			return attribLocations.vpl_VertexColor;
		}

		Ref<Uniform> GLSLProgram::GetUniform(const char * name)
		{
			return CreateGetUniformSet()->GetUniform(name);
		}

		Ref<Uniform> GLSLProgram::CreateGetUniform(const char* name)
		{
			return CreateGetUniformSet()->CreateGetUniform(name);
		}

		void GLSLProgram::RemoveUniform(const char* name)
		{
			if (uniformSet)
				uniformSet->RemoveUniform(name);
		}

		void GLSLProgram::RemoveUniform(const Ref<Uniform>& uniform)
		{
			if (uniform)
				uniformSet->RemoveUniform(uniform);
		}

		void GLSLProgram::Apply(int index, const Camera * camera, RenderContext* context) const
		{
			assert(context);
			context->UseProgram(std::static_pointer_cast<const GLSLProgram>(shared_from_this()));
			GL_CHECK
		}

		void GLSLProgram::PreLink()
		{
			assert(handle);

			// Bind Fragment data location

			for (const auto &each : fragDataLocation)
			{
				GL(glBindFragDataLocation(handle, each.second, each.first.c_str()));
				//GL(glBindFragDataLocationIndexed(handle))
			}


			// bind pre-defined vertex attributes to pre-defined locations

			// return -1 if the attribute is not active

			GL(glBindAttribLocation(handle, VA_VertexPositionAttribLocation, "vpl_VertexPosition"));
			GL(glBindAttribLocation(handle, VA_VertexColorAttribLocation, "vpl_VertexColor"));
			GL(glBindAttribLocation(handle, VA_VertexNormalAttribLocation, "vpl_VertexNormal"));

			GL(glBindAttribLocation(handle, VA_VertexTexCoord0AttribLocation, "vpl_VertexTexCoord0"));
			GL(glBindAttribLocation(handle, VA_VertexTexCoord1AttribLocation, "vpl_VertexTexCoord1"));
			GL(glBindAttribLocation(handle, VA_VertexTexCoord2AttribLocation, "vpl_VertexTexCoord2"));
			GL(glBindAttribLocation(handle, VA_VertexTexCoord3AttribLocation, "vpl_VertexTexCoord3"));
			GL(glBindAttribLocation(handle, VA_VertexTexCoord4AttribLocation, "vpl_VertexTexCoord4"));

		}

		void GLSLProgram::PostLink()
		{
			assert(handle);
			// Get pre-defined uniforms' locations

			// return -1 if the uniform is not active
			GL(uniformLocations.vpl_ModelMatrix = glGetUniformLocation(handle, "vpl_ModelMatrix"));
			GL(uniformLocations.vpl_ViewMatrix = glGetUniformLocation(handle, "vpl_ViewMatrix"));
			GL(uniformLocations.vpl_ProjectionMatrix = glGetUniformLocation(handle, "vpl_ProjectionMatrix"));
			GL(uniformLocations.vpl_MVPMatrix = glGetUniformLocation(handle, "vpl_MVPMatrix"));
			GL(uniformLocations.vpl_NormalMatrix = glGetUniformLocation(handle, "vpl_NormalMatrix"));

			GL(attribLocations.vpl_VertexPosition = glGetAttribLocation(handle, "vpl_VertexPosition"));
			GL(attribLocations.vpl_VertexColor = glGetAttribLocation(handle, "vpl_VertexTexColor"));
			GL(attribLocations.vpl_VertexNormal = glGetAttribLocation(handle, "vpl_VertexNormal"));

			GL(attribLocations.vpl_VertexTexCoord[0] = glGetAttribLocation(handle, "vpl_VertexTexCoord1"));
			GL(attribLocations.vpl_VertexTexCoord[1] = glGetAttribLocation(handle, "vpl_VertexTexCoord2"));
			GL(attribLocations.vpl_VertexTexCoord[2] = glGetAttribLocation(handle, "vpl_VertexTexCoord3"));
			GL(attribLocations.vpl_VertexTexCoord[3] = glGetAttribLocation(handle, "vpl_VertexTexCoord4"));
			GL(attribLocations.vpl_VertexTexCoord[4] = glGetAttribLocation(handle, "vpl_VertexTexCoord5"));

		}
	}
}
