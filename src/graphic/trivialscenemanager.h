
#ifndef _TRIVIALSCENEMANAGER_H_
#define _TRIVIALSCENEMANAGER_H_
#include "graphictype.h"
#include "abstrascenemanager.h"

namespace ysl
{
	namespace graphics
	{
		class GRAPHICS_EXPORT_IMPORT TrivialSceneManager:public AbstraSceneManager
		{
		public:
			TrivialSceneManager()
			{
				
			}
			void AddActor(const Ref(Actor) actor)
			{
				this->actors.push_back(actor);
			}

			void AppendActors(ActorRefArray& actorArray)override
			{
				actorArray.insert(actorArray.end(), actors.begin(), actors.end());
			}
		private:
			void ComputeBoundingBox();
			ActorRefArray actors;
		};
	}
}

#endif