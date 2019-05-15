
#include "uniform.h"
namespace ysl
{
	namespace graphics
	{
		void UniformSet::AddUniform(Ref<Uniform> uniform)
		{
			bool exist = false;
			for (auto & u : uniforms)
				if (u->GetName() == uniform->GetName())
					exist = true;
			if (!exist)
				uniforms.push_back(uniform);
		}

		void UniformSet::SetUniform(Ref<Uniform> uniform)
		{
			for (auto & u : uniforms)
				if (u->GetName() == uniform->GetName())
					u = uniform;
		}

		void UniformSet::RemoveUniform(Ref<Uniform> uniform)
		{
			for(auto it = uniforms.begin();it != uniforms.end();++it)
			{
				if ((*it)->GetName() == uniform->GetName())
					uniforms.erase(it);
			}
		}

		void UniformSet::RemoveUniform(const std::string& name)
		{
			for (auto it = uniforms.begin(); it != uniforms.end(); ++it)
			{
				if ((*it)->GetName() == name)
					uniforms.erase(it);
			}
		}

		void UniformSet::RemoveAllUniforms()
		{
			uniforms.swap(std::vector<Ref<Uniform>>());
		}
	}
}
