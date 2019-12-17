
#ifndef _AGGREGATE_H_
#define _AGGREGATE_H_
#include <vector>
#include <memory>
#include "abstrascenemanager.h"
#include "camera.h"
#include "graphictype.h"
#include "abstrarenderstudio.h"
#include "renderer.h"

namespace vm
{
class VISUALMAN_EXPORT_IMPORT Aggregate : public AbstraAggregate
{
public:
	Aggregate()
	{
		//camera = MakeRef<Camera>();  // A default camera
		renderers.push_back( MakeVMRef<Renderer>() );
	}
	void SetCamera( const VMRef<Camera> &camera ) { this->camera = camera; }
	VMRef<Camera> CreateGetCamera() { return camera ? camera : ( camera = MakeVMRef<Camera>() ); }

	void Render() override;

	std::vector<VMRef<AbstraSceneManager>> &SceneManager() { return sceneManagers; }
	const std::vector<VMRef<AbstraSceneManager>> &SceneManager() const { return sceneManagers; }

	std::vector<VMRef<Renderer>> &Renderers() { return renderers; }
	const std::vector<VMRef<Renderer>> &Renderers() const { return renderers; }

protected:
	std::vector<VMRef<AbstraSceneManager>> sceneManagers;
	std::vector<VMRef<Renderer>> renderers;
	std::vector<VMRef<Actor>> actorQueue;	 // extract from scene manager
	VMRef<Camera> camera;

private:
	RenderQueue MakeRenderQueue( const std::vector<VMRef<Actor>> &queue );
};

/**
		 * \brief This class is used to run aggregates in a serialized order
		 */
class VISUALMAN_EXPORT_IMPORT SerializedAggregates : public AbstraAggregate	 // Decorator Pattern
{
public:
	void Render() override;
	std::vector<VMRef<AbstraAggregate>> &GetAggregates() { return aggregates; }
	const std::vector<VMRef<AbstraAggregate>> &GetAggregates() const { return aggregates; }

private:
	std::vector<VMRef<AbstraAggregate>> aggregates;
};
}  // namespace vm

#endif