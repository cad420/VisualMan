
#ifndef _RENDERSTUDIO_H_
#define _RENDERSTUDIO_H_
#include "abstrarenderstudio.h"
#include <vector>
#include "graphictype.h"
#include <memory>
#include "abstrascenemanager.h"
#include "../cameras/pbrcamera.h"

namespace ysl
{
	namespace gpu
	{
		class RenderStudio:public AbstraRenderStudio
		{
		public:
			RenderStudio(){}
			void SetCamera(const Ref(Camera) & camera)
			{
				this->camera = camera;
			};
			Ref(Camera) GetCamera()const { return camera; };

			
		protected:
			std::vector<Ref(AbstraSceneManager)> sceneManagers;
			Ref(Camera) camera;
		};
	}
}

#endif