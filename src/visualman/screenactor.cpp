
#include "screenactor.h"
#include "abstraarray.h"
#include "drawarray.h"
#include "shaderprogram.h"
#include "camera.h"
#include "actor.h"

namespace ysl
{
	namespace vm
	{
		ScreenActorEventCallback::ScreenActorEventCallback()
		{
			// Init a screen primitive
			SetPrimitive(MakeRef<Primitive>());
		}

		void ScreenActorEventCallback::OnActorRenderStartedEvent(Actor*actor, const Camera*camera,
			Renderable* , const Shading*  shading,int )
		{
			//const auto program = shading->GetProgram().get();

			//const auto half_way_position = program->GetGenericUniformLocation("halfway");
			//if(half_way_position != -1)
			//{
			//	actor->CreateGetUniformSet()->CreateGetUniform("halfway")->SetUniform3f(1, camera->Front().Data());
			//}
		}

		void ScreenActorEventCallback::SetPrimitive(Ref<Primitive> primitive)
		{
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

			vertexNDC->GetBufferObject()->SetLocalData(v, sizeof(v));
			texCoord->GetBufferObject()->SetLocalData(t, sizeof(t));
			primitive->SetVertexPositionArray(vertexNDC);
			primitive->SetVertexTexCoordArray(texCoord);
			//draw call
			primitive->DrawCalls().push_back(MakeRef<DrawArray>(0, 6));
			screenRect = primitive;
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
