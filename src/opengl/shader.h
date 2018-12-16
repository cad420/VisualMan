#ifndef _SHADER_H_
#define _SHADER_H_

#include "../mathematics/transformation.h"
#include "../mathematics/geometry.h"

#include <vector>
#include "spectrum.h"

namespace ysl
{
	class ShaderProgram
	{

		public:
			enum class ShaderType { Vertex, Fragment };

			ShaderProgram();
			void create();		// Check OpenGL context

			void addShaderFromFile(const std::string & fileName, ShaderType type);
			void addShaderFromSourceCode(const std::string & prog, ShaderType type);

			void link();
			void bind();
			void unbind();
			void destroy();

			int uniformLocation(const char * name)const;

			void setUniformValue(const char * name, const ysl::Matrix4x4 & mat);

			void setUniformValue(const char * name, const ysl::RGBSpectrum & rbg);

			void setUniformValue(const char * name, bool value);

			//void addShader(const & prog);
			//int attributeLocation(const char * name)const;
			//int attributeLocation(const std::string & name)const;
			//void bindAttributeLocation(const char * name, int location);
			//void bindAttributeLocation(const std::string & name, int location);
			//void setAttributeValue(int location, GLfloat value);
			//void enableAttributeArray(int location);
			//void setUniformValue(int location, GLfloat value);
			//void setUniformValue(int location, const ysl::Vector3f & value);
			

		private:
			unsigned int m_program;	// 0 for vertex shader, 1 for fragment shader
			bool m_created;
			std::vector<unsigned int> m_shaders;
			ShaderType m_shaderType;
	};
}




#endif