
#ifndef _AGGREGATE_H_
#define _AGGREGATE_H_
#include <vector>
#include <memory>
#include "abstrascenemanager.h"
#include "camera.h"
#include "graphictype.h"
#include "abstrarenderstudio.h"
#include "renderer.h"

namespace ysl
{
	namespace vm
	{
		class GRAPHICS_EXPORT_IMPORT Aggregate:public AbstraAggregate
		{
		public:
			Aggregate()
			{
				
				camera = MakeRef<Camera>();  // A default camera
				renderers.push_back(MakeRef<Renderer>());
			}
			void SetCamera(const Ref<Camera> & camera)
			{
				this->camera = camera;
			}

			Ref<Camera> GetCamera()const { return camera; };
			void Render() override;
			
			std::vector<Ref<AbstraSceneManager>> & SceneManager() { return sceneManagers; }
			const std::vector<Ref<AbstraSceneManager>> & SceneManager()const { return sceneManagers; }

			std::vector<Ref<Renderer>> & Renderers() { return renderers; }
			const std::vector<Ref<Renderer>> & Renderers()const { return renderers; }

			
		protected:
			std::vector<Ref<AbstraSceneManager>> sceneManagers;
			std::vector<Ref<Renderer>> renderers;
			std::vector<Ref<Actor>> actorQueue; // extract from scene manager
			Ref<Camera> camera;
		private:
			RenderQueue MakeRenderQueue(const std::vector<Ref<Actor>> & queue);
		};
	}
}

#endif