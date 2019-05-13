
#ifndef _RENDERCONTEXT_H_
#define _RENDERCONTEXT_H_
#include "../mathematics/geometry.h"

#include <memory>
#include <vector>
#include "graphictype.h"
#include "framebuffer.h"
#include "eventinterface.h"

namespace ysl
{
	namespace gpu
	{
		class RenderContextFormat
		{
		public:
			RenderContextFormat() :
				rGBABits(8, 8, 8, 0),
				accumRGBABits(0, 0, 0, 0),
				zBufferBits(24),
				stencilBufferBits(8),
				nMultiSamples(16),
				doubleBuffer(true),
				bMultiSample(false)

			{}

			void SetRGBABits(const Vec4i & bits) { rGBABits = bits; }
			void SetAccumRGBABits(const Vec4i & bits) { accumRGBABits = bits; }
			void SetDoubleBuffer(bool on) { doubleBuffer = on; }
			void SetMultiSample(bool on) { bMultiSample = on; }
			void SetMultiSampleNumber(int num) { nMultiSamples = num; }

			Vec4i GetRGBABits()const { return rGBABits; }
			Vec4i GetAccumRGBABits()const { return accumRGBABits; }
			bool DoubleBuffer()const { return doubleBuffer; }
			bool MultiSample()const { return bMultiSample; }
			int GetMultiSampleNumber()const { return nMultiSamples; }

		private:
			Vec4i rGBABits;
			Vec4i accumRGBABits;
			int zBufferBits;
			int stencilBufferBits;
			int nMultiSamples;
			int contextClientVersion;
			int majorVersion;
			int minorVersion;
			bool doubleBuffer;
			bool bMultiSample;
		};

		class RenderContext
		{
		public:
			RenderContext() = default;
			~RenderContext() = default;
			RenderContext(const RenderContext &) = delete;
			RenderContext & operator=(const RenderContext&) = delete;
			RenderContext(RenderContext &&)noexcept;
			RenderContext & operator=(RenderContext&&)noexcept;
			virtual void SwapBuffer() = 0;
			virtual void MakeCurrent() = 0;
			void SetContextFormat(const RenderContextFormat fmt);
			bool InitContext();

			virtual SharedPtr(Framebuffer) GetFramebuffer() = 0;
			virtual Ref(Framebuffer) CreateFramebufferObject() = 0;
			virtual void DestroyGLResources() = 0;
			virtual void Update() = 0;


			void AddUIEventListener(const Ref(IEventListener) & listener);
			void DeleteUIEventListener(const Ref(IEventListener) & listener);

		private:
			//GLFWwindow * windowContext;
			std::vector<Ref(IEventListener)> listeners;

			// Render State

			// Current GLSLProgram

		};

	}
}

#endif
