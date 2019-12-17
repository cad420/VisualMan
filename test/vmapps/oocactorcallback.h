
#ifndef _OOCACTORCALLBACK_H_
#define _OOCACTORCALLBACK_H_

#include "config.h"
#include <actor.h>
#include "oocprimitive.h"
#include "ooctexture.h"
#include "oocresources.h"


	namespace vm
	{

		class ArrayFloat3;
		class OutOfCorePrimitive;

		class VMAPPS_EXPORT_IMPORT OOCActorEvent :public IActorEvent
		{
		public:
			OOCActorEvent();
			void OnActorRenderStartedEvent(Actor* actor, const Camera* camera, Renderable* renderable, const Shading* shading, int pass) override;
			void BindToActor(VMRef<Actor> actor);
			VMRef<OutOfCorePrimitive> ProxyGeometry() { return proxyGeometry; }
			VMRef<const OutOfCorePrimitive> ProxyGeometry()const { return proxyGeometry; }
			VMRef<OutOfCoreVolumeTexture> GetTexture() { return OutOfCoreVolumeTexture;  }
			VMRef<const OutOfCoreVolumeTexture> GetTexture()const { return OutOfCoreVolumeTexture;  }
		private:
			VMRef<OutOfCorePrimitive> proxyGeometry;
			VMRef<OutOfCoreVolumeTexture> OutOfCoreVolumeTexture;
			VMRef<ArrayFloat3> vertexArray;
			VMRef<ArrayFloat3> texCoordArray;
		};

	}
#endif
