
#ifndef _BLITFRAMEBUFFER_H_
#define _BLITFRAMEBUFFER_H_

#include "graphictype.h"
#include "renderevent.h"
#include "framebuffer.h"
#include <VMat/geometry.h>

namespace vm
{
class VISUALMAN_EXPORT_IMPORT BlitFramebufferEvent : public IRenderEvent
{
public:
	BlitFramebufferEvent() = default;
	BlitFramebufferEvent( VMRef<Framebuffer> srcFB, const Bound2i &srcRect, VMRef<Framebuffer> dstFB, const Bound2i &dstRect, BufferBits mask );
	BlitFramebufferEvent( VMRef<Framebuffer> srcFB, VMRef<Framebuffer> dstFB, BufferBits mask );

	void SetSrcRect( const Bound2i &rect ) { srcRect = rect; }
	void SetDstRect( const Bound2i &rect ) { dstRect = rect; }
	Bound2i GetSrcRect() const { return srcRect; }
	Bound2i GetDstRect() const { return dstRect; }
	void SetMask( int mask ) { this->mask = mask; }
	void SetSrcReadBuffer( ReadDrawBuffer drawBuffer ) { this->drawBuffer = drawBuffer; }

	VMRef<Framebuffer> GetReadFramebuffer() { return srcFBO; }
	VMRef<const Framebuffer> GetReadFramebuffer() const { return srcFBO; }
	VMRef<Framebuffer> GetDrawFramebuffer() { return dstFBO; }
	VMRef<const Framebuffer> GetDrawFramebuffer() const { return dstFBO; }
	void SetReadFramebuffer( VMRef<Framebuffer> fb ) { srcFBO = std::move( fb ); }
	void SetDrawFramebuffer( VMRef<Framebuffer> fb ) { dstFBO = std::move( fb ); }

	bool OnRendererFinishedEvent( const AbstraRenderer *renderer ) override
	{
		Blit();
		return true;
	}
	bool OnRendererStartedEvent( const AbstraRenderer *renderer ) override
	{
		Blit();
		return true;
	}
	bool OnRenderingStartedEvent( const vm::AbstraAggregate *studio ) override
	{
		Blit();
		return true;
	}
	bool OnRenderingFinishedEvent( const vm::AbstraAggregate *studio ) override
	{
		Blit();
		return true;
	}

private:
	void Blit();

	VMRef<Framebuffer> srcFBO;
	VMRef<Framebuffer> dstFBO;
	Bound2i srcRect{ { 0, 0 }, { 0, 0 } };
	Bound2i dstRect{ { 0, 0 }, { 0, 0 } };
	ReadDrawBuffer drawBuffer = RDB_COLOR_ATTACHMENT0;
	int mask = 0;
};
}  // namespace vm
#endif