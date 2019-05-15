
#include "abstrarenderstate.h"

namespace ysl
{
	namespace graphics
	{
		void RenderStateSet::SetRenderState(Ref<RenderState> state, int index)
		{
			for (auto & each : renderStates)
			{
				if (each->Type() == state->Type())
				{
					each = state;
					return;
				}
			}
			renderStates.push_back(std::move(state));
		}
		Ref<RenderState> RenderStateSet::GetRenderState(RenderStateType type)
		{
			for(const auto & each:renderStates)
				if (each->Type() == type)
					return each;
			return nullptr;
		}

	}
}