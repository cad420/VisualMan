
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
			for (int i = 0; i < 10; i++)
			{
				lodTable.push_back(1.0 / std::pow(8, 10 - i - 1));
			}
		}

		void ScreenActorEventCallback::OnActorRenderStartedEvent(Actor*actor, const Camera * camera,
			Renderable*, const Shading*  shading, int)
		{
			if (camera)
			{

				actor->CreateGetUniformSet()->CreateGetUniform("fov")->SetUniformValue(camera->GetFov());
				actor->CreateGetUniformSet()->CreateGetUniform("fuckPos")->SetUniform3f(1, camera->GetPosition().Data());

				//const auto d = (camera->GetPosition() - Point3f(0, 0, 0)).Length();
				//const float fov = camera->GetFov();
				//const float fovRadian = fov * Pi / 180.0;
				//const Vec2i vSize(camera->GetViewport()->GetWidth(), camera->GetViewport()->GetHeight());
				//const float a = fovRadian * fovRadian*camera->GetAspectRatio() / (vSize.x*vSize.y*1.0);
				//const auto samplingRate = 0.01;
				//const auto r = (a * (d*d*samplingRate + d * samplingRate*samplingRate + samplingRate * samplingRate*samplingRate / 3.0));
				//for (int i = 0; i < 10; i++)
				//{
				//	if (r < lodTable[i])
				//	{
				//		//actor->CreateGetUniformSet()->CreateGetUniform("CUR_LOD")->SetUniformValue(i);
				//		std::cout << ":" << i << std::endl;
				//		return;
				//	}
				//}
			}
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
