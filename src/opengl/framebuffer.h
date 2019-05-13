
#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_
#include "../../lib/gl3w/GL/glcorearb.h"
#include <memory>
#include "texture.h"
#include <unordered_set>


class OpenGLFramebufferObject
{
public:
	enum FramebufferTarget
	{
		Framebuffer = GL_FRAMEBUFFER
	};

	enum Attachment
	{
		DepthAttachment = GL_DEPTH_ATTACHMENT,
		StencilAttachment = GL_STENCIL_ATTACHMENT,
		DepthAndStencilAttachment = GL_DEPTH_STENCIL_ATTACHMENT,
		ColorAttachment0 = GL_COLOR_ATTACHMENT0,
		ColorAttachment1 = GL_COLOR_ATTACHMENT1,
		ColorAttachment2 = GL_COLOR_ATTACHMENT2,
		ColorAttachment3 = GL_COLOR_ATTACHMENT3,
		ColorAttachment4 = GL_COLOR_ATTACHMENT4,
		ColorAttachment5 = GL_COLOR_ATTACHMENT5,
		ColorAttachment6 = GL_COLOR_ATTACHMENT6,
		ColorAttachment7 = GL_COLOR_ATTACHMENT7,
		ColorAttachment8 = GL_COLOR_ATTACHMENT8,
	};

	OpenGLFramebufferObject();
	OpenGLFramebufferObject(const OpenGLFramebufferObject &) = delete;
	OpenGLFramebufferObject& operator=(const OpenGLFramebufferObject &) = delete;
	OpenGLFramebufferObject(OpenGLFramebufferObject &&) = delete;
	OpenGLFramebufferObject& operator=(OpenGLFramebufferObject &&) = delete;

	void AttachTexture(Attachment attach ,const std::shared_ptr<OpenGLTexture> & texture,int level = 0,int offset = 0);
	void UnattachedTexture(const std::shared_ptr<OpenGLTexture> & texture);
	void Bind();
	void Unbind();
	void SaveAsImage(const std::string & fileName, Attachment attach);
	void CheckFramebufferStatus();
	~OpenGLFramebufferObject();

private:
	unsigned int framebufferObjectId;
	std::shared_ptr<OpenGLCurrentContext> currentContext;
	std::unordered_map<Attachment, std::shared_ptr<OpenGLTexture>> attachedTextures;
};
#endif
