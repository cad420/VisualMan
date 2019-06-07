
#include "screenactor.h"
#include "abstraarray.h"
#include "drawarray.h"

namespace ysl
{
	namespace vm
	{
		ScreenActorEventCallback::ScreenActorEventCallback()
		{
			// Init a screen primitive

			auto vertexNDC = MakeRef<ArrayFloat2>();
			auto texCoord = MakeRef<ArrayFloat2>();

			float v[] =
			{
				-1.f,1.0f,
				-1.0f,-1.0f,
				1.0f,-1.0f,

				-1.0f,1.0f,
				1.0f,-1.0f,
				1.0f,1.0f
			};

			float t[] =
			{
				0.0f,1.0f,
				0.0f,0.0f,
				1.0f,0.0f,

				0.0f,1.0f,
				1.0f,0.0f,
				1.0f,1.0f
			};

			vertexNDC->GetBufferObject()->SetLocalData(v,sizeof(v));
			texCoord->GetBufferObject()->SetLocalData(t, sizeof(t));

			screenRect = MakeRef<Primitive>();
			screenRect->SetVertexPositionArray(vertexNDC);
			screenRect->SetVertexTexCoordArray(texCoord);

			//draw call
			screenRect->DrawCalls().push_back(MakeRef<DrawArray>(0, 6));

		}

		void ScreenActorEventCallback::OnActorRenderStartedEvent(Actor*, const Camera*,
			Renderable* , const Shading*  ,int )
		{

		}

		void ScreenActorEventCallback::BindToActor(Ref<Actor> actor)
		{
			const auto sharedThis = std::static_pointer_cast<ScreenActorEventCallback>(shared_from_this());
			actor->RemoveActorRenderEventCallback(sharedThis);
			actor->AddActorRenderEventCallback(sharedThis);
			actor->SetRenderable(screenRect, 0);
		}
	}
}