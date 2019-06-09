
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

		void Framebuffer::BindFramebuffer(FramebufferBind target)
		{
			GL(glBindFramebuffer(target, 0));			// Bind to default framebuffer
			if (target == FBB_FRAMEBUFFER || target == FBB_READ_FRAMEBUFFER)
				BindReadBuffer();
			if (target == FBB_FRAMEBUFFER || target == FBB_DRAW_FRAMEBUFFER)
				BindDrawBuffers();
		}

		void Framebuffer::Activate(FramebufferBind target)
		{
			BindFramebuffer(target);
		}

		void Framebuffer::BindReadBuffer()
		{
			GL(glReadBuffer(readBuffer));
		}

		void Framebuffer::BindDrawBuffers()
		{
			if (!drawBuffers.empty())
			{
				//assert(Handle());
				GL(glNamedFramebufferDrawBuffers(Handle(), drawBuffers.size(), (GLenum*)drawBuffers.data()));
			}
		}

		//void Framebuffer::BindFramebuffer()
		//{

		//}

		//void Framebuffer::SetDrawBuffer(ReadDrawBuffer drawBuffer)
		//{
		//	drawBuffers.clear();
		//	drawBuffers.push_back(drawBuffer);
		//}

		//void Framebuffer::SetDrawBuffers(ReadDrawBuffer drawBuffer0, ReadDrawBuffer drawBuffer1)
		//{
		//	drawBuffers.clear();
		//	drawBuffers.push_back(drawBuffer0);
		//	drawBuffers.push_back(drawBuffer1);
		//}

		//void Framebuffer::SetDrawBuffers(ReadDrawBuffer drawBuffer0, ReadDrawBuffer drawBuffer1,
		//	ReadDrawBuffer drawBuffer2)
		//{
		//	drawBuffers.clear();
		//	drawBuffers.push_back(drawBuffer0);
		//	drawBuffers.push_back(drawBuffer1);
		//	drawBuffers.push_back(drawBuffer2);
		//}

		//void Framebuffer::SetDrawBuffers(ReadDrawBuffer drawBuffer0, ReadDrawBuffer drawBuffer1,
		//	ReadDrawBuffer drawBuffer2, ReadDrawBuffer drawBuffer3)
		//{
		//	drawBuffers.clear();
		//	drawBuffers.push_back(drawBuffer0);
		//	drawBuffers.push_back(drawBuffer1);
		//	drawBuffers.push_back(drawBuffer2);
		//	drawBuffers.push_back(drawBuffer3);
		//}
	}
}
