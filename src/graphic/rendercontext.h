
#ifndef _RENDERCONTEXT_H_
#define _RENDERCONTEXT_H_

#include <memory>
#include <vector>
#include <array>

#include "../mathematics/geometry.h"
#include "graphictype.h"
#include "framebuffer.h"
#include "eventinterface.h"
#include "framebufferobject.h"
#include "shaderprogram.h"

namespace ysl
{
	namespace graphics
	{
		class EnableStateSet;
		class IVertexAttribSet;
		class RenderStateSet;
		class RenderStateBox;

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
			bool HasDoubleBuffer()const { return doubleBuffer; }
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


		enum ContextState
		{
			Context_OnRenderingStarted,
			Context_OnRenderingFinished
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
			virtual void Update() = 0;


			bool InitContext();			// We use gl3w

			bool EnableUpdate()const { return enableUpdate; }
			void SetEnableUpdate(bool update) { enableUpdate = update; }
			void SetContextFormat(const RenderContextFormat& fmt);
			bool HasDoubleBuffer()const { return format.HasDoubleBuffer(); }
			bool IsInitialized()const { return initialized; }

			void SetContextState(ContextState state) {  contextState = state; }
			ContextState GetContextState()const { return contextState; }

			Ref<Framebuffer> GetFramebuffer();
			Ref<FramebufferObject> CreateFramebufferObject();
			Ref<FramebufferObject> CreateFramebufferObject(int width,int height,ReadDrawBuffer readBuffer,ReadDrawBuffer drawBuffer);


			virtual void DestroyGLResources();
			

			void AddEventListener(Ref<IEventListener> listener);
			void RemoveEventListener(Ref<IEventListener> listener);


			// Event Call

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

			//Rendering State 

			// glUseProgram
			void UseProgram(const GLSLProgram * program);


			// glVertexAttribArray
			void BindVertexArray(const IVertexAttribSet * vas);
			void Bind_VAO(int vbo_handle);
			void ApplyRenderState(const RenderStateSet * rss);
			void ApplyRenderEnable(const EnableStateSet * ess);


		private:

			void GetMaxInteger();

			void InitDefaultRenderState();
			void InitDefaultRenderEnable();


		private:

			struct MAXINTEGER
			{
				int MAX_VERTEX_ATTRIBS = 0;
				int MAX_TEXTURE_IMAGE_UNITE = 0;
			}maxInteger;



			//GLFWwindow * windowContext;
			std::vector<Ref<IEventListener>> listeners;
			std::vector<Ref<FramebufferObject>> framebufferObjects;
			Ref<Framebuffer> framebuffer;


			RenderContextFormat format;
			bool enableUpdate = true;
			bool initialized = false;
			ContextState contextState = Context_OnRenderingFinished;

			// Render State

			// Current GLSLProgram
			Ref<GLSLProgram> curProgram;
			std::array<RenderStateBox, RS_RenderState_Count> defaultRenderStates;


			//struct VertexArrayInfo
			//{
			//	unsigned vertexBufferHandle = 0;
			//	bool enabled = false;
			//};
			//std::array<VertexArrayInfo, VA_VertexAttribArrayCount> vertexAttributeInfo;

		};

	}
}

#endif
