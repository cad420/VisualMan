
#ifndef _OPENGLBUFFER_H_
#define _OPENGLBUFFER_H_

#include <cstdlib>
#include "../../lib/gl3w/GL/glcorearb.h"
#include <memory>
#include "openglcontext.h"
#include "texture.h"

class OpenGLBuffer
{
public:
	// Disable copy semantics
	enum  BufferTarget
	{
		VertexArrayBuffer = GL_ARRAY_BUFFER,
		ElementArrayBuffer = GL_ELEMENT_ARRAY_BUFFER, 
		ShaderStorageBuffer = GL_SHADER_STORAGE_BUFFER,
		AtomicCounterBuffer = GL_ATOMIC_COUNTER_BUFFER,
		PixelUnpackBuffer = GL_PIXEL_UNPACK_BUFFER
	};		
	// Only support these now.
	enum  BufferUsage
	{
		StaticDraw = GL_STATIC_DRAW,
		DynamicDraw = GL_DYNAMIC_DRAW,
		StreamDraw = GL_STREAM_DRAW,
		StaticCopy = GL_STATIC_COPY,
		DynamicCopy = GL_DYNAMIC_COPY,
		StreamCopy = GL_STREAM_COPY
	};

	enum Access
	{
		ReadOnly= GL_READ_ONLY,
		WriteOnly = GL_WRITE_ONLY,
		ReadWrite = GL_READ_WRITE
	};

	explicit  OpenGLBuffer(BufferTarget type,BufferUsage usage = BufferUsage::StaticDraw);
	OpenGLBuffer(const OpenGLBuffer &) = delete;
	OpenGLBuffer& operator=(const OpenGLBuffer &) = delete;

	//void create();
	BufferTarget Target()const;
	void Bind();
	void SetSize(std::size_t size);
	std::size_t Size()const;
	void AllocateFor(const void* data, std::size_t size);
	//void setUsage(BufferUsage usage);
	void Write(const void* data);
	void Unbind();

	void BindBufferBase(int index);
	void * Map(Access access);
	void Unmap();

	~OpenGLBuffer();
private:
	unsigned int bufferId;
	BufferUsage usage;
	BufferTarget target;
	std::size_t dataSize;

	std::shared_ptr<OpenGLCurrentContext> currentContext;

	//GLenum glEnumTarget() const;
	//GLenum glBufferUsage() const;
};

#endif
