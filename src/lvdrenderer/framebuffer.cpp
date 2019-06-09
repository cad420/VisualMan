
#include "framebuffer.h"
#include <GL/gl3w.h>
#include <error.h>
#include "openglutils.h"

OpenGLFramebufferObject::OpenGLFramebufferObject():currentContext(OpenGLCurrentContext::GetCurrentOpenGLContext())
{
	glCreateFramebuffers(1, &framebufferObjectId);
}

void OpenGLFramebufferObject::AttachTexture(Attachment attach, const std::shared_ptr<OpenGLTexture>& texture,int level, int offset)
{
	//auto texTarget = texture->Target();
	//if (texTarget == OpenGLTexture::Texture1D)
	//	glFramebufferTexture1D(GL_FRAMEBUFFER, attach, texTarget, texture->NativeTextureId(), level);
	//if (texTarget == OpenGLTexture::Texture2D || texTarget == OpenGLTexture::Texture2DRect)
	//	glFramebufferTexture2D(GL_FRAMEBUFFER, attach, texTarget, texture->NativeTextureId(), level);
	//if(texTarget == OpenGLTexture::Texture3D)
	//	glFramebufferTexture3D(GL_FRAMEBUFFER,attach,texTarget,texture->NativeTextureId(),level,offset);
	glNamedFramebufferTexture(framebufferObjectId, attach, texture->NativeTextureId(),level);
	GL_ERROR_REPORT
}

void OpenGLFramebufferObject::UnattachedTexture(const std::shared_ptr<OpenGLTexture>& texture)
{

}

void OpenGLFramebufferObject::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferObjectId);
}

void OpenGLFramebufferObject::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebufferObject::SaveAsImage(const std::string& fileName, Attachment attach)
{

}

void OpenGLFramebufferObject::CheckFramebufferStatus()
{
	//Bind();
	const auto status = glCheckNamedFramebufferStatus(framebufferObjectId,GL_FRAMEBUFFER);
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
	Unbind();
}

OpenGLFramebufferObject::~OpenGLFramebufferObject()
{
	if (*currentContext != *OpenGLCurrentContext::GetCurrentOpenGLContext())
		ysl::Warning("Current context is different from the context which the object is created with");
	glDeleteFramebuffers(1, &framebufferObjectId);
}

