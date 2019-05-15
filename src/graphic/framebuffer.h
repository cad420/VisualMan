
#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include "graphictype.h"
#include <vector>

namespace ysl
{
	namespace graphics
	{
		class GRAPHICS_EXPORT_IMPORT Framebuffer:public std::enable_shared_from_this<Framebuffer>
		{
			friend class RenderContext;
		public:
			Framebuffer(RenderContext * context,
				int width,
				int height,
				ReadDrawBuffer readBuffer,
				ReadDrawBuffer drawBuffer) :context(context), width(width), height(height),readBuffer(readBuffer)
			{
				drawBuffers.push_back(drawBuffer);
			}

			virtual ~Framebuffer() = default;

			int Width()const { return width; }
			int Height()const { return height; }
			void SetSize(int w, int h) { width = w; height = h; }
			void SetHeight(int h) { height = h; }
			void SetWidth(int w) { width = w; }
			virtual int Handle()const { return 0; }		// Default framebuffer

			bool CheckBuffer();

			void BindFramebuffer();
			void BindReadBuffer();
			void BindDrawBuffers();
			//void SetDrawBuffer(ReadDrawBuffer drawBuffer);
			//void SetDrawBuffers(ReadDrawBuffer drawBuffer0, ReadDrawBuffer drawBuffer1);
			//void SetDrawBuffers(ReadDrawBuffer drawBuffer0, ReadDrawBuffer drawBuffer1, ReadDrawBuffer drawBuffer2);
			//void SetDrawBuffers(ReadDrawBuffer drawBuffer0, ReadDrawBuffer drawBuffer1, ReadDrawBuffer drawBuffer2, ReadDrawBuffer drawBuffer3);
			template<typename ...Args>
			void SetDrawBuffers(Args&&...);

			ReadDrawBuffer ReadBuffer()const { return readBuffer; }
			const std::vector<ReadDrawBuffer> & DrawBuffers()const { return drawBuffers; }

			RenderContext * Context() { return context; }
			const RenderContext * Context()const { return context; }

		private:
			int width = -1;
			int height = -1;
			RenderContext * context = nullptr;
			std::vector<ReadDrawBuffer> drawBuffers;		
			ReadDrawBuffer readBuffer;		// only one read buffer

		};

		template <typename ... Args>
		void Framebuffer::SetDrawBuffers(Args&&... args)
		{
			drawBuffers.emplace_back(std::forward<Args>(args)...);
		}
	}
}

#endif