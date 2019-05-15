
#ifndef _RENDERCONTEXT_H_
#define _RENDERCONTEXT_H_
#include "../mathematics/geometry.h"

#include <memory>
#include <vector>
#include "graphictype.h"
#include "framebuffer.h"
#include "eventinterface.h"
#include "framebufferobject.h"

namespace ysl
{
	namespace graphics
	{
		enum class ContextProfile
		{
			Core,
			Compatibility,
			Default
		};

		class RenderContextFormat
		{
		public:
			RenderContextFormat():
				rGBABits(8, 8, 8, 0),
				accumRGBABits(0, 0, 0, 0),
				nDepthBufferBits(24),
				stencilBufferBits(8),
				nMultiSamples(16)
			{}

			void SetRGBABits(const Vec4i & bits) { rGBABits = bits; }
			void SetAccumRGBABits(const Vec4i & bits) { accumRGBABits = bits; }
			void SetDoubleBuffer(bool on) { doubleBuffer = on; }
			void SetMultiSample(bool on) { bMultiSample = on; }
			void SetMultiSampleNumber(int num) { nMultiSamples = num; }
			void SetDepthBufferBits(int num) { nDepthBufferBits = num; }
			void SetStencilBufferBits(int num) { stencilBufferBits = num; }

			Vec4i GetRGBABits()const { return rGBABits; }
			Vec4i GetAccumRGBABits()const { return accumRGBABits; }
			bool DoubleBuffer()const { return doubleBuffer; }
			bool MultiSample()const { return bMultiSample; }
			int GetMultiSampleNumber()const { return nMultiSamples; }
			int GetDepthBufferBits()const { return nDepthBufferBits; }
			int GetStencilBufferBits()const { return stencilBufferBits; }

			ContextProfile Profile()const { return ContextProfile::Core; }

		private:
			Vec4i rGBABits;
			Vec4i accumRGBABits;
			int nDepthBufferBits;
			int stencilBufferBits;
			int nMultiSamples;
			int contextClientVersion;
			int majorVersion = 4;
			int minorVersion = 4;
			bool doubleBuffer = true;
			bool bMultiSample = false;

		};

		class GRAPHICS_EXPORT_IMPORT RenderContext
		{
		public:
			RenderContext();
			virtual ~RenderContext() = default;
			RenderContext(const RenderContext &) = delete;
			RenderContext & operator=(const RenderContext&) = delete;
			RenderContext(RenderContext && other)noexcept:listeners(std::move(other.listeners))
			{
				
			}
			RenderContext & operator=(RenderContext&&)noexcept
			{
				return *this;
			}
			virtual void SwapBuffer() = 0;
			virtual void MakeCurrent() = 0;
			bool InitContext();			// We use gl3w

			bool EnableUpdate()const { return enableUpdate; }
			void SetEnableUpdate(bool update) { enableUpdate = update; }

			void SetContextFormat(const RenderContextFormat& fmt);
			

			Ref<Framebuffer> GetFramebuffer();
			Ref<FramebufferObject> CreateFramebufferObject();
			Ref<FramebufferObject> CreateFramebufferObject(int width,int height,ReadDrawBuffer readBuffer,ReadDrawBuffer drawBuffer);


			virtual void DestroyGLResources();
			virtual void Update() = 0;


			void AddUIEventListener(const Ref<IEventListener> & listener);
			void DeleteUIEventListener(const Ref<IEventListener> & listener);

			virtual void DispatchInitEvent();
			virtual void DispatchUpdateEvent();
			virtual void DispatchDestroyEvent();
			virtual void DispatchResizeEvent(int w, int h);
			virtual void DispatchMousePressedEvent(EMouseButton button,int xpos,int ypos);
			virtual void DispatchMouseMoveEvent(EMouseButton button,int xpos,int ypos);
			virtual void DispatchMouseReleasedEvent(EMouseButton,int xpos,int ypos);
			virtual void DispatchMouseWheelEvent(int ydegree, int xdegree);
			virtual void DispatchKeyReleasedEvent(EKeyButton key);
			virtual void DispatchKeyPressedEvent(EKeyButton key);

		private:
			//GLFWwindow * windowContext;
			std::vector<Ref<IEventListener>> listeners;
			std::vector<Ref<FramebufferObject>> framebufferObjects;
			Ref<Framebuffer> framebuffer;

			RenderContextFormat format;
			bool enableUpdate = true;

			// Render State

			// Current GLSLProgram

		};

	}
}

#endif
