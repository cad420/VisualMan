#ifndef _OPENGLCONTEXT_H_
#define _OPENGLCONTEXT_H_

#include "../../lib/glfw/include/GLFW/glfw3.h"
#include <memory>
#include "openglobject.h"


namespace ysl
{
	class GPUContext 
	{
	public:
		GPUContext();
		GPUContext(const GPUContext &) = delete;
		GPUContext & operator=(const GPUContext &) = delete;
		GPUContext(GPUContext &&)noexcept;
		GPUContext & operator=(GPUContext &&)noexcept;
		virtual bool IsValid()const = 0;
	};
}

class OpenGLCurrentContext
{
public:
	OpenGLCurrentContext();
	OpenGLCurrentContext(const OpenGLCurrentContext &) = delete;
	OpenGLCurrentContext & operator=(const OpenGLCurrentContext&) = delete;
	OpenGLCurrentContext(OpenGLCurrentContext &&)noexcept;
	OpenGLCurrentContext & operator=(OpenGLCurrentContext&&)noexcept;
	bool IsValid()const;
	bool operator==(const OpenGLCurrentContext & ctx)const;
	bool operator!=(const OpenGLCurrentContext & ctx)const;
	static std::shared_ptr<OpenGLCurrentContext> GetCurrentOpenGLContext();
private:
	GLFWwindow * windowContext;
};


#endif
