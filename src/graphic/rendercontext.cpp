
#include "rendercontext.h"
#include "vertexattribsetinterface.h"

#include "../../lib/gl3w/GL/gl3w.h"
#include "../opengl/openglutils.h"

#include "abstraarray.h"
#include "renderstateset.h"
#include "enableset.h"
#include "abstrarenderstate.h"
#include "ogl.h"

namespace ysl {
	namespace graphics
	{
		RenderContext::RenderContext() :
			framebuffer(MakeRef<Framebuffer>(this, 800, 600, RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT0))
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

			GetMaxInteger();

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
			assert(framebuffer);
			return framebuffer;
		}

		Ref<FramebufferObject> RenderContext::CreateFramebufferObject()
		{
			return CreateFramebufferObject(0, 0, RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT0);
		}

		Ref<FramebufferObject> RenderContext::CreateFramebufferObject(int width, int height, ReadDrawBuffer readBuffer,
			ReadDrawBuffer drawBuffer)
		{
			framebufferObjects.push_back(MakeRef<FramebufferObject>(this, width, height, readBuffer, drawBuffer));
			framebufferObjects.back()->CreateFrambufferObject();
			return framebufferObjects.back();
		}


		void RenderContext::AddEventListener(Ref<IEventListener> listener)
		{
			if (listener->context == nullptr && listener)
			{
				listener->context = this;
				if (IsInitialized())
					listener->InitEvent();
				listener->AddedEvent(this);
				listeners.push_back(std::move(listener));
			}
		}

		void RenderContext::RemoveEventListener(Ref<IEventListener> listener)
		{
			if (listener && listener->context == this)
			{
				for (auto it = listeners.begin(); it != listeners.end();)
				{
					if (*it == listener)
					{
						it = listeners.erase(it);
						listener->DeletedEvent(this);
					}
					else
					{
						++it;
					}
				}
			}
		}

		void RenderContext::DispatchInitEvent()
		{
			//std::cout << " RenderContext::DispatchInitEvent\n";
			MakeCurrent();
			if (IsInitialized())
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
			//std::cout << "RenderContext::DispatchUpdateEvent\n";
			MakeCurrent();
			for (const auto & each : listeners)
			{
				if (each->Enabled())
					each->UpdateEvent();
			}
		}

		void RenderContext::DispatchDestroyEvent()
		{
			//std::cout << "RenderContext::DispatchDestroyEvent\n";
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
			//std::cout << "RenderContext::DispatchResizeEvent:" << w << " " << h << std::endl;
			MakeCurrent();
			for (const auto & each : listeners)
			{
				if (each->Enabled())
					each->ResizeEvent(w, h);
			}
		}

		void RenderContext::DispatchMousePressedEvent(EMouseButton button, int xpos, int ypos)
		{
			//std::cout << "DispatchMousePressedEvent:" << button<<" "<<xpos << " " << ypos << std::endl;
			MakeCurrent();
			for (const auto & each : listeners)
			{
				if (each->Enabled())
					each->MousePressEvent(button, xpos, ypos);
			}
		}

		void RenderContext::DispatchMouseMoveEvent(EMouseButton button, int xpos, int ypos)
		{
			//std::cout << "RenderContext::DispatchMouseMoveEvent:" << button << " " << xpos << " " << ypos << std::endl;
			MakeCurrent();
			for (const auto & each : listeners)
			{
				if (each->Enabled())
					each->MouseMoveEvent(button, xpos, ypos);
			}
		}

		void RenderContext::DispatchMouseReleasedEvent(EMouseButton button, int xpos, int ypos)
		{
			//std::cout << "RenderContext::DispatchMouseReleasedEvent:" << button << " " << xpos << " " << ypos << std::endl;
			MakeCurrent();
			for (const auto & each : listeners)
			{
				if (each->Enabled())
					each->MouseReleaseEvent(button, xpos, ypos);
			}
		}

		void RenderContext::DispatchMouseWheelEvent(int ydegree, int xdegree)
		{
			//std::cout << "RenderContext::DispatchMouseWheelEvent:" <<ydegree << std::endl;
			MakeCurrent();
			for (const auto & each : listeners)
			{
				if (each->Enabled())
					each->MouseWheelEvent(ydegree, xdegree);
			}
		}


		void RenderContext::DispatchKeyReleasedEvent(EKeyButton key)
		{

			//std::cout << "RenderContext::DispatchKeyReleasedEvent:" << key << std::endl;
			MakeCurrent();
			for (const auto & each : listeners)
			{
				if (each->Enabled())
					each->KeyReleaseEvent(key);
			}
		}

		void RenderContext::DispatchKeyPressedEvent(EKeyButton key)
		{
			//std::cout << "RenderContext::DispatchKeyPressedEvent:" << key << std::endl;
			MakeCurrent();
			for (const auto & each : listeners)
			{
				if (each->Enabled())
					each->KeyPressEvent(key);
			}
		}

		void RenderContext::UseProgram(const GLSLProgram* program)
		{
			assert(program);
			GL(glUseProgram(program->Handle()));
		}

		void RenderContext::BindVertexArray(const IVertexAttribSet* vas)
		{
			assert(false);
			//if (vas)
			//{
			//	for (auto i = 0; i < maxInteger.MAX_VERTEX_ATTRIBS; i++)
			//	{
			//		const auto ptr = vas->GetVertexAttribArray(i);
			//		if (ptr == nullptr)
			//		{
			//			// if there is no corresponding generic vertex array, disable it.
			//			if(vertexAttributeInfo[i].enabled)
			//			{
			//				vertexAttributeInfo[i].enabled = false;
			//				vertexAttributeInfo[i].vertexBufferHandle = 0;
			//				GL(glDisableVertexAttribArray(i));
			//			}

			//			GL(glDisableVertexAttribArray(i));


			//		}
			//		else
			//		{
			//			// If there is a coreesponding generic vertex array, enable it
			//			if(!vertexAttributeInfo[i].enabled)
			//			{
			//				vertexAttributeInfo[i].enabled = true;
			//				//vertexAttributeInfo[i].vertexBufferHandle = ptr->GetBufferObject()->Handle();
			//				GL(glEnableVertexAttribArray(i));
			//			}

			//			const auto curBufferObjectHandle = ptr->GetBufferObject()->Handle();
			//			if(curBufferObjectHandle != vertexAttributeInfo[i].vertexBufferHandle)
			//			{
			//				vertexAttributeInfo[i].vertexBufferHandle = curBufferObjectHandle;

			//				GL(glBindBuffer(GL_ARRAY_BUFFER,curBufferObjectHandle));

			//				GL(glVertexAttribPointer(i, // attribute location
			//					ptr->ComponentNum(), // component of vector
			//					ptr->Type(), // type enum
			//					GL_FALSE,	// normalized
			//					0,		// stride
			//					reinterpret_cast<void*>(0)));		// offset

			//				Debug("%d %d",ptr->ComponentNum(),ptr->Type());

			//			}
			//		}
			//	}
			//}
		}

		void RenderContext::Bind_VAO(int vbo_handle)
		{
			GL(glBindVertexArray(vbo_handle));
		}

		void RenderContext::ApplyRenderState(const RenderStateSet* rss)
		{
			// Set the new render state and reset all states that 
			// don't exist in the new render state list as default
			assert(rss);

			std::unordered_map<RenderStateType, RenderStateBox> newStates;

			for(const auto & each: rss->renderStates)
			{
				const auto type = each.StateType();

				auto it = currentRenderStates.find(type);
				if(it == currentRenderStates.end())
				{
					each.Apply(nullptr, this);
					newStates[type] = each;
				}
			}

			for(const auto & each:currentRenderStates)
			{
				const auto type = each.first;

				auto it = newStates.find(type);
				if(it == newStates.end())
				{
					//apply default states;
					defaultRenderStates[type].Apply(nullptr, this);
				}
			}

			std::swap(currentRenderStates, newStates);
		}

		void RenderContext::ApplyRenderEnable(const EnableStateSet* ess)
		{
			// Set the new enable state and reset all enable state that
			// don't exist in the new enable state list as disabled
			assert(ess);
			std::unordered_set<EnableState> newEnableStates;

			for(const auto & each:ess->enableSet)
			{
				auto it = currentEnableStates.find(each);
				if(it == currentEnableStates.end())
				{
					GL(glEnable(EnableEnum2GLEnum[*it]));
					newEnableStates.insert(*it);
				}
			}

			for(const auto & each:currentEnableStates)
			{
				auto it = newEnableStates.find(each);
				if(it == newEnableStates.end())
				{
					GL(glDisable(EnableEnum2GLEnum[*it]));
				}
			}

			std::swap(currentEnableStates, newEnableStates);

		}

		void RenderContext::GetMaxInteger()
		{
			// Get max integer at run-time
			GL(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxInteger.MAX_VERTEX_ATTRIBS));
			maxInteger.MAX_VERTEX_ATTRIBS = std::min(int(VA_VertexAttribArray_Count), int(maxInteger.MAX_VERTEX_ATTRIBS));
			GL(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxInteger.MAX_TEXTURE_IMAGE_UNITE));
		}

		void RenderContext::InitDefaultRenderState()
		{

		}
	}
}
