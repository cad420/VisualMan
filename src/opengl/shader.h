#ifndef _SHADER_H_
#define _SHADER_H_

#include "../mathematics/transformation.h"
#include "../mathematics/geometry.h"
#include "../core/spectrum.h"

#include <vector>
#include "texture.h"


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
		int uniformLocation(const std::string & name)const;

		void setUniformValue(int location, bool value);
		void setUniformValue(int location, int value);
		void setUniformValue(int location, float value);
		void setUniformValue(int location, const ysl::Matrix4x4 & mat4x4);
		void setUniformValue(int location, const ysl::Matrix3x3 & mat3x3);
		void setUniformValue(int location, const ysl::RGBSpectrum & rgb);
		void setUniformValue(int location, const ysl::Vector3f & fvec3);
		void setUniformValue(int location, const ysl::Vector3i & ivec3);
		void setUniformValue(int location, const ysl::Vector2f & fvec2);
		void setUniformValue(int location, const ysl::Vector2i & ivec2);
		void setUniformSampler(int location, TextureUnit texUnit, TextureTarget target, unsigned int textureId);

		void setUniformValue(const char * name, bool value);
		void setUniformValue(const char * name, int value);
		void setUniformValue(const char * name, float value);
		void setUniformValue(const char * name, const ysl::Matrix4x4 & mat);
		void setUniformValue(const char * name, const ysl::Matrix3x3 & mat);
		void setUniformValue(const char * name, const ysl::RGBSpectrum & rbg);
		void setUniformValue(const char * name, const ysl::Vector3f & fvec3);
		void setUniformValue(const char * name, const ysl::Vector3i & ivec3);
		void setUniformValue(const char * name, const ysl::Vector2f & fvec2);
		void setUniformValue(const char * name, const ysl::Vector2i & ivec2);
		void setUniformSampler(const char * name, TextureUnit texUnit, TextureTarget target, unsigned int textureId);

		void setUniformValue(const std::string & name, bool value);
		void setUniformValue(const std::string & name, int value);
		void setUniformValue(const std::string & name, float value);
		void setUniformValue(const std::string & name, const ysl::Matrix4x4 & mat);
		void setUniformValue(const std::string & name, const ysl::Matrix3x3 & mat);
		void setUniformValue(const std::string & name, const ysl::RGBSpectrum & rbg);
		void setUniformValue(const std::string & name, const ysl::Vector3f & fvec3);
		void setUniformValue(const std::string & name, const ysl::Vector3i & ivec3);
		void setUniformValue(const std::string & name, const ysl::Vector2f & fvec2);
		void setUniformValue(const std::string & name, const ysl::Vector2i & ivec2);
		void setUniformSampler(const std::string & name, TextureUnit texUnit, TextureTarget target, unsigned int textureId);


	private:
		unsigned int m_program;	// 0 for vertex shader, 1 for fragment shader
		bool m_created;
		std::vector<unsigned int> m_shaders;
		ShaderType m_shaderType;
	};
}




#endif