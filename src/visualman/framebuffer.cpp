
#include <GL/gl3w.h>
#include "framebuffer.h"
#include "graphictype.h"
#include <cassert>

namespace ysl
{
namespace vm
{
bool Framebuffer::CheckBuffer()
{
	return true;
}

void Framebuffer::BindFramebuffer( FramebufferBind target )
{
	GL( glBindFramebuffer( target, 0 ) );  // Bind to default framebuffer
	if ( target == FBB_FRAMEBUFFER || target == FBB_READ_FRAMEBUFFER )
		BindReadBuffer();
	if ( target == FBB_FRAMEBUFFER || target == FBB_DRAW_FRAMEBUFFER )
		BindDrawBuffers();
}
void Framebuffer::Activate( FramebufferBind target )
{
	BindFramebuffer( target );
}

void Framebuffer::BindReadBuffer()
{
	GL( glReadBuffer( readBuffer ) );
}

void Framebuffer::BindDrawBuffers()
{
	if ( !drawBuffers.empty() ) {
		GL( glNamedFramebufferDrawBuffers( Handle(), drawBuffers.size(), (GLenum *)drawBuffers.data() ) );
	}
}
}  // namespace vm
}  // namespace ysl
