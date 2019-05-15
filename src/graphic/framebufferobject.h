
#ifndef _FRAMEBUFFEROBJECT_H_
#define _FRAMEBUFFEROBJECT_H_

#include "graphictype.h"
#include <set>
#include "framebuffer.h"
#include "texture.h"

namespace ysl
{
	namespace graphics
	{
		class FramebufferObject;

		class GRAPHICS_EXPORT_IMPORT AbstraFBOAttachment:public std::enable_shared_from_this<AbstraFBOAttachment>
		{
			friend class FramebufferObject;
		public:
			AbstraFBOAttachment() = default;
			virtual ~AbstraFBOAttachment() = default;
			AbstraFBOAttachment(const AbstraFBOAttachment &) = delete;
			AbstraFBOAttachment & operator=(const AbstraFBOAttachment&) = delete;
			void DetachFromAllFBO();

		protected:
			virtual void BindAttachment(Ref<FramebufferObject> fbo, AttachmentBindPoint point) = 0;
			std::set<Ref<FramebufferObject>> frambufferobjects;
		};

		class GRAPHICS_EXPORT_IMPORT FBORenderBufferAttachment:public AbstraFBOAttachment
		{
		public:

		};

		class GRAPHICS_EXPORT_IMPORT FBOColorBufferAttachment:public AbstraFBOAttachment
		{
		public:
			FBOColorBufferAttachment(ColorBufferFormat type):type(type) {}
			ColorBufferFormat Type()const { return type; }
			void SetType(ColorBufferFormat type) { this->type = type; }
		private:
			ColorBufferFormat type = CBF_RGBA;
		};

		class GRAPHICS_EXPORT_IMPORT FBODepthAttachment:public AbstraFBOAttachment
		{
		public:
		};

		class GRAPHICS_EXPORT_IMPORT FBOStencilAttachment:public AbstraFBOAttachment
		{
		public:
			FBOStencilAttachment(StencilBufferFormat type):type(type){}
			StencilBufferFormat Type()const { return type; }
			void SetType(StencilBufferFormat type) { this->type = type; }
		private:
			StencilBufferFormat type = SBF_STENCIL_INDEX8;
		};

		class GRAPHICS_EXPORT_IMPORT FBODepthStencilAttachment:public AbstraFBOAttachment
		{
		public:
			FBODepthStencilAttachment(DepthStencilBufferFormat type):type(type){}
			DepthStencilBufferFormat Type()const { return type; }
			void SetType(DepthStencilBufferFormat type) { this->type = type; }
		private:
			DepthStencilBufferFormat type;
		};

		class GRAPHICS_EXPORT_IMPORT AbstraFBOTextureAttachment:public AbstraFBOAttachment
		{
		public:
			AbstraFBOTextureAttachment(Ref<Texture> texture):texture(std::move(texture)){}
			void SetTexture(Ref<Texture> texture) { this->texture = std::move(texture); }
			Ref<Texture> GetTexture() { return texture; }
		private:
			Ref<Texture> texture;
		};

		class GRAPHICS_EXPORT_IMPORT FBOTexture2DAttachment:public AbstraFBOTextureAttachment
		{
		public:
			FBOTexture2DAttachment(Ref<Texture> texture, Texture2DTarget target):AbstraFBOTextureAttachment(std::move(texture)),target(target)
			{

			}

			void SetTarget(Texture2DTarget target) { this->target = target; }
			Texture2DTarget Target()const { return this->target; }

		private:
			Texture2DTarget target;
		};

		class GRAPHICS_EXPORT_IMPORT FramebufferObject:public Framebuffer
		{
		public:
			FramebufferObject() :Framebuffer(nullptr,0,0,ReadDrawBuffer::RDB_COLOR_ATTACHMENT0,ReadDrawBuffer::RDB_COLOR_ATTACHMENT0)
			{
				
			}

			FramebufferObject(RenderContext * context,
				int width,
				int height,
				ReadDrawBuffer readBuffer,
				ReadDrawBuffer drawBuffer):Framebuffer(context,width,height,readBuffer,drawBuffer)
			{
				
			}

			void Create();
			void Destroy();

			void CheckFramebufferStatus();

			void AddColorAttachment(AttachmentBindPoint point,Ref<AbstraFBOAttachment> colorAttachment);
			void AddTextureAttachment(AttachmentBindPoint point, Ref<AbstraFBOTextureAttachment> texAttachment);
			void AddDepthAttachment(Ref<FBODepthAttachment> depthAttachment);
			void AddStencilAttachment(Ref<FBOStencilAttachment> stencilAttachment);

			void RemoveAllAttachments();
			void RemoveAttachments(AttachmentBindPoint point);
			void RemoveAttachments(const Ref<AbstraFBOAttachment> &attachment);

		private:
			unsigned int handle;
			std::map<AttachmentBindPoint, Ref<AbstraFBOAttachment>> attachments;
		};
	}
}

#endif
