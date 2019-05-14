
#include "rendercontext.h"
#include "../../lib/gl3w/GL/gl3w.h"

namespace ysl {
	namespace gpu
	{
		bool RenderContext::InitContext()			// can be seemed as InitGLResources()
		{
			gl3wInit();
			if (!gl3wIsSupported(4, 4))
			{
				throw std::runtime_error("OpenGL 4.4 or higher is needed.");
			}
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
			return Ref < Framebuffer>();
		}

		Ref<Framebuffer> RenderContext::CreateFramebufferObject()
		{
			return Ref<Framebuffer>();
		}


		void RenderContext::AddUIEventListener(const Ref<IEventListener> & listener)
		{
			if(listener->context == nullptr)
			{
				listener->context = this;
				listeners.push_back(listener);
			}
		}

		void RenderContext::DeleteUIEventListener(const Ref<IEventListener> & listener)
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
			for(const auto & each:listeners)
			{
				if (each->Enabled())
					each->InitEvent();
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
