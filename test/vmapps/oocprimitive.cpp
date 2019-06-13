
#include "oocprimitive.h"
#include <rendercontext.h>


namespace ysl
{
	namespace vm
	{

		void OutOfCorePrimitive::Render(const Actor* actor, const Shading* shading, const Camera* camera,
			RenderContext* context)
		{
			// Check renderable whether is dirty
			if (GetBufferObjectEnabled() && GetBufferObjectDirty())
			{
				UpdateDirtyBufferObject(VM_UM_KeepRAM);
				SetBufferObjectDirty(false);
			}

			renderFinished = false;
			do
			{
				for (auto &each : callbacks)
				{
					each->OnDrawCallStart(this);
					if (renderFinished)
						break;
				}
				if (renderFinished) break;
				Render_Implement(actor, shading, camera, context);
				for(auto &each:callbacks)
				{
					each->OnDrawCallFinished(this);
					if (renderFinished)break;
				}

			} while (!renderFinished);
		}

		void OutOfCorePrimitive::AddOutOfCoreEvent(Ref<IOutOfCoreCallback> e)
		{
			callbacks.insert(e);
		}

		void OutOfCorePrimitive::RemoveOutOfCoreEvent(Ref<IOutOfCoreCallback> e)
		{
			callbacks.erase(e);
		}

		void OutOfCorePrimitive::RemoveOutOfCoreEvent()
		{
			std::set<Ref<IOutOfCoreCallback>>().swap(callbacks);
		}
	}
}