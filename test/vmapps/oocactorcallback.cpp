
#include "oocactorcallback.h"
#include <abstraarray.h>
#include <drawelements.h>
#include "oocprimitive.h"
#include "oocresources.h"

namespace ysl
{
	namespace vm
	{
		OOCActorEvent::OOCActorEvent()
		{
			proxyGeometry = MakeRef<OutOfCorePrimitive>();

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

			// bind resources
			auto oocresource = MakeRef<OutOfCoreResources>();

			oocresource->BindPrimitive(proxyGeometry);

		}
		void OOCActorEvent::OnActorRenderStartedEvent(Actor * actor,
			const Camera* camera,
			Renderable *,
			const Shading* shading,
			int pass)
		{

		}

		void OOCActorEvent::BindToActor(Ref<Actor> actor)
		{
			if (actor)
			{
				const auto shared_this = std::static_pointer_cast<OOCActorEvent>(shared_from_this());
				actor->RemoveActorRenderEventCallback(shared_this);
				actor->AddActorRenderEventCallback(shared_this);
				actor->SetRenderable(proxyGeometry, 0);
			}
		}

	}
}
