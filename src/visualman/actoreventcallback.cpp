
#include "actoreventcallback.h"
#include "primitive.h"
#include "abstraarray.h"
#include "drawelements.h"
#include "shaderprogram.h"
#include <cassert>
#include <iostream>

namespace ysl
{
	namespace vm
	{
		RayCastActorEventCallback::RayCastActorEventCallback()
		{
			proxyGeometry = MakeRef<Primitive>();

			Bound3f bound({ 0,0,0 }, { 1,1,1 });
			Point3f points[8];
			Point3f texCoords[8];
			for (int i = 0; i < 8; i++)
			{
				points[i] = bound.Corner(i);
				texCoords[i] = bound.Corner(i);
			}
			unsigned int indices[] = 
			{
				0,2,1,1,2,3,
				4,5,6,5,7,6,
				0,1,4,1,5,4,
				2,6,3,3,6,7,
				0,4,2,2,4,6,
				1,3,5,3,7,5
			};
			
			auto vertexIndex = MakeRef<ArrayUInt>();
			vertexIndex->GetBufferObject()->SetLocalData(indices, sizeof(indices));
			vertexArray = MakeRef<ArrayFloat3>();
			texCoordArray = MakeRef<ArrayFloat3>();
			vertexArray->GetBufferObject()->SetLocalData(points, sizeof(points));
			texCoordArray->GetBufferObject()->SetLocalData(texCoords, sizeof(texCoords));
			proxyGeometry->SetVertexPositionArray(vertexArray);
			proxyGeometry->SetVertexTexCoordArray(texCoordArray);
			auto drawCall = MakeRef<DrawElementsUInt>();
			drawCall->SetIndexBuffer(vertexIndex);
			proxyGeometry->DrawCalls().push_back(drawCall);

		}
		void RayCastActorEventCallback::OnActorRenderStartedEvent(Actor * actor,
			const Camera* camera,
			Renderable *,
			const Shading* shading,
			int pass)
		{
			assert(pass == 0);
			(void)pass;
			if (shading)
			{
				auto const program = shading->GetProgram();
				const auto eyePos = camera->GetPosition();
				const auto eye_position = program->GetGenericUniformLocation("eye_position");
				if (eye_position != -1)
					actor->CreateGetUniformSet()->CreateGetUniform("eye_position")->SetUniform3f(1, eyePos.ConstData());;
				// update light dir and halfway 
			}
		}

		void RayCastActorEventCallback::BindToActor(Ref<Actor> actor)
		{
			if (actor)
			{
				const auto shared_this = std::static_pointer_cast<RayCastActorEventCallback>(shared_from_this());
				actor->RemoveActorRenderEventCallback(shared_this);
				actor->AddActorRenderEventCallback(shared_this);
				actor->SetRenderable(proxyGeometry, 0);
			}
		}


		RayCast2ActorEventCallback::RayCast2ActorEventCallback()
		{
			proxyGeometry = MakeRef<Primitive>();

			Bound3f bound({ 0,0,0 }, { 1,1,1 });
			Point3f points[8];
			Point3f texCoords[8];
			for (int i = 0; i < 8; i++)
			{
				points[i] = bound.Corner(i);
				texCoords[i] = bound.Corner(i);
				//std::cout << bound.Corner(i) << std::endl;
			}
			unsigned int indices[] =
			{
				0,2,1,1,2,3,
				4,5,6,5,7,6,
				0,1,4,1,5,4,
				2,6,3,3,6,7,
				0,4,2,2,4,6,
				1,3,5,3,7,5
			};

			auto vertexIndex = MakeRef<ArrayUInt>();
			vertexIndex->GetBufferObject()->SetLocalData(indices, sizeof(indices));
			vertexArray = MakeRef<ArrayFloat3>();
			texCoordArray = MakeRef<ArrayFloat3>();
			vertexArray->GetBufferObject()->SetLocalData(points, sizeof(points));
			texCoordArray->GetBufferObject()->SetLocalData(texCoords, sizeof(texCoords));
			proxyGeometry->SetVertexPositionArray(vertexArray);
			proxyGeometry->SetVertexTexCoordArray(texCoordArray);
			auto drawCall = MakeRef<DrawElementsUInt>();
			drawCall->SetIndexBuffer(vertexIndex);
			proxyGeometry->DrawCalls().push_back(drawCall);

		}
		void RayCast2ActorEventCallback::OnActorRenderStartedEvent(Actor * actor,
			const Camera* camera,
			Renderable *,
			const Shading* shading,
			int pass)
		{
			assert(pass == 0);
			(void)pass;
			if (shading)
			{
				auto const program = shading->GetProgram().get();
				const auto eyePos = camera->GetPosition();
				const auto eye_position = program->GetGenericUniformLocation("viewPos");
				if (eye_position != -1)
					actor->CreateGetUniformSet()->CreateGetUniform("viewPos")->SetUniform3f(1, eyePos.ConstData());
				
			}

		}

		void RayCast2ActorEventCallback::BindToActor(Ref<Actor> actor)
		{
			if (actor)
			{
				const auto shared_this = std::static_pointer_cast<RayCast2ActorEventCallback>(shared_from_this());
				actor->RemoveActorRenderEventCallback(shared_this);
				actor->AddActorRenderEventCallback(shared_this);
				actor->SetRenderable(proxyGeometry, 0);
			}
		}





	}
}
