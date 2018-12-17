
#include "openglbuffer.h"

#include "openglutils.h"

#include "../../lib/gl3w/GL/gl3w.h"

OpenGLBuffer::OpenGLBuffer(BufferType type,BufferUsage usage):
m_type(type),
m_usage(usage),
m_buffer(0)
{

}

void OpenGLBuffer::create()
{
	glGenBuffers(1, &m_buffer);
}

void OpenGLBuffer::bind()
{
	glBindBuffer(glEnumTarget(), m_buffer);
}

void OpenGLBuffer::allocate(void* data, std::size_t size)
{
	m_dataSize = size;
	write(data);
}

void OpenGLBuffer::setUsage(BufferUsage usage)
{
	m_usage = usage;
}

void OpenGLBuffer::write(void * data)
{
	glBufferData(glEnumTarget(), m_dataSize, data,glBufferUsage());
}

void OpenGLBuffer::unbind()
{
	glBindBuffer(glEnumTarget(), 0);
}

void OpenGLBuffer::destroy()
{
	glDeleteBuffers(1, &m_buffer);
}


GLenum OpenGLBuffer::glEnumTarget() const
{
	if (m_type == BufferType::VertexArrayBuffer)
		return GL_ARRAY_BUFFER;
	if (m_type == BufferType::ElementArrayBuffer)
		return GL_ELEMENT_ARRAY_BUFFER;
	return 0;
}

GLenum OpenGLBuffer::glBufferUsage() const
{
	if (m_usage == BufferUsage::Dynamic)
		return GL_DYNAMIC_DRAW;
	if (m_usage == BufferUsage::Static)
		return GL_STATIC_DRAW;
	if (m_usage == BufferUsage::Stream)
		return GL_STREAM_DRAW;
	return 0;
}
