
#ifndef _SCENEMANAGER_H_
#define _SCENEMANAGER_H_
#include "../mathematics/geometry.h"
#include <vector>
#include "actor.h"

namespace ysl
{
	namespace vm
	{
		class GRAPHICS_EXPORT_IMPORT AbstraSceneManager
		{
		public:
			AbstraSceneManager() = default;
			virtual ~AbstraSceneManager() = default;
			Bound3f BoundingBox()const { return bound; }
			void SetBoundingBox(const Bound3f & bound) { this->bound = bound; }
			virtual void ExtractActors(std::vector<Ref<Actor>> & actorArray) = 0;
			virtual std::vector<Ref<Actor>> Actors() = 0;
		protected:
			Bound3f bound;
		};
	}
}

#endif