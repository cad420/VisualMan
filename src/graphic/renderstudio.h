
#ifndef _RENDERSTUDIO_H_
#define _RENDERSTUDIO_H_
#include <vector>
#include <memory>
#include "abstrascenemanager.h"
#include "camera.h"
#include "graphictype.h"
#include "abstrarenderstudio.h"
#include "abstrarenderer.h"
#include "renderer.h"

namespace ysl
{
	namespace graphics
	{
		class GRAPHICS_EXPORT_IMPORT RenderStudio:public AbstraRenderStudio
		{
		public:
			RenderStudio()
			{
				camera = MakeRef<Camera>();  // A default camera
				renderers.push_back(MakeRef<Renderer>(new Renderer));
			}
			void SetCamera(const Ref<Camera> & camera)
			{
				this->camera = camera;
			};
			Ref<Camera> GetCamera()const { return camera; };
			void Render() override;
			
			std::vector<Ref<AbstraSceneManager>> & SceneManager() { return sceneManagers; }
			const std::vector<Ref<AbstraSceneManager>> & SceneManager()const { return sceneManagers; }

			std::vector<Ref<AbstraRenderer>> & Renderers() { return renderers; }
			const std::vector<Ref<AbstraRenderer>> & Renderers()const { return renderers; }

			
			
		protected:
			std::vector<Ref<AbstraSceneManager>> sceneManagers;
			std::vector<Ref<AbstraRenderer>> renderers;

			std::vector<Ref<Actor>> actorQueue; // extract from scene manager

			Ref<Camera> camera;

		private:

			RenderQueue MakeRenderQueue(const std::vector<Ref<Actor>> & queue);

		};
	}
}

#endif