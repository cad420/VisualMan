
#ifndef _TRIVIALSCENEMANAGER_H_
#define _TRIVIALSCENEMANAGER_H_
#include "graphictype.h"
#include "abstrascenemanager.h"

namespace vm
{
class TrivialSceneManager : public AbstraSceneManager
{
public:
	TrivialSceneManager()
	{
	}
	void AddActor( const VMRef<Actor> actor )
	{
		this->actors.push_back( actor );
	}

	void ExtractActors( std::vector<VMRef<Actor>> &actorArray ) override
	{
		actorArray.insert( actorArray.end(), actors.begin(), actors.end() );
	}

	std::vector<VMRef<Actor>> Actors() override
	{
		return actors;
	}

private:
	void ComputeBoundingBox();
	std::vector<VMRef<Actor>> actors;
};
}  // namespace vm

#endif