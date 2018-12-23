
#include "openglbuffer.h"

#include "../../lib/gl3w/GL/gl3w.h"
#include "error.h"

OpenGLBuffer::OpenGLBuffer(BufferTarget type,BufferUsage usage):
target(type),
usage(usage),
bufferId(0),
currentContext(OpenGLCurrentContext::GetCurrentOpenGLContext())
{
	if (!currentContext)
	{
		ysl::Error("No OpenGL Context");
	}
	glGenBuffers(1, &bufferId);
	Bind();
}



OpenGLBuffer::BufferTarget OpenGLBuffer::Target()const
{
	return target;
}

void OpenGLBuffer::Bind()
{
	glBindBuffer(target, bufferId);
}

void OpenGLBuffer::SetSize(std::size_t size)
{
	dataSize = size;
}

std::size_t OpenGLBuffer::Size() const
{
	return dataSize;
}

void OpenGLBuffer::AllocateFor(const void* data, std::size_t size)
{
	dataSize = size;
	Write(data);
}

void OpenGLBuffer::Write(const void* data)
{
	glBufferData(target, dataSize, data, usage);
}

void OpenGLBuffer::Unbind()
{
	glBindBuffer(target, 0);
}

bool OpenGLBuffer::ShaderStorageBufferBind(int index)
{
	if (target != ShaderStorageBuffer)
	{
		ysl::Warning("The target of buffer is wrong");
		return false;
	}
	Bind();
	glBindBufferBase(target, index, bufferId);

}

void * OpenGLBuffer::Map(Access access)
{
	Bind();
	const auto ptr = glMapBuffer(target, access);
	Unbind();
	return ptr;
}

void OpenGLBuffer::Unmap()
{
	Bind();
	glUnmapBuffer(target);
	Unbind();
}

OpenGLBuffer::~OpenGLBuffer()
{
	const auto ctx = OpenGLCurrentContext::GetCurrentOpenGLContext();
	if (!ctx || *currentContext != *ctx)
		ysl::Warning("The context is different from the one with which the resource is created");
	glDeleteBuffers(1, &bufferId);
}

//
//GLenum OpenGLBuffer::glEnumTarget() const
//{
//	if (m_target == BufferTarget::VertexArrayBuffer)
//		return GL_ARRAY_BUFFER;
//	if (m_target == BufferTarget::ElementArrayBuffer)
//		return GL_ELEMENT_ARRAY_BUFFER;
//	return 0;
//}
//
//GLenum OpenGLBuffer::glBufferUsage() const
//{
//	if (m_usage == BufferUsage::Dynamic)
//		return GL_DYNAMIC_DRAW;
//	if (m_usage == BufferUsage::Static)
//		return GL_STATIC_DRAW;
//	if (m_usage == BufferUsage::Stream)
//		return GL_STREAM_DRAW;
//	return 0;
//}
