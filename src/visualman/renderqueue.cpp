
#include "renderqueue.h"
namespace ysl
{
	namespace vm
	{
		void RenderQueue::Sort()
		{
			std::sort(nodes.begin(), nodes.end(),[](const RenderNode * n1,const RenderNode * n2)
			{
					return n1->actor->GetPriority() < n2->actor->GetPriority();
			});
		}
	}
}