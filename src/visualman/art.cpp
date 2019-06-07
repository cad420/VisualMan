
#include "art.h"
namespace ysl
{
	namespace vm
	{
		int Artist::EvalLOD(const Actor* actor, const Camera* camera)
		{
			if (lodEvaluator)
			{
				return activeLOD = lodEvaluator->Eval(actor, camera);
			}
			return 0;
		}

		Ref<Shading> Artist::GetShader(int lod, int pass)
		{
			const auto ptr = GetLOD(lod);
			if (ptr)
			{
				pass = Clamp(pass, 0, ptr->size());
				return (*ptr)[pass];
			}
			return nullptr;
		}

		Ref<ShadingPasses> Artist::GetLOD(int lod)
		{
			lod = Clamp(lod, 0, MaxLodLevel);
			return LodShadingPasses[lod];
		}
	}
}