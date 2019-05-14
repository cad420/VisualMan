#ifndef _RENDERSTATE_H_
#define _RENDERSTATE_H_
#include "abstrarenderstate.h"


namespace ysl
{
	namespace gpu
	{

		class GRAPHICS_EXPORT_IMPORT DepthFuncState:public RenderStateNonIndexed
		{
		public:
			DepthFuncState(Func type) :RenderStateNonIndexed(RS_DepthFunc){}
			void Apply(int index, Ref<Camera> camera, RenderContext * context)const override;
			void Set(Func depthFunc) { func = depthFunc; }
			Func DepthFunc()const { return func; }

		private:
			Func func;
		};

	}
}

#endif