
#ifndef _ABSTRARENDERSTATE_H_
#define _ABSTRARENDERSTATE_H_
#include "graphictype.h"
#include "camera.h"
#include <vector>

namespace ysl
{
	namespace vm
	{
		class RenderContext;
		class GLSLProgram;
	

		class GRAPHICS_EXPORT_IMPORT RenderState
		{
		public:
			RenderState() = delete;
			RenderState(RenderStateType type) :type(type) {}
			virtual void Apply(int index, const Camera * camera, RenderContext * context)const = 0;
			RenderStateType Type()const { return type; }
			virtual ~RenderState() = default;
		private:
			RenderStateType type;
		};


		class GRAPHICS_EXPORT_IMPORT RenderStateNonIndexed :public RenderState
		{
		public:
			RenderStateNonIndexed(RenderStateType type) :RenderState(type) {}
		};

		class GRAPHICS_EXPORT_IMPORT RenderStateIndexed:public RenderState
		{
		public:
			RenderStateIndexed(RenderStateType type):RenderState(type){}
			//int Index()const { return index; }
			//void SetIndex(int index) { this->index = index; }
		private:
			//int index = -1;
		};

		class RenderStateBox
		{
		public:
			RenderStateBox() = default;
			RenderStateBox(Ref<RenderState> state,int i):rawRenderState(std::move(state)),index(i){}
			RenderStateType StateType()const
			{
				if (index < 0)
					return rawRenderState->Type();
				return RenderStateType(rawRenderState->Type() + index);
			}

			virtual void Apply(const Camera * camera,RenderContext * context)const
			{
				rawRenderState->Apply(index, camera, context);
			}
			Ref<RenderState> rawRenderState;
			int index = -1;
		};



	}
}

#endif