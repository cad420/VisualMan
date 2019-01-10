
#include "openglbuffer.h"

#include "../../lib/gl3w/GL/gl3w.h"
#include "error.h"
#include "openglutils.h"

OpenGLBuffer::OpenGLBuffer(BufferTarget type,BufferUsage usage,BufferType typ):
target(type),
usage(usage),
type(typ),
bufferId(0),
dataSize(0),
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

std::size_t OpenGLBuffer::Size() const
{
	return dataSize;
}

void OpenGLBuffer::AllocateFor(const void* data, std::size_t size)
{
	if(type == Immutable && (!size && size != dataSize))
	{
		ysl::Warning("Buffer is immutable, size is different from previous one");
		return;
	}
	dataSize = size;
	Write(data);
}

void OpenGLBuffer::Write(const void* data)
{
	if(type == Mutable)
		glBufferData(target, dataSize, data, usage);
	else if(type == Immutable)
		glBufferStorage(target, dataSize, data,GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
	//std::cout << dataSize;
}

void OpenGLBuffer::Unbind()
{
	glBindBuffer(target, 0);
}

void OpenGLBuffer::BindBufferBase(int index)
{
	Bind();
	glBindBufferBase(target, index, bufferId);
}

void * OpenGLBuffer::Map(Access access)
{
	Bind();
	const auto ptr = glMapBuffer(target, access);
	return ptr;
}

void OpenGLBuffer::Unmap()
{
	Bind();
	glUnmapBuffer(target);
}

OpenGLBuffer::BufferType OpenGLBuffer::Type() const
{
	return type;
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
