
#include "openglcontext.h"



OpenGLCurrentContext::OpenGLCurrentContext() :
	windowContext(glfwGetCurrentContext())
{
}

OpenGLCurrentContext::OpenGLCurrentContext(OpenGLCurrentContext&& ctx) noexcept
{
	windowContext = ctx.windowContext;
	ctx.windowContext = nullptr;
}

OpenGLCurrentContext& OpenGLCurrentContext::operator=(OpenGLCurrentContext&&ctx) noexcept
{
	windowContext = ctx.windowContext;
	ctx.windowContext = nullptr;
	return *this;
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

//uint64_t OpenGLCurrentContext::MemoryUsage()
//{
//
//}
//
//std::string OpenGLCurrentContext::ToString() const
//{
//	return "OpenGLCurrentContext";
//}

std::shared_ptr<OpenGLCurrentContext> OpenGLCurrentContext::GetCurrentOpenGLContext()
{
	return std::make_shared<OpenGLCurrentContext>();
}
