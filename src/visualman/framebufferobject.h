
#ifndef _FRAMEBUFFEROBJECT_H_
#define _FRAMEBUFFEROBJECT_H_

#include "graphictype.h"
#include <set>
#include "framebuffer.h"
#include "texture.h"

namespace vm
{
class FramebufferObject;

class VISUALMAN_EXPORT_IMPORT AbstraFBOAttachment : public std::enable_shared_from_this<AbstraFBOAttachment>
{
	friend class FramebufferObject;

public:
	AbstraFBOAttachment() = default;
	virtual ~AbstraFBOAttachment() = default;
	AbstraFBOAttachment( const AbstraFBOAttachment & ) = delete;
	AbstraFBOAttachment &operator=( const AbstraFBOAttachment & ) = delete;
	void DetachFromAllFBO();

protected:
	virtual void BindAttachment( VMRef<FramebufferObject> fbo, AttachmentBindPoint point ) = 0;
	std::set<WeakRef<FramebufferObject>, std::owner_less<WeakRef<FramebufferObject>>> frambufferObjects;
};

/**
		 * \brief 
		 */
class VISUALMAN_EXPORT_IMPORT FBORenderBufferAttachment : public AbstraFBOAttachment
{
	friend class FramebufferObject;

public:
	~FBORenderBufferAttachment() { DestroyRenderBuffer(); }
	void CreateRenderBuffer();
	void DestroyRenderBuffer();
	void InitStorage( int w, int h, int samples );
	int Width() const { return width; }
	int Height() const { return height; }
	int Samples() const { return samples; }
	bool StorageChanged() const { return storageChanged; }

protected:
	virtual int InternalType() = 0;
	void BindAttachment( VMRef<FramebufferObject> fbo, AttachmentBindPoint point ) override;

	bool storageChanged = false;
	unsigned int handle = 0;
	int width = 0;
	int height = 0;
	int samples = 0;
};

/**
		 * \brief This is a render attachment
		 */
class VISUALMAN_EXPORT_IMPORT FBOColorBufferAttachment : public FBORenderBufferAttachment
{
	friend class FramebufferObject;

public:
	FBOColorBufferAttachment( ColorBufferFormat type = CBF_RGBA ) :
	  type( type ) {}
	ColorBufferFormat Type() const { return type; }
	void SetType( ColorBufferFormat type )
	{
		if ( this->type != type ) {
			this->type = type;
			storageChanged = true;
		}
	}

protected:
	int InternalType() override { return type; }

private:
	ColorBufferFormat type = CBF_RGBA;
};

/**
		 * \brief 
		 */
class VISUALMAN_EXPORT_IMPORT FBODepthAttachment : public FBORenderBufferAttachment
{
	friend class FramebufferObject;

private:
	DepthBufferFormat format = DBF_DEPTH_COMPONENT24;

protected:
	int InternalType() override { return format; }

public:
	FBODepthAttachment( DepthBufferFormat format ) :
	  format( format ){};
	DepthBufferFormat Type() const { return format; }
	void SetFormat( DepthBufferFormat format ) { this->format = format; }
};

/**
		 * \brief 
		 */
class VISUALMAN_EXPORT_IMPORT FBOStencilAttachment : public FBORenderBufferAttachment
{
	friend class FramebufferObject;

public:
	FBOStencilAttachment( StencilBufferFormat type ) :
	  type( type ) {}
	StencilBufferFormat Type() const { return type; }
	void SetType( StencilBufferFormat type ) { this->type = type; }

protected:
	int InternalType() override { return type; }

private:
	StencilBufferFormat type = SBF_STENCIL_INDEX8;
};

/**
		 * \brief 
		 */
class VISUALMAN_EXPORT_IMPORT FBODepthStencilAttachment : public FBORenderBufferAttachment
{
	friend class FramebufferObject;

public:
	FBODepthStencilAttachment() = default;
	FBODepthStencilAttachment( DepthStencilBufferFormat type ) :
	  type( type ) {}
	DepthStencilBufferFormat Type() const { return type; }
	void SetType( DepthStencilBufferFormat type ) { this->type = type; }

protected:
	int InternalType() override { return type; }

private:
	DepthStencilBufferFormat type = DSBT_DEPTH24_STENCIL8;
};

/**
		 * \brief 
		 */
class VISUALMAN_EXPORT_IMPORT AbstraFBOTextureAttachment : public AbstraFBOAttachment
{
	friend class FramebufferObject;

public:
	AbstraFBOTextureAttachment( VMRef<Texture> texture ) :
	  texture( std::move( texture ) ) {}
	void SetTexture( VMRef<Texture> texture ) { this->texture = std::move( texture ); }
	VMRef<Texture> GetTexture() { return texture; }
	void SetMipMapLevel( int mlevel ) { mipMapLevel = mlevel; }
	int GetMipMapLevel() const { return mipMapLevel; }

protected:
	VMRef<Texture> texture;
	int mipMapLevel = 0;
};

/**
		 * \brief 
		 */
class VISUALMAN_EXPORT_IMPORT FBOTextureAttachment : public AbstraFBOTextureAttachment
{
	friend class FramebufferObject;

public:
	FBOTextureAttachment( VMRef<Texture> texture ) :
	  AbstraFBOTextureAttachment( std::move( texture ) )
	{
	}
	void SetTexture2DTarget( Texture2DTarget target ) { this->target = target; }
	Texture2DTarget GetTexture2DTarget() const { return target; }

protected:
	void BindAttachment( VMRef<FramebufferObject> fbo, AttachmentBindPoint point ) override;
	Texture2DTarget target = T2DT_TEXTURE_2D;
};

/**
		 * \brief 
		 */
class VISUALMAN_EXPORT_IMPORT FramebufferObject : public Framebuffer
{
public:
	FramebufferObject() :
	  Framebuffer( nullptr, 0, 0, ReadDrawBuffer::RDB_COLOR_ATTACHMENT0, ReadDrawBuffer::RDB_COLOR_ATTACHMENT0 )
	{
	}

	FramebufferObject( RenderContext *context,
					   int width,
					   int height,
					   ReadDrawBuffer readBuffer,
					   ReadDrawBuffer drawBuffer ) :
	  Framebuffer( context, width, height, readBuffer, drawBuffer )
	{
	}

	void BindFramebuffer( FramebufferBind target ) override;

	void CreateFrambufferObject();

	void DestroyFramebufferObject();

	int CheckFramebufferStatus();

	void AddColorAttachment( AttachmentBindPoint point, VMRef<FBOColorBufferAttachment> colorAttachment );

	void AddTextureAttachment( AttachmentBindPoint point, VMRef<AbstraFBOTextureAttachment> texAttachment );

	void AddDepthAttachment( VMRef<FBODepthAttachment> depthAttachment );

	void AddStencilAttachment( VMRef<FBOStencilAttachment> stencilAttachment );

	void AddDepthStencilAttachment( VMRef<FBODepthStencilAttachment> dsAttachment );

	void RemoveAllAttachments();

	void RemoveAttachments( AttachmentBindPoint point );

	void RemoveAttachments( const VMRef<AbstraFBOAttachment> &attachment );

	unsigned Handle() const override { return handle; }

	~FramebufferObject() { DestroyFramebufferObject(); }

private:
	unsigned int handle = 0;

	std::map<AttachmentBindPoint, VMRef<AbstraFBOAttachment>> attachments;
};
}  // namespace vm

#endif
