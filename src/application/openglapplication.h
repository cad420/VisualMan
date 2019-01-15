
#ifndef _OPENGLAPPLICATION_H_
#define _OPENGLAPPLICATION_H_

#include "application.h"
#include <memory>
#include "../opengl/framebuffer.h"

namespace ysl
{
	namespace app
	{
		class OpenGLApplication :public Application
		{
		public:
			OpenGLApplication(int argc, char ** argv, int w, int h);
			DISABLE_COPY(OpenGLApplication);
			DISABLE_MOVE(OpenGLApplication);
			void SetClearColor(float * color[4]);
			~OpenGLApplication() = default;
			int Exec() override;
		protected:

			virtual void RenderLoop() = 0;
			virtual void InitializeOpenGLResources() = 0;
			void InitOpenGLContext();
			void DestroyOpenGLContext();

			//virtual void MousePressEvent(MouseEvent* e) override;
			//virtual void MouseReleaseEvent(MouseEvent* e) override;
			//virtual void MouseMoveEvent(MouseEvent* e) override;

			GLFWwindow * window;
			int width;
			int height;
			float clearColor[4];
		};
	}

}



#endif
