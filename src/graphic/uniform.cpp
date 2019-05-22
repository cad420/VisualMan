
#include "uniform.h"
namespace ysl
{
	namespace vm
	{
		void Uniform::SetUniform1i(int count, const int* value)
		{
			ResizeBytes(count * 1 * sizeof(int));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_INT;
			cnt = count;
		}

		void Uniform::SetUniform2i(int count, const int* value)
		{
			ResizeBytes(count * 2 * sizeof(int));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_INT_VEC2;
			cnt = count;
		}

		void Uniform::SetUniform3i(int count, const int* value)
		{
			ResizeBytes(count * 3 * sizeof(int));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_INT_VEC3;
			cnt = count;
		}

		void Uniform::SetUniform4i(int count, const int* value)
		{
			ResizeBytes(count * 4 * sizeof(int));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_INT_VEC4;
			cnt = count;
		}

		void Uniform::SetUniform1ui(int count, const unsigned* value)
		{
			ResizeBytes(count * 1 * sizeof(unsigned int));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_UNSIGNED_INT;
			cnt = count;
		}

		void Uniform::SetUniform2ui(int count, const unsigned* value)
		{
			ResizeBytes(count * 2 * sizeof(unsigned int));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_UNSIGNED_INT_VEC2;
			cnt = count;
		}

		void Uniform::SetUniform3ui(int count, const unsigned* value)
		{
			ResizeBytes(count * 3 * sizeof(unsigned int));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_UNSIGNED_INT_VEC3;
			cnt = count;
		}

		void Uniform::SetUniform4ui(int count, const unsigned* value)
		{
			ResizeBytes(count * 4 * sizeof(unsigned int));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_UNSIGNED_INT_VEC4;
			cnt = count;
		}

		void Uniform::SetUniform1f(int count, const float* value)
		{
			ResizeBytes(count * 1 * sizeof(float));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_FLOAT;
			cnt = count;
		}

		void Uniform::SetUniform2f(int count, const float* value)
		{
			ResizeBytes(count * 2 * sizeof(float));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_FLOAT_VEC2;
			cnt = count;
		}

		void Uniform::SetUniform3f(int count, const float* value)
		{
			ResizeBytes(count * 3 * sizeof(float));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_FLOAT_VEC3;
			cnt = count;
		}

		void Uniform::SetUniform4f(int count, const float* value)
		{
			ResizeBytes(count * 4 * sizeof(float));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_FLOAT_VEC4;
			cnt = count;
		}

		void Uniform::SetUniform1d(int count, const double* value)
		{
			ResizeBytes(count * 1 * sizeof(double));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_DOUBLE;
			cnt = count;
		}

		void Uniform::SetUniform2d(int count, const double* value)
		{
			ResizeBytes(count * 2 * sizeof(double));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_DOUBLE_VEC2;
			cnt = count;
		}

		void Uniform::SetUniform3d(int count, const double* value)
		{
			ResizeBytes(count * 3 * sizeof(double));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_DOUBLE_VEC3;
			cnt = count;
		}

		void Uniform::SetUniform4d(int count, const double* value)
		{
			ResizeBytes(count * 4 * sizeof(double));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_DOUBLE_VEC4;
			cnt = count;
		}

		void Uniform::SetUniformMatrix2f(int count, const float* value)
		{
			ResizeBytes(count * 2 * 2 * sizeof(float));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_FLOAT_MAT2;
			cnt = count;
		}

		void Uniform::SetUniformMatrix3f(int count, const float* value)
		{
			ResizeBytes(count * 3 * 3 * sizeof(float));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_FLOAT_MAT3;
			cnt = count;
		}

		void Uniform::SetUniformMatrix4f(int count, const float* value)
		{
			ResizeBytes(count * 4 * 4 * sizeof(float));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_FLOAT_MAT4;
			cnt = count;
		}

		void Uniform::SetUniformMatrix2x3f(int count, const float* value)
		{
			ResizeBytes(count * 2 * 3 * sizeof(float));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_FLOAT_MAT2x3;
			cnt = count;
		}

		void Uniform::SetUniformMatrix3x2f(int count, const float* value)
		{
			ResizeBytes(count * 3 * 2 * sizeof(float));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_FLOAT_MAT3x2;
			cnt = count;
		}

		void Uniform::SetUniformMatrix2x4f(int count, const float* value)
		{
			ResizeBytes(count * 2 * 4 * sizeof(float));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_FLOAT_MAT2x4;
			cnt = count;
		}

		void Uniform::SetUniformMatrix4x2f(int count, const float* value)
		{
			ResizeBytes(count * 4 * 2 * sizeof(float));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_FLOAT_MAT4x2;
			cnt = count;
		}

		void Uniform::SetUniformMatrix3x4f(int count, const float* value)
		{
			ResizeBytes(count * 3 * 4 * sizeof(float));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_FLOAT_MAT3x4;
			cnt = count;
		}

		void Uniform::SetUniformMatrix4x3f(int count, const float* value)
		{
			ResizeBytes(count * 4 * 3 * sizeof(float));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_FLOAT_MAT4x3;
			cnt = count;
		}

		void Uniform::SetUniformMatrix2d(int count, const double* value)
		{
			ResizeBytes(count * 2 * 2 * sizeof(double));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_DOUBLE_MAT2;
			cnt = count;
		}

		void Uniform::SetUniformMatrix3d(int count, const double* value)
		{
			ResizeBytes(count * 3 * 3 * sizeof(double));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_DOUBLE_MAT3;
			cnt = count;
		}

		void Uniform::SetUniformMatrix4d(int count, const double* value)
		{
			ResizeBytes(count * 4 * 4 * sizeof(double));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_DOUBLE_MAT4;
			cnt = count;
		}

		void Uniform::SetUniformMatrix2x3d(int count, const double* value)
		{
			ResizeBytes(count * 2 * 3 * sizeof(double));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_DOUBLE_MAT2x3;
			cnt = count;
		}

		void Uniform::SetUniformMatrix3x2d(int count, const double* value)
		{
			ResizeBytes(count * 3 * 2 * sizeof(double));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_DOUBLE_MAT3x2;
			cnt = count;
		}

		void Uniform::SetUniformMatrix2x4d(int count, const double* value)
		{
			ResizeBytes(count * 2 * 4 * sizeof(double));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_DOUBLE_MAT2x4;
			cnt = count;
		}

		void Uniform::SetUniformMatrix4x2d(int count, const double* value)
		{
			ResizeBytes(count * 4 * 2 * sizeof(double));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_DOUBLE_MAT4x2;
			cnt = count;
		}

		void Uniform::SetUniformMatrix3x4d(int count, const double* value)
		{
			ResizeBytes(count * 3 * 4 * sizeof(double));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_DOUBLE_MAT3x4;
			cnt = count;
		}

		void Uniform::SetUniformMatrix4x3d(int count, const double* value)
		{
			ResizeBytes(count * 4 * 3 * sizeof(double));
			memcpy(&data[0], value, sizeof(data[0]) * data.size());
			type = UT_DOUBLE_MAT4x3;
			cnt = count;
		}

		int Uniform::Count() const
		{
			if (data.empty())
				return 0;
			return cnt;
		}

		void UniformSet::SetUniform(Ref<Uniform> uniform)
		{
			auto exist = false;
			for (auto & u : uniforms)
			{
				if (u->GetName() == uniform->GetName())
				{
					u = uniform;
					exist = true;
				}
			}
			if (!exist)
				uniforms.push_back(std::move(uniform));
		}

		Ref<Uniform> UniformSet::GetUniform(const char* name)
		{
			for(auto each:uniforms)
			{
				if (each->GetName() == name)
					return each;
			}
			return nullptr;
		}

		void UniformSet::RemoveUniform(Ref<Uniform> uniform)
		{
			for(auto it = uniforms.begin();it != uniforms.end();)
			{
				if ((*it)->GetName() == uniform->GetName())
				{
					it = uniforms.erase(it);
					break;
				}
				else
					++it;
			}
		}

		void UniformSet::RemoveUniform(const char * name)
		{
			for (auto it = uniforms.begin(); it != uniforms.end();)
			{
				if ((*it)->GetName() == name)
				{
					it = uniforms.erase(it);
					break;
				}
				else
					++it;
			}
		}

		void UniformSet::RemoveAllUniforms()
		{
			uniforms.swap(std::vector<Ref<Uniform>>());
		}
	}
}
