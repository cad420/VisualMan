
#ifndef _OPENGLBUFFER_H_
#define _OPENGLBUFFER_H_

#include <cstdlib>
#include "../../lib/gl3w/GL/glcorearb.h"

class OpenGLBuffer
{
public:
	// Disable copy semantics
	enum class BufferType{VertexArrayBuffer,ElementArrayBuffer};		// Only support these now.
	enum class BufferUsage{Static,Dynamic,Stream};

	OpenGLBuffer(BufferType type,BufferUsage usage = BufferUsage::Static);
	OpenGLBuffer(const OpenGLBuffer &) = delete;
	OpenGLBuffer& operator=(const OpenGLBuffer &) = delete;

	void create();
	void bind();
	void allocate(void * data,std::size_t size);
	void setUsage(BufferUsage usage);
	void write(void * data);
	void unbind();
	void destroy();
	~OpenGLBuffer()=default;
private:
	unsigned int m_buffer;
	BufferUsage m_usage;
	BufferType m_type;
	std::size_t m_dataSize;
	GLenum glEnumTarget() const;
	GLenum glBufferUsage() const;
};

#endif
