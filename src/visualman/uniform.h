

#ifndef _UNIFORM_H_
#define _UNIFORM_H_
#include <string>
#include <vector>
#include <GL/glcorearb.h>
//#include "../core/spectrum.h"
#include "graphictype.h"
#include <geometry.h>
#include <transformation.h>

namespace ysl {
	struct Matrix3x3;
	struct Matrix4x4;
}

namespace ysl
{
	namespace vm
	{

		enum UniformType
		{
			UT_NONE = 0x0,

			UT_FLOAT = GL_FLOAT,
			UT_FLOAT_VEC2 = GL_FLOAT_VEC2,
			UT_FLOAT_VEC3 = GL_FLOAT_VEC3,
			UT_FLOAT_VEC4 = GL_FLOAT_VEC4,
			UT_DOUBLE = GL_DOUBLE,
			UT_DOUBLE_VEC2 = GL_DOUBLE_VEC2,
			UT_DOUBLE_VEC3 = GL_DOUBLE_VEC3,
			UT_DOUBLE_VEC4 = GL_DOUBLE_VEC4,
			UT_INT = GL_INT,
			UT_INT_VEC2 = GL_INT_VEC2,
			UT_INT_VEC3 = GL_INT_VEC3,
			UT_INT_VEC4 = GL_INT_VEC4,
			UT_UNSIGNED_INT = GL_UNSIGNED_INT,
			UT_UNSIGNED_INT_VEC2 = GL_UNSIGNED_INT_VEC2,
			UT_UNSIGNED_INT_VEC3 = GL_UNSIGNED_INT_VEC3,
			UT_UNSIGNED_INT_VEC4 = GL_UNSIGNED_INT_VEC4,
			UT_BOOL = GL_BOOL,
			UT_BOOL_VEC2 = GL_BOOL_VEC2,
			UT_BOOL_VEC3 = GL_BOOL_VEC3,
			UT_BOOL_VEC4 = GL_BOOL_VEC4,
			UT_FLOAT_MAT2 = GL_FLOAT_MAT2,
			UT_FLOAT_MAT3 = GL_FLOAT_MAT3,
			UT_FLOAT_MAT4 = GL_FLOAT_MAT4,
			UT_FLOAT_MAT2x3 = GL_FLOAT_MAT2x3,
			UT_FLOAT_MAT2x4 = GL_FLOAT_MAT2x4,
			UT_FLOAT_MAT3x2 = GL_FLOAT_MAT3x2,
			UT_FLOAT_MAT3x4 = GL_FLOAT_MAT3x4,
			UT_FLOAT_MAT4x2 = GL_FLOAT_MAT4x2,
			UT_FLOAT_MAT4x3 = GL_FLOAT_MAT4x3,
			UT_DOUBLE_MAT2 = GL_DOUBLE_MAT2,
			UT_DOUBLE_MAT3 = GL_DOUBLE_MAT3,
			UT_DOUBLE_MAT4 = GL_DOUBLE_MAT4,
			UT_DOUBLE_MAT2x3 = GL_DOUBLE_MAT2x3,
			UT_DOUBLE_MAT2x4 = GL_DOUBLE_MAT2x4,
			UT_DOUBLE_MAT3x2 = GL_DOUBLE_MAT3x2,
			UT_DOUBLE_MAT3x4 = GL_DOUBLE_MAT3x4,
			UT_DOUBLE_MAT4x2 = GL_DOUBLE_MAT4x2,
			UT_DOUBLE_MAT4x3 = GL_DOUBLE_MAT4x3,

			UT_SAMPLER_1D = GL_SAMPLER_1D,
			UT_SAMPLER_2D = GL_SAMPLER_2D,
			UT_SAMPLER_3D = GL_SAMPLER_3D,
			UT_SAMPLER_CUBE = GL_SAMPLER_CUBE,
			UT_SAMPLER_1D_SHADOW = GL_SAMPLER_1D_SHADOW,
			UT_SAMPLER_2D_SHADOW = GL_SAMPLER_2D_SHADOW,
			UT_SAMPLER_1D_ARRAY = GL_SAMPLER_1D_ARRAY,
			UT_SAMPLER_2D_ARRAY = GL_SAMPLER_2D_ARRAY,
			UT_SAMPLER_1D_ARRAY_SHADOW = GL_SAMPLER_1D_ARRAY_SHADOW,
			UT_SAMPLER_2D_ARRAY_SHADOW = GL_SAMPLER_2D_ARRAY_SHADOW,
			UT_SAMPLER_2D_MULTISAMPLE = GL_SAMPLER_2D_MULTISAMPLE,
			UT_SAMPLER_2D_MULTISAMPLE_ARRAY = GL_SAMPLER_2D_MULTISAMPLE_ARRAY,
			UT_SAMPLER_CUBE_SHADOW = GL_SAMPLER_CUBE_SHADOW,
			UT_SAMPLER_BUFFER = GL_SAMPLER_BUFFER,
			UT_SAMPLER_2D_RECT = GL_SAMPLER_2D_RECT,
			UT_SAMPLER_2D_RECT_SHADOW = GL_SAMPLER_2D_RECT_SHADOW,
			UT_INT_SAMPLER_1D = GL_INT_SAMPLER_1D,
			UT_INT_SAMPLER_2D = GL_INT_SAMPLER_2D,
			UT_INT_SAMPLER_3D = GL_INT_SAMPLER_3D,
			UT_INT_SAMPLER_CUBE = GL_INT_SAMPLER_CUBE,
			UT_INT_SAMPLER_1D_ARRAY = GL_INT_SAMPLER_1D_ARRAY,
			UT_INT_SAMPLER_2D_ARRAY = GL_INT_SAMPLER_2D_ARRAY,
			UT_INT_SAMPLER_2D_MULTISAMPLE = GL_INT_SAMPLER_2D_MULTISAMPLE,
			UT_INT_SAMPLER_2D_MULTISAMPLE_ARRAY = GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
			UT_INT_SAMPLER_BUFFER = GL_INT_SAMPLER_BUFFER,
			UT_INT_SAMPLER_2D_RECT = GL_INT_SAMPLER_2D_RECT,
			UT_UNSIGNED_INT_SAMPLER_1D = GL_UNSIGNED_INT_SAMPLER_1D,
			UT_UNSIGNED_INT_SAMPLER_2D = GL_UNSIGNED_INT_SAMPLER_2D,
			UT_UNSIGNED_INT_SAMPLER_3D = GL_UNSIGNED_INT_SAMPLER_3D,
			UT_UNSIGNED_INT_SAMPLER_CUBE = GL_UNSIGNED_INT_SAMPLER_CUBE,
			UT_UNSIGNED_INT_SAMPLER_1D_ARRAY = GL_UNSIGNED_INT_SAMPLER_1D_ARRAY,
			UT_UNSIGNED_INT_SAMPLER_2D_ARRAY = GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,
			UT_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE = GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE,
			UT_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY = GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
			UT_UNSIGNED_INT_SAMPLER_BUFFER = GL_UNSIGNED_INT_SAMPLER_BUFFER,
			UT_UNSIGNED_INT_SAMPLER_2D_RECT = GL_UNSIGNED_INT_SAMPLER_2D_RECT,

			UT_UnifortypeCount
		};


		class GRAPHICS_EXPORT_IMPORT Uniform
		{
		public:
			Uniform() :type(UT_NONE) {}
			Uniform(const std::string & name) :type(UT_NONE), name(name) {}

			std::string GetName()const { return name; }
			
			void SetName(const std::string & name) { this->name = name; }

			UniformType Type()const { return type; }


			template<typename Ty>
			const Ty * Data()const { return reinterpret_cast<const Ty*>(data.data()); }

			// massive variable setters

			void SetUniform1i(int count, const int* value);

			void SetUniform2i(int count, const int* value);

			void SetUniform3i(int count, const int* value);

			void SetUniform4i(int count, const int* value);

			void SetUniform1ui(int count, const unsigned int* value);

			void SetUniform2ui(int count, const unsigned int* value);

			void SetUniform3ui(int count, const unsigned int* value);

			void SetUniform4ui(int count, const unsigned int* value);

			void SetUniform1f(int count, const float* value);

			void SetUniform2f(int count, const float* value);

			void SetUniform3f(int count, const float* value);

			void SetUniform4f(int count, const float* value);

			void SetUniform1d(int count, const double* value);

			void SetUniform2d(int count, const double* value);

			void SetUniform3d(int count, const double* value);

			void SetUniform4d(int count, const double* value);

			void SetUniformMatrix2f(int count, const float* value);

			void SetUniformMatrix3f(int count, const float* value);

			void SetUniformMatrix4f(int count, const float* value);

			void SetUniformMatrix2x3f(int count, const float* value);

			void SetUniformMatrix3x2f(int count, const float* value);

			void SetUniformMatrix2x4f(int count, const float* value);

			void SetUniformMatrix4x2f(int count, const float* value);

			void SetUniformMatrix3x4f(int count, const float* value);

			void SetUniformMatrix4x3f(int count, const float* value);

			void SetUniformMatrix2d(int count, const double* value);

			void SetUniformMatrix3d(int count, const double* value);

			void SetUniformMatrix4d(int count, const double* value);

			void SetUniformMatrix2x3d(int count, const double* value);

			void SetUniformMatrix3x2d(int count, const double* value);

			void SetUniformMatrix2x4d(int count, const double* value);

			void SetUniformMatrix4x2d(int count, const double* value);

			void SetUniformMatrix3x4d(int count, const double* value);

			void SetUniformMatrix4x3d(int count, const double* value);

			void SetUniformValue(const Matrix4x4 & value) { SetUniformMatrix4f(1, value.Transposed().m[0]); }

			void SetUniformValue(const Matrix3x3 & value) { SetUniformMatrix3f(1, value.Transposed().m[0]); }

			//void SetUniformValue(const RGBSpectrum & value) { SetUniform3f(1, value.c); }

			//void SetUniformValue(const RGBASpectrum & value) { SetUniform4f(1, value.c); }

			void SetUniformValue(const Vector3f & value) { SetUniform3f(1, value.ConstData()); }

			void SetUniformValue(const Point3f & value) { SetUniform3f(1, value.ConstData()); }

			void SetUniformValue(const Vector3i & value) { SetUniform3i(1, value.ConstData()); }

			void SetUniformValue(const Vector2f & value) { SetUniform2f(1, value.ConstData()); }

			void SetUniformValue(const Vector2i & value) { SetUniform2i(1, value.ConstData()); }

			void SetUniformValue(bool value) { int v = value; SetUniform1i(1, &v); }

			void SetUniformValue(int value) { SetUniform1i(1, &value); }

			void SetUniformValue(float value) { SetUniform1f(1, &value); }


			int Count() const;

		private:
			void ResizeBytes(int bytes) { data.resize(bytes); }
			UniformType type;
			int cnt = 0;
			std::string name;
			std::vector<uint8_t> data;
		};


		class GRAPHICS_EXPORT_IMPORT UniformSet
		{
		public:
			UniformSet() = default;
			void SetUniform(Ref<Uniform> uniform);

			Ref<Uniform> GetUniform(const char * name);
			Ref<Uniform> CreateGetUniform(const char * name)
			{
				auto unf = GetUniform(name);
				if (unf) return unf;
				unf = MakeRef<Uniform>(name);
				SetUniform(unf);
				return unf;
			}
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