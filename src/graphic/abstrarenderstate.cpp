
#include "abstrarenderstate.h"

namespace ysl
{
	namespace graphics
	{
		void RenderStateSet::SetRenderState(Ref<RenderState> state)
		{
			for (auto & each : renderStates)
				if (each->Type() == state->Type())
					each = state;
		}

		void RenderStateSet::AddRenderState(Ref<RenderState> state)
		{
			for(const auto & each:renderStates)
				if (each->Type() == state->Type())
					return;
			renderStates.push_back(std::move(state));
		}
	}
}