
#ifndef _SCENEMANAGER_H_
#define _SCENEMANAGER_H_
#include <vector>
#include "geometry.h"
#include "actor.h"
namespace ysl
{
	namespace vm
	{
		class Actor;
		class VISUALMAN_EXPORT_IMPORT AbstraSceneManager
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