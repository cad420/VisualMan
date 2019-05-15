
#include "rendercontext.h"
#include "../../lib/gl3w/GL/gl3w.h"

namespace ysl {
	namespace graphics
	{
		RenderContext::RenderContext():
		framebuffer(MakeRef<Framebuffer>(this,800,600,RDB_COLOR_ATTACHMENT0,RDB_COLOR_ATTACHMENT0))
		{
			int width = 800, height = 600;
		}

		bool RenderContext::InitContext()			// can be seemed as InitGLResources()
		{
			MakeCurrent();
			gl3wInit();
			if (!gl3wIsSupported(4, 4))
			{
				throw std::runtime_error("OpenGL 4.4 or higher is needed.");
			}

			/// TODO:: Lot of works to do here for robust.

			initialized = true;
		}

		void RenderContext::DestroyGLResources()
		{
			// Destroy all gl resources

		}

		void RenderContext::SetContextFormat(const RenderContextFormat& fmt)
		{
			format = fmt;
		}

		Ref<Framebuffer> RenderContext::GetFramebuffer()
		{
			return framebuffer;
		}

		Ref<FramebufferObject> RenderContext::CreateFramebufferObject()
		{
			return CreateFramebufferObject(0, 0, RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT0);
		}

		Ref<FramebufferObject> RenderContext::CreateFramebufferObject(int width, int height, ReadDrawBuffer readBuffer,
			ReadDrawBuffer drawBuffer)
		{
			framebufferObjects.push_back(MakeRef<FramebufferObject>(this,width, height, readBuffer, drawBuffer));
			framebufferObjects.back()->Create();
			return framebufferObjects.back();
		}


		void RenderContext::AddUIEventListener(const Ref<IEventListener> & listener)
		{
			if(listener->context == nullptr)
			{
				listener->context = this;
				listeners.push_back(listener);
				if(IsInitialized())
				{
					listener->InitEvent();
				}
			}
		}

		void RenderContext::RemoveUIEventListener(const Ref<IEventListener> & listener)
		{
			if(listener->context == this)
			{
				for(auto it = listeners.begin() ; it != listeners.end();++it)
				{
					if (*it == listener)
					{
						listeners.erase(it);
						break;
					}
				}
			}
		}

		void RenderContext::DispatchInitEvent()
		{
			std::cout << " RenderContext::DispatchInitEvent\n";
			MakeCurrent();
			if(IsInitialized())
			{
				for (const auto & each : listeners)
				{
					if (each->Enabled())
						each->InitEvent();
				}
			}
		}

		void RenderContext::DispatchUpdateEvent()
		{
			std::cout << "RenderContext::DispatchUpdateEvent\n";
			MakeCurrent();
			for (const auto & each : listeners)
			{
				if (each->Enabled())
					each->UpdateEvent();
			}
		}

		void RenderContext::DispatchDestroyEvent()
		{
			std::cout << "RenderContext::DispatchDestroyEvent\n";
			MakeCurrent();
			for (const auto & each : listeners)
			{
				if (each->Enabled())
					each->DestroyEvent();
			}
			DestroyGLResources();
		}

		void RenderContext::DispatchResizeEvent(int w, int h)
		{
			std::cout << "RenderContext::DispatchResizeEvent:" << w << " " << h << std::endl;
			MakeCurrent();
			for (const auto & each : listeners)
			{
				if (each->Enabled())
					each->ResizeEvent(w,h);
			}
		}

		void RenderContext::DispatchMousePressedEvent(EMouseButton button, int xpos, int ypos)
		{
			std::cout << "DispatchMousePressedEvent:" << button<<" "<<xpos << " " << ypos << std::endl;
			MakeCurrent();
			for (const auto & each : listeners)
			{
				if (each->Enabled())
					each->MousePressEvent(button,xpos,ypos);
			}
		}

		void RenderContext::DispatchMouseMoveEvent(EMouseButton button, int xpos, int ypos)
		{
			std::cout << "RenderContext::DispatchMouseMoveEvent:" << button << " " << xpos << " " << ypos << std::endl;
			MakeCurrent();
			for (const auto & each : listeners)
			{
				if (each->Enabled())
					each->MouseMoveEvent(button, xpos, ypos);
			}
		}

		void RenderContext::DispatchMouseReleasedEvent(EMouseButton button, int xpos, int ypos)
		{
			std::cout << "RenderContext::DispatchMouseReleasedEvent:" << button << " " << xpos << " " << ypos << std::endl;
			MakeCurrent();
			for (const auto & each : listeners)
			{
				if (each->Enabled())
					each->MouseReleaseEvent(button,xpos,ypos);
			}
		}

		void RenderContext::DispatchMouseWheelEvent(int ydegree, int xdegree)
		{
			std::cout << "RenderContext::DispatchMouseWheelEvent:" <<ydegree << std::endl;
			MakeCurrent();
			for (const auto & each : listeners)
			{
				if (each->Enabled())
					each->MouseWheelEvent(ydegree,xdegree);
			}
		}


		void RenderContext::DispatchKeyReleasedEvent(EKeyButton key)
		{

			std::cout << "RenderContext::DispatchKeyReleasedEvent:" << key << std::endl;
			MakeCurrent();
			for (const auto & each : listeners)
			{
				if (each->Enabled())
					each->KeyReleaseEvent(key);
			}
		}

		void RenderContext::DispatchKeyPressedEvent(EKeyButton key)
		{
			std::cout << "RenderContext::DispatchKeyPressedEvent:" << key << std::endl;
			MakeCurrent();
			for (const auto & each : listeners)
			{
				if (each->Enabled())
					each->KeyPressEvent(key);
			}
		}


	}
}
