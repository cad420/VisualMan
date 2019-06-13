
#ifndef _LARGEVOLUME_H_
#define _LARGEVOLUME_H_
#include "config.h"
#include <primitive.h>
#include <memory>

namespace ysl
{
	namespace vm
	{
		class OutOfCorePrimitive;

		struct VMAPPS_EXPORT_IMPORT IOutOfCoreCallback:std::enable_shared_from_this<IOutOfCoreCallback>
		{
			virtual void OnDrawCallStart(OutOfCorePrimitive*p) = 0;
			virtual void OnDrawCallFinished(OutOfCorePrimitive*p) = 0;
			virtual ~IOutOfCoreCallback() = default;
		};

		class VMAPPS_EXPORT_IMPORT OutOfCorePrimitive:public Primitive
		{
		public:
			void Render(const Actor* actor, const Shading* shading, const Camera* camera, RenderContext* context) override;
			void AddOutOfCoreEvent(Ref<IOutOfCoreCallback> e);
			void RemoveOutOfCoreEvent(Ref<IOutOfCoreCallback> e);
			void RemoveOutOfCoreEvent();
			void SetRenderFinished(bool finished) { renderFinished = finished; }
		private:
			
			std::set<Ref<IOutOfCoreCallback>> callbacks;
			bool renderFinished = false;
		};
	}
}
#endif