
#include "blitframebuffer.h"
#include "graphictype.h"

namespace ysl
{
	namespace vm
	{

		BlitFramebufferEvent::BlitFramebufferEvent(Ref<Framebuffer> srcFB, const Bound2i& srcRect,
			Ref<Framebuffer> dstFB, const Bound2i& dstRect, BufferBits mask):
			srcFBO(std::move(srcFB)),
			srcRect(srcRect),
			dstFBO(std::move(dstFB)),
			dstRect(dstRect),
			mask(mask)
		{
		}

		BlitFramebufferEvent::BlitFramebufferEvent(Ref<Framebuffer> srcFB, Ref<Framebuffer> dstFB, BufferBits mask) :
			srcFBO(std::move(srcFB)),
			dstFBO(std::move(dstFB)),
			mask(mask)

		{
			if (srcFB) srcRect = Bound2i{ {0,0},{srcFB->Width(),srcFB->Height()} };
			if (dstFB) dstRect = Bound2i{ {0,0},{dstFB->Width(),dstFB->Height()} };
		}

		void BlitFramebufferEvent::Blit()
		{
			const auto src = srcFBO->Handle();
			const auto dst = dstFBO->Handle();
			GL(glBlitNamedFramebuffer(src,
				dst,
				srcRect.min.x, srcRect.min.y, srcRect.max.x - srcRect.min.x, srcRect.max.y - srcRect.min.y,
				dstRect.min.x, dstRect.min.y, dstRect.max.x - dstRect.min.x, dstRect.max.y - dstRect.min.y,
				mask,
				GL_LINEAR));
		}

	}


}
