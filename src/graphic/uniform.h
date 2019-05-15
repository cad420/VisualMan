

#ifndef _UNIFORM_H_
#define _UNIFORM_H_
#include <string>
#include <vector>
#include "../../lib/gl3w/GL/glcorearb.h"
#include "../core/spectrum.h"
#include "../mathematics/geometry.h"
#include "../mathematics/transformation.h"
#include "graphictype.h"

namespace ysl {
	struct Matrix3x3;
	struct Matrix4x4;
}

namespace ysl
{
	namespace graphics
	{

		enum Unifortype
		{
			UT_NONE = 0x0,

			UT_FLOAT = GL_FLOAT, //!< float
			UT_FLOAT_VEC2 = GL_FLOAT_VEC2, //!< vec2
			UT_FLOAT_VEC3 = GL_FLOAT_VEC3, //!< vec3
			UT_FLOAT_VEC4 = GL_FLOAT_VEC4, //!< vec4
			UT_DOUBLE = GL_DOUBLE, //!< double
			UT_DOUBLE_VEC2 = GL_DOUBLE_VEC2, //!< dvec2
			UT_DOUBLE_VEC3 = GL_DOUBLE_VEC3, //!< dvec3
			UT_DOUBLE_VEC4 = GL_DOUBLE_VEC4, //!< dvec4
			UT_INT = GL_INT, //!< int
			UT_INT_VEC2 = GL_INT_VEC2, //!< ivec2
			UT_INT_VEC3 = GL_INT_VEC3, //!< ivec3
			UT_INT_VEC4 = GL_INT_VEC4, //!< ivec4
			UT_UNSIGNED_INT = GL_UNSIGNED_INT, //!< unsigned int
			UT_UNSIGNED_INT_VEC2 = GL_UNSIGNED_INT_VEC2, //!< uvec2
			UT_UNSIGNED_INT_VEC3 = GL_UNSIGNED_INT_VEC3, //!< uvec3
			UT_UNSIGNED_INT_VEC4 = GL_UNSIGNED_INT_VEC4, //!< uvec4
			UT_BOOL = GL_BOOL, //!< bool
			UT_BOOL_VEC2 = GL_BOOL_VEC2, //!< bvec2
			UT_BOOL_VEC3 = GL_BOOL_VEC3, //!< bvec3
			UT_BOOL_VEC4 = GL_BOOL_VEC4, //!< bvec4
			UT_FLOAT_MAT2 = GL_FLOAT_MAT2, //!< mat2
			UT_FLOAT_MAT3 = GL_FLOAT_MAT3, //!< mat3
			UT_FLOAT_MAT4 = GL_FLOAT_MAT4, //!< mat4
			UT_FLOAT_MAT2x3 = GL_FLOAT_MAT2x3, //!< mat2x3
			UT_FLOAT_MAT2x4 = GL_FLOAT_MAT2x4, //!< mat2x4
			UT_FLOAT_MAT3x2 = GL_FLOAT_MAT3x2, //!< mat3x2
			UT_FLOAT_MAT3x4 = GL_FLOAT_MAT3x4, //!< mat3x4
			UT_FLOAT_MAT4x2 = GL_FLOAT_MAT4x2, //!< mat4x2
			UT_FLOAT_MAT4x3 = GL_FLOAT_MAT4x3, //!< mat4x3
			UT_DOUBLE_MAT2 = GL_DOUBLE_MAT2, //!< dmat2
			UT_DOUBLE_MAT3 = GL_DOUBLE_MAT3, //!< dmat3
			UT_DOUBLE_MAT4 = GL_DOUBLE_MAT4, //!< dmat4
			UT_DOUBLE_MAT2x3 = GL_DOUBLE_MAT2x3, //!< dmat2x3
			UT_DOUBLE_MAT2x4 = GL_DOUBLE_MAT2x4, //!< dmat2x4
			UT_DOUBLE_MAT3x2 = GL_DOUBLE_MAT3x2, //!< dmat3x2
			UT_DOUBLE_MAT3x4 = GL_DOUBLE_MAT3x4, //!< dmat3x4
			UT_DOUBLE_MAT4x2 = GL_DOUBLE_MAT4x2, //!< dmat4x2
			UT_DOUBLE_MAT4x3 = GL_DOUBLE_MAT4x3, //!< dmat4x3

			UT_SAMPLER_1D = GL_SAMPLER_1D, //!< sampler1D
			UT_SAMPLER_2D = GL_SAMPLER_2D, //!< sampler2D
			UT_SAMPLER_3D = GL_SAMPLER_3D, //!< sampler3D
			UT_SAMPLER_CUBE = GL_SAMPLER_CUBE, //!< samplerCube
			UT_SAMPLER_1D_SHADOW = GL_SAMPLER_1D_SHADOW, //!< sampler1DShadow
			UT_SAMPLER_2D_SHADOW = GL_SAMPLER_2D_SHADOW, //!< sampler2DShadow
			UT_SAMPLER_1D_ARRAY = GL_SAMPLER_1D_ARRAY, //!< sampler1DArray
			UT_SAMPLER_2D_ARRAY = GL_SAMPLER_2D_ARRAY, //!< sampler2DArray
			UT_SAMPLER_1D_ARRAY_SHADOW = GL_SAMPLER_1D_ARRAY_SHADOW, //!< sampler1DArrayShadow
			UT_SAMPLER_2D_ARRAY_SHADOW = GL_SAMPLER_2D_ARRAY_SHADOW, //!< sampler2DArrayShadow
			UT_SAMPLER_2D_MULTISAMPLE = GL_SAMPLER_2D_MULTISAMPLE, //!< sampler2DMS
			UT_SAMPLER_2D_MULTISAMPLE_ARRAY = GL_SAMPLER_2D_MULTISAMPLE_ARRAY, //!< sampler2DMSArray
			UT_SAMPLER_CUBE_SHADOW = GL_SAMPLER_CUBE_SHADOW, //!< samplerCubeShadow
			UT_SAMPLER_BUFFER = GL_SAMPLER_BUFFER, //!< samplerBuffer
			UT_SAMPLER_2D_RECT = GL_SAMPLER_2D_RECT, //!< sampler2DRect
			UT_SAMPLER_2D_RECT_SHADOW = GL_SAMPLER_2D_RECT_SHADOW, //!< sampler2DRectShadow
			UT_INT_SAMPLER_1D = GL_INT_SAMPLER_1D, //!< isampler1D
			UT_INT_SAMPLER_2D = GL_INT_SAMPLER_2D, //!< isampler2D
			UT_INT_SAMPLER_3D = GL_INT_SAMPLER_3D, //!< isampler3D
			UT_INT_SAMPLER_CUBE = GL_INT_SAMPLER_CUBE, //!< isamplerCube
			UT_INT_SAMPLER_1D_ARRAY = GL_INT_SAMPLER_1D_ARRAY, //!< isampler1DArray
			UT_INT_SAMPLER_2D_ARRAY = GL_INT_SAMPLER_2D_ARRAY, //!< isampler2DArray
			UT_INT_SAMPLER_2D_MULTISAMPLE = GL_INT_SAMPLER_2D_MULTISAMPLE, //!< isampler2DMS
			UT_INT_SAMPLER_2D_MULTISAMPLE_ARRAY = GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, //!< isampler2DMSArray
			UT_INT_SAMPLER_BUFFER = GL_INT_SAMPLER_BUFFER, //!< isamplerBuffer
			UT_INT_SAMPLER_2D_RECT = GL_INT_SAMPLER_2D_RECT, //!< isampler2DRect
			UT_UNSIGNED_INT_SAMPLER_1D = GL_UNSIGNED_INT_SAMPLER_1D, //!< usampler1D
			UT_UNSIGNED_INT_SAMPLER_2D = GL_UNSIGNED_INT_SAMPLER_2D, //!< usampler2D
			UT_UNSIGNED_INT_SAMPLER_3D = GL_UNSIGNED_INT_SAMPLER_3D, //!< usampler3D
			UT_UNSIGNED_INT_SAMPLER_CUBE = GL_UNSIGNED_INT_SAMPLER_CUBE, //!< usamplerCube
			UT_UNSIGNED_INT_SAMPLER_1D_ARRAY = GL_UNSIGNED_INT_SAMPLER_1D_ARRAY, //!< usampler2DArray
			UT_UNSIGNED_INT_SAMPLER_2D_ARRAY = GL_UNSIGNED_INT_SAMPLER_2D_ARRAY, //!< usampler2DArray
			UT_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE = GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE, //!< usampler2DMS
			UT_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY = GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, //!< usampler2DMSArray
			UT_UNSIGNED_INT_SAMPLER_BUFFER = GL_UNSIGNED_INT_SAMPLER_BUFFER, //!< usamplerBuffer
			UT_UNSIGNED_INT_SAMPLER_2D_RECT = GL_UNSIGNED_INT_SAMPLER_2D_RECT, //!< usampler2DRect

			UT_UnifortypeCount
		};


		class GRAPHICS_EXPORT_IMPORT Uniform
		{
		public:
			Uniform() :type(UT_NONE) {}
			Uniform(const std::string & name) :type(UT_NONE), name(name) {}
			std::string GetName()const { return name; }
			void SetName(const std::string & name) { this->name = name; }
			Unifortype Type()const { return type; }

			// massive variable setters
			void setUniform1i(int count, const int* value) { resizeBytes(count * 1 * sizeof(int)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_INT; }
			void setUniform2i(int count, const int* value) { resizeBytes(count * 2 * sizeof(int)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_INT_VEC2; }
			void setUniform3i(int count, const int* value) { resizeBytes(count * 3 * sizeof(int)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_INT_VEC3; }
			void setUniform4i(int count, const int* value) { resizeBytes(count * 4 * sizeof(int)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_INT_VEC4; }

			void setUniform1ui(int count, const unsigned int* value) { resizeBytes(count * 1 * sizeof(unsigned int)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_UNSIGNED_INT; }
			void setUniform2ui(int count, const unsigned int* value) { resizeBytes(count * 2 * sizeof(unsigned int)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_UNSIGNED_INT_VEC2; }
			void setUniform3ui(int count, const unsigned int* value) { resizeBytes(count * 3 * sizeof(unsigned int)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_UNSIGNED_INT_VEC3; }
			void setUniform4ui(int count, const unsigned int* value) { resizeBytes(count * 4 * sizeof(unsigned int)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_UNSIGNED_INT_VEC4; }

			void setUniform1f(int count, const float* value) { resizeBytes(count * 1 * sizeof(float)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_FLOAT; }
			void setUniform2f(int count, const float* value) { resizeBytes(count * 2 * sizeof(float)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_FLOAT_VEC2; }
			void setUniform3f(int count, const float* value) { resizeBytes(count * 3 * sizeof(float)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_FLOAT_VEC3; }
			void setUniform4f(int count, const float* value) { resizeBytes(count * 4 * sizeof(float)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_FLOAT_VEC4; }

			void setUniform1d(int count, const double* value) { resizeBytes(count * 1 * sizeof(double)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_DOUBLE; }
			void setUniform2d(int count, const double* value) { resizeBytes(count * 2 * sizeof(double)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_DOUBLE_VEC2; }
			void setUniform3d(int count, const double* value) { resizeBytes(count * 3 * sizeof(double)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_DOUBLE_VEC3; }
			void setUniform4d(int count, const double* value) { resizeBytes(count * 4 * sizeof(double)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_DOUBLE_VEC4; }

			void setUniformMatrix2f(int count, const float* value) { resizeBytes(count * 2 * 2 * sizeof(float)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_FLOAT_MAT2; }
			void setUniformMatrix3f(int count, const float* value) { resizeBytes(count * 3 * 3 * sizeof(float)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_FLOAT_MAT3; }
			void setUniformMatrix4f(int count, const float* value) { resizeBytes(count * 4 * 4 * sizeof(float)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_FLOAT_MAT4; }

			void setUniformMatrix2x3f(int count, const float* value) { resizeBytes(count * 2 * 3 * sizeof(float)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_FLOAT_MAT2x3; }
			void setUniformMatrix3x2f(int count, const float* value) { resizeBytes(count * 3 * 2 * sizeof(float)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_FLOAT_MAT3x2; }
			void setUniformMatrix2x4f(int count, const float* value) { resizeBytes(count * 2 * 4 * sizeof(float)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_FLOAT_MAT2x4; }
			void setUniformMatrix4x2f(int count, const float* value) { resizeBytes(count * 4 * 2 * sizeof(float)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_FLOAT_MAT4x2; }
			void setUniformMatrix3x4f(int count, const float* value) { resizeBytes(count * 3 * 4 * sizeof(float)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_FLOAT_MAT3x4; }
			void setUniformMatrix4x3f(int count, const float* value) { resizeBytes(count * 4 * 3 * sizeof(float)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_FLOAT_MAT4x3; }

			void setUniformMatrix2d(int count, const double* value) { resizeBytes(count * 2 * 2 * sizeof(double)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_DOUBLE_MAT2; }
			void setUniformMatrix3d(int count, const double* value) { resizeBytes(count * 3 * 3 * sizeof(double)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_DOUBLE_MAT3; }
			void setUniformMatrix4d(int count, const double* value) { resizeBytes(count * 4 * 4 * sizeof(double)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_DOUBLE_MAT4; }

			void setUniformMatrix2x3d(int count, const double* value) { resizeBytes(count * 2 * 3 * sizeof(double)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_DOUBLE_MAT2x3; }
			void setUniformMatrix3x2d(int count, const double* value) { resizeBytes(count * 3 * 2 * sizeof(double)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_DOUBLE_MAT3x2; }
			void setUniformMatrix2x4d(int count, const double* value) { resizeBytes(count * 2 * 4 * sizeof(double)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_DOUBLE_MAT2x4; }
			void setUniformMatrix4x2d(int count, const double* value) { resizeBytes(count * 4 * 2 * sizeof(double)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_DOUBLE_MAT4x2; }
			void setUniformMatrix3x4d(int count, const double* value) { resizeBytes(count * 3 * 4 * sizeof(double)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_DOUBLE_MAT3x4; }
			void setUniformMatrix4x3d(int count, const double* value) { resizeBytes(count * 4 * 3 * sizeof(double)); memcpy(&data[0], value, sizeof(data[0]) * data.size()); type = UT_DOUBLE_MAT4x3; }

			void setUniformValue(const Matrix4x4 & value) { setUniformMatrix4f(1, value.Transposed().m[0]); }
			void setUniformValue(const Matrix3x3 & value) { setUniformMatrix3f(1, value.Transposed().m[0]); };
			void setUniformValue(const RGBSpectrum & value) { setUniform3f(1, value.c); }
			void setUniformValue(const RGBASpectrum & value) { setUniform4f(1, value.c); }
			void setUniformValue(const Vector3f & value) { setUniform3f(1, value.ConstData()); }
			void setUniformValue(const Point3f & value) { setUniform3f(1, value.ConstData()); }
			void setUniformValue(const Vector3i & value) { setUniform3i(1, value.ConstData()); }
			void setUniformValue(const Vector2f & value) { setUniform2f(1, value.ConstData()); }
			void setUniformValue(const Vector2i & value) { setUniform2i(1, value.ConstData()); }
			void setUniformValue(bool value) { int v = value; setUniform1i(1, &v); }
			void setUniformValue(int value) { setUniform1i(1, &value); };
			void setUniformValue(float value) { setUniform1f(1, &value); }
		private:
			void resizeBytes(int bytes) { data.resize(bytes); }
			Unifortype type;
			std::string name;
			std::vector<uint8_t> data;
		};


		class GRAPHICS_EXPORT_IMPORT UniformSet
		{
		public:
			UniformSet() = default;
			void SetUniform(Ref<Uniform> uniform);
			Ref<Uniform> GetUniform(const char * name);
			void RemoveUniform(Ref<Uniform> uniform);
			void RemoveUniform(const char * name);
			void RemoveAllUniforms();
			const auto & Uniforms()const { return uniforms; }
			auto & Uniforms() { return uniforms; }
		private:
			std::vector<Ref<Uniform>> uniforms;
		};
	}
}
#endif