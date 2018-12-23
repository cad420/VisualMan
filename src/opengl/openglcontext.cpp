
#include "openglcontext.h"

OpenGLCurrentContext::OpenGLCurrentContext():
windowContext(glfwGetCurrentContext())
{
}

bool OpenGLCurrentContext::IsValid() const
{
	return windowContext;
}

bool OpenGLCurrentContext::operator==(const OpenGLCurrentContext& ctx) const
{
	return ctx.windowContext == windowContext;
}

bool OpenGLCurrentContext::operator!=(const OpenGLCurrentContext& ctx) const
{
	return !(ctx == *this);
}

std::shared_ptr<OpenGLCurrentContext> OpenGLCurrentContext::GetCurrentOpenGLContext()
{
	return std::make_shared<OpenGLCurrentContext>();
}

