
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
			std::set<WeakRef<FramebufferObject>,std::owner_less<WeakRef<FramebufferObject>>> frambufferObjects;
		};


		class GRAPHICS_EXPORT_IMPORT FBORenderBufferAttachment:public AbstraFBOAttachment
		{
		public:
			~FBORenderBufferAttachment() { DestroyRenderBuffer(); }

			void CreateRenderBuffer();
			void DestroyRenderBuffer();

			void InitStorage(int w, int h, int samples);

			int Width()const { return width; }
			int Height()const { return height; }
			int Samples()const { return samples; }

			bool StorageChanged()const { return storageChanged; }

		protected:
			virtual int InternalType() = 0;
			void BindAttachment(Ref<FramebufferObject> fbo, AttachmentBindPoint point) override;

			bool storageChanged = false;
			unsigned int handle = 0;
			int width = 0;
			int height = 0;
			int samples = 0;
			

		};

		class GRAPHICS_EXPORT_IMPORT FBOColorBufferAttachment:public FBORenderBufferAttachment
		{
			friend class FramebufferObject;
		public:
			FBOColorBufferAttachment(ColorBufferFormat type = CBF_RGBA):type(type) {}
			ColorBufferFormat Type()const { return type; }
			void SetType(ColorBufferFormat type) { if (this->type != type) { this->type = type; storageChanged = true; } }
		protected:
			int InternalType() override { return type; }
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
			void SetMipMapLevel(int mlevel) { mipMapLevel = mlevel; }
			int GetMipMapLevel()const { return mipMapLevel; }
		protected:
			Ref<Texture> texture;
			int mipMapLevel = 0;
		};

		class GRAPHICS_EXPORT_IMPORT FBOTextureAttachment:public AbstraFBOTextureAttachment
		{
		public:
			FBOTextureAttachment(Ref<Texture> texture):AbstraFBOTextureAttachment(std::move(texture))
			{
			}
			void SetTexture2DTarget(Texture2DTarget target) { this->target = target; }
			Texture2DTarget GetTexture2DTarget()const { return target; }

		protected:
			void BindAttachment(Ref<FramebufferObject> fbo, AttachmentBindPoint point) override;
			Texture2DTarget target = T2DT_TEXTURE_2D;
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

			void CreateFrambufferObject();

			void DestroyFramebufferObject();

			void CheckFramebufferStatus();

			void AddColorAttachment(AttachmentBindPoint point,Ref<FBOColorBufferAttachment> colorAttachment);

			void AddTextureAttachment(AttachmentBindPoint point, Ref<AbstraFBOTextureAttachment> texAttachment);

			void AddDepthAttachment(Ref<FBODepthAttachment> depthAttachment);

			void AddStencilAttachment(Ref<FBOStencilAttachment> stencilAttachment);

			void AddDepthStencilAttachment(Ref<FBODepthStencilAttachment> dsAttachment);

			void RemoveAllAttachments();

			void RemoveAttachments(AttachmentBindPoint point);

			void RemoveAttachments(const Ref<AbstraFBOAttachment> &attachment);

			~FramebufferObject() { DestroyFramebufferObject(); }

		private:
			unsigned int handle = 0;
			std::map<AttachmentBindPoint, Ref<AbstraFBOAttachment>> attachments;
		};
	}
}

#endif

