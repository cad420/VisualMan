
#ifndef _SCENEMANAGER_H_
#define _SCENEMANAGER_H_
#include "../mathematics/geometry.h"
#include <vector>
#include "actor.h"

namespace ysl
{
	namespace graphics
	{
		class GRAPHICS_EXPORT_IMPORT AbstraSceneManager
		{
		public:
			AbstraSceneManager() = default;
			virtual ~AbstraSceneManager() = default;
			Bound3f BoundingBox()const { return bound; }
			void SetBoundingBox(const Bound3f & bound) { this->bound = bound; }
			virtual void AppendActors(ActorRefArray& actorArray) = 0;
		protected:
			Bound3f bound;
		};
	}
}

#endif