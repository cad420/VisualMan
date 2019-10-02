
#ifndef _TRIVIALSCENEMANAGER_H_
#define _TRIVIALSCENEMANAGER_H_
#include "graphictype.h"
#include "abstrascenemanager.h"

namespace ysl
{
namespace vm
{
class TrivialSceneManager : public AbstraSceneManager
{
public:
	TrivialSceneManager()
	{
	}
	void AddActor( const Ref<Actor> actor )
	{
		this->actors.push_back( actor );
	}

	void ExtractActors( std::vector<Ref<Actor>> &actorArray ) override
	{
		actorArray.insert( actorArray.end(), actors.begin(), actors.end() );
	}

	std::vector<Ref<Actor>> Actors() override
	{
		return actors;
	}

private:
	void ComputeBoundingBox();
	std::vector<Ref<Actor>> actors;
};
}  // namespace vm
}  // namespace ysl

#endif