
#include "shading.h"
namespace ysl
{
	namespace vpl
	{
		Ref<TextureSampler> Shading::CreateGetTextureSampler(int unitIndex)
		{
			auto texSampler = std::static_pointer_cast<TextureSampler>(
				CreateGetRenderStateSet()->
				GetRenderState(RS_TextureSampler, unitIndex)
				);
			if(texSampler == nullptr)
			{
				texSampler = MakeRef<TextureSampler>();
				CreateGetRenderStateSet()->SetRenderState(texSampler, unitIndex);
			}
			return texSampler;
		}
	}
}