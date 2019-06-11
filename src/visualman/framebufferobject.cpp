
#include "framebufferobject.h"
#include "rendercontext.h"
#include "graphictype.h"
#include <cassert>
#include <iostream>

namespace ysl
{
	namespace vm
	{
		void AbstraFBOAttachment::DetachFromAllFBO()
		{
			for (auto it = frambufferObjects.begin(); it != frambufferObjects.end();)
			{
				if (!it->expired())
					it->lock()->RemoveAttachments(shared_from_this());
				it = frambufferObjects.erase(it);
			}
		}

		void FBORenderBufferAttachment::CreateRenderBuffer()
		{
			if (handle == 0)
			{
				GL(glCreateRenderbuffers(1, &handle));
				storageChanged = true;
			}
		}

		void FBORenderBufferAttachment::DestroyRenderBuffer()
		{

			DetachFromAllFBO();

			if (handle != 0)
			{
				GL(glDeleteRenderbuffers(1, &handle));
				storageChanged = true;
				handle = 0;
			}
		}

		void FBORenderBufferAttachment::InitStorage(int w, int h, int samples)
		{
			assert(w > 0 && h > 0 && samples >= 0);
			if (w != width || h != height)
			{
				storageChanged = true;
			}
			width = w;
			height = h;
			this->samples = samples;

			if (handle == 0)
				CreateRenderBuffer();

			// init storage
			if (samples > 0)
			{
				GL(glNamedRenderbufferStorageMultisample(handle, samples, InternalType(), width, height));
			}
			else
			{
				// sample = 0;
				GL(glNamedRenderbufferStorage(handle, InternalType(), width, height));
			}
			storageChanged = false;
		}

		void FBORenderBufferAttachment::BindAttachment(Ref<FramebufferObject> fbo, AttachmentBindPoint point)
		{
			assert(fbo);
			CreateRenderBuffer();

			// If render buffer is not set, use the fbo's size as default size
			int w = Width() ? Width() : fbo->Width();
			int h = Height() ? Height() : fbo->Height();

			assert(w >= 0 && h >= 0);
			InitStorage(w, h, samples);
			assert(glIsFramebuffer(fbo->Handle()) == GL_TRUE);
			GL(glNamedFramebufferRenderbuffer(fbo->Handle(), point, GL_RENDERBUFFER, handle));
		}



		void FBOTextureAttachment::BindAttachment(Ref<FramebufferObject> fbo, AttachmentBindPoint point)
		{
			assert(fbo);

			assert(texture);

			assert(texture->Handle());

			// texture is created
			//std::cout << "adfsadfsdaf--------------";
			GL(glIsFramebuffer(texture->Handle()) == GL_TRUE);
			GL(glNamedFramebufferTexture(fbo->Handle(), point, texture->Handle(), mipMapLevel));

		}

		void FramebufferObject::BindFramebuffer(FramebufferBind target)
		{
			assert(handle);

			assert((Width() > 0 || Height() > 0));

			assert(!attachments.empty());

			assert(Context());

			Context()->MakeCurrent();

			GL(glBindFramebuffer(target, handle));			// Bind to default framebuffer
			if (target == FBB_FRAMEBUFFER || target == FBB_READ_FRAMEBUFFER)
				BindReadBuffer();
			if (target == FBB_FRAMEBUFFER || target == FBB_DRAW_FRAMEBUFFER)
				BindDrawBuffers();

			assert(CheckFramebufferStatus() == GL_FRAMEBUFFER_COMPLETE);
		}

		void FramebufferObject::CreateFrambufferObject()
		{
			assert(Context());

			Context()->MakeCurrent();

			if (handle == 0)
			{
				GL(glCreateFramebuffers(1, &handle));
				assert(handle);
			}

		}

		void FramebufferObject::DestroyFramebufferObject()
		{
			if (handle != 0)
			{
				GL(glDeleteFramebuffers(1, &handle));
				handle = 0;
			}
		}

		int FramebufferObject::CheckFramebufferStatus()
		{
			int status;
			GL(status = glCheckNamedFramebufferStatus(handle, GL_FRAMEBUFFER));
			switch (status) {
			case GL_FRAMEBUFFER_COMPLETE: // Everything's OK

				break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				std::cout << "glift::CheckFramebufferStatus() ERROR:\n\t"
					<< "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				std::cout << "glift::CheckFramebufferStatus() ERROR:\n\t"
					<< "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n";
				break;
			case GL_FRAMEBUFFER_UNDEFINED:
				std::cout << "glift::CheckFramebufferStatus() ERROR:\n\t"
					<< "GL_FRAMEBUFFER_UNDEFINED\n";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				std::cout << "glift::CheckFramebufferStatus() ERROR:\n\t"
					<< "GL_FRAMEBUFFER_INCOMPLETE_FORMATS\n";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				std::cout << "glift::CheckFramebufferStatus() ERROR:\n\t"
					<< "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				std::cout << "glift::CheckFramebufferStatus() ERROR:\n\t"
					<< "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n";
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				std::cout << "glift::CheckFramebufferStatus() ERROR:\n\t"
					<< "GL_FRAMEBUFFER_UNSUPPORTED\n";
				break;
			default:
				std::cout << "glift::CheckFramebufferStatus() ERROR:\n\t"
					<< "Unknown ERROR\n";

			}
			return status;
		}

		void FramebufferObject::AddColorAttachment(AttachmentBindPoint point,
			Ref<FBOColorBufferAttachment> colorAttachment)
		{
			assert(point >= AP_COLOR_ATTACHMENT0 && point <= AP_COLOR_ATTACHMENT15);
			CreateFrambufferObject();
			//RemoveAttachments(point);
			const auto thisFBO = std::static_pointer_cast<FramebufferObject>(shared_from_this());
			(colorAttachment->frambufferObjects).insert(thisFBO);
			colorAttachment->BindAttachment(thisFBO, point);
			attachments[point] = std::move(colorAttachment);	// remove old attachment attached on this point
		}

		void FramebufferObject::AddTextureAttachment(AttachmentBindPoint point,
			Ref<AbstraFBOTextureAttachment> texAttachment)
		{
			assert(point >= AP_COLOR_ATTACHMENT0 && point <= AP_COLOR_ATTACHMENT15);
			CreateFrambufferObject();
			//RemoveAttachments(point);
			const auto thisFBO = std::static_pointer_cast<FramebufferObject>(shared_from_this());
			(texAttachment->frambufferObjects).insert(thisFBO);
			texAttachment->BindAttachment(thisFBO, point);
			attachments[point] = std::move(texAttachment);	// remove old attachment attached on this point
		}

		void FramebufferObject::AddDepthAttachment(Ref<FBODepthAttachment> depthAttachment)
		{
			CreateFrambufferObject();
			//RemoveAttachments(point);
			const auto thisFBO = std::static_pointer_cast<FramebufferObject>(shared_from_this());
			depthAttachment->BindAttachment(thisFBO, AP_DEPTH_ATTACHMENT);
			(depthAttachment->frambufferObjects).insert(thisFBO);
			attachments[AP_DEPTH_ATTACHMENT] = std::move(depthAttachment);
		}

		void FramebufferObject::AddStencilAttachment(Ref<FBOStencilAttachment> stencilAttachment)
		{
			CreateFrambufferObject();
			const auto thisFBO = std::static_pointer_cast<FramebufferObject>(shared_from_this());
			stencilAttachment->BindAttachment(thisFBO, AP_STENCIL_ATTACHMENT);
			(stencilAttachment->frambufferObjects).insert(thisFBO);
			attachments[AP_DEPTH_ATTACHMENT] = std::move(stencilAttachment);
		}

		void FramebufferObject::AddDepthStencilAttachment(Ref<FBODepthStencilAttachment> dsAttachment)
		{
			CreateFrambufferObject();
			const auto thisFBO = std::static_pointer_cast<FramebufferObject>(shared_from_this());
			dsAttachment->BindAttachment(thisFBO, AP_DEPTH_STENCIL_ATTACHMENT);
			(dsAttachment->frambufferObjects).insert(thisFBO);
			attachments[AP_DEPTH_ATTACHMENT] = std::move(dsAttachment);
		}

		void FramebufferObject::RemoveAllAttachments()
		{
			//std::swap(attachments, std::map<AttachmentBindPoint, Ref<AbstraFBOAttachment>>());
			std::map<AttachmentBindPoint,Ref<AbstraFBOAttachment>>().swap(attachments);
		}

		void FramebufferObject::RemoveAttachments(AttachmentBindPoint point)
		{
			if (handle)
			{
				assert(Context());
				Context()->MakeCurrent();
				attachments.erase(point);
			}
		}

		void FramebufferObject::RemoveAttachments(const Ref<AbstraFBOAttachment>& attachment)
		{
			std::vector<AttachmentBindPoint> points;
			for (auto it = attachments.begin(); it != attachments.end(); ++it)
			{
				if (it->second == attachment)
				{
					points.push_back(it->first);
				}
			}

			for (const auto & each : points)
			{
				RemoveAttachments(each);
			}
		}
	}
}
