
#include "oocresources.h"
namespace ysl
{
	namespace vm
	{
		OutOfCoreResources::OutOfCoreResources(const std::string & fileName)
		{
			//cpuMemoryData = std::make_unique<CPUVolumeDataCache>(fileName);
		}

		void OutOfCoreResources::OnDrawCallFinished(OutOfCorePrimitive* p)
		{
			/*
			 * Capture missed block ids and transfer them from memory into texture
			 */

			p->SetRenderFinished(true);
		}

		void OutOfCoreResources::OnDrawCallStart(OutOfCorePrimitive* p)
		{

		}

		void OutOfCoreResources::BindPrimitive(Ref<OutOfCorePrimitive> primitive)
		{
			primitive->RemoveOutOfCoreResources(shared_from_this());
			primitive->SetOutOfCoreResources(shared_from_this());
		}
	}
}