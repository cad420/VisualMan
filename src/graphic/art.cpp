
#include "art.h"
namespace ysl
{
	namespace graphics
	{
		Ref<Shading> Artist::GetShader(int lod, int pass)
		{
			auto ptr = CreateGetLOD(lod);
			if (ptr)
			{
				if (pass >= 0 && pass < ptr->size())
					return (*ptr)[pass];
			}
			return nullptr;
		}

		Ref<ShadingPasses>& Artist::CreateGetLOD(int lod)
		{
			if (lod >= 0 && lod < MaxLodLevel)
				return LodShadingPasses[lod] ? LodShadingPasses[lod] : LodShadingPasses[lod] = MakeRef<ShadingPasses>();
		}
	}
}