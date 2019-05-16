
#include "uniform.h"
namespace ysl
{
	namespace graphics
	{
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
