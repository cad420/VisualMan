#ifndef _OPENGLCONTEXT_H_
#define _OPENGLCONTEXT_H_

#include "../../lib/glfw/include/GLFW/glfw3.h"
#include <memory>
#include "openglobject.h"



class GPUContext:public ysl::GPUObject
{
public:
	GPUContext();
};

class OpenGLCurrentContext:public ysl::GPUObject
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
	uint64_t MemoryUsage() override;
	std::string ToString() const override;
	static std::shared_ptr<OpenGLCurrentContext> GetCurrentOpenGLContext();
private:
	GLFWwindow * windowContext;
};

#endif
