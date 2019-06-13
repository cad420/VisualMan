
#include "oocresources.h"
namespace ysl
{
	namespace vm
	{
		OutOfCoreResources::OutOfCoreResources(const std::string & fileName)
		{
			cpuMemoryData = std::make_unique<CPUVolumeDataCache>(fileName);
		}

		void OutOfCoreResources::OnDrawCallFinished(OutOfCorePrimitive* p)
		{
			p->SetRenderFinished(true);
		}

		void OutOfCoreResources::OnDrawCallStart(OutOfCorePrimitive* p)
		{

		}

		void OutOfCoreResources::BindPrimitive(Ref<OutOfCorePrimitive> primitive)
		{
			primitive->RemoveOutOfCoreEvent(shared_from_this());
			primitive->AddOutOfCoreEvent(shared_from_this());
		}
	}
}