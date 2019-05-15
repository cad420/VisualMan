
#include "framebufferobject.h"

namespace ysl
{
	namespace graphics
	{
		void AbstraFBOAttachment::DetachFromAllFBO()
		{

		}

		void AbstraFBOAttachment::BindAttachment(Ref<FramebufferObject> fbo, AttachmentBindPoint point)
		{

		}

		void FramebufferObject::Create()
		{

		}

		void FramebufferObject::Destroy()
		{

		}

		void FramebufferObject::CheckFramebufferStatus()
		{

		}

		void FramebufferObject::AddColorAttachment(AttachmentBindPoint point, Ref<AbstraFBOAttachment> colorAttachment)
		{

		}

		void FramebufferObject::AddTextureAttachment(AttachmentBindPoint point,
			Ref<AbstraFBOTextureAttachment> texAttachment)
		{

		}

		void FramebufferObject::AddDepthAttachment(Ref<FBODepthAttachment> depthAttachment)
		{
		}

		void FramebufferObject::AddStencilAttachment(Ref<FBOStencilAttachment> stencilAttachment)
		{
		}

		void FramebufferObject::RemoveAllAttachments()
		{
		}

		void FramebufferObject::RemoveAttachments(AttachmentBindPoint point)
		{
		}

		void FramebufferObject::RemoveAttachments(const Ref<AbstraFBOAttachment>& attachment)
		{

		}
	}
}