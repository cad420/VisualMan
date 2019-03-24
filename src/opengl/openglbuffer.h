
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
		PixelUnpackBuffer = GL_PIXEL_UNPACK_BUFFER,
		TextureBuffer = GL_TEXTURE_BUFFER
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
		ReadOnly = GL_READ_ONLY,
		WriteOnly = GL_WRITE_ONLY,
		ReadWrite = GL_READ_WRITE
	};

	enum BufferType
	{
		Mutable,
		Immutable
	};

	enum DataType
	{
		Float = GL_FLOAT,
	};


	explicit  OpenGLBuffer(BufferTarget type,BufferUsage usage = BufferUsage::StaticDraw, BufferType typ = Mutable);
	OpenGLBuffer(const OpenGLBuffer &) = delete;
	OpenGLBuffer& operator=(const OpenGLBuffer &) = delete;

	//void create();
	BufferTarget Target()const;
	void Bind();
	//void SetSize(std::size_t size);
	std::size_t Size()const;
	void AllocateFor(const void * data, std::size_t size);
	void SetSubData(const void * data, std::size_t size, std::size_t offsetBytes);
	void VertexAttribPointer(int index,std::size_t size,DataType type,bool normalized,std::size_t stride,void * pointer);

	//void setUsage(BufferUsage usage);
	void Write(const void* data);
	void Unbind();

	void BindBufferBase(int index);
	void * Map(Access access);
	void Unmap();
	BufferType Type() const;

	~OpenGLBuffer();
private:
	unsigned int bufferId;
	BufferUsage usage;
	BufferTarget target;
	BufferType type;
	std::size_t dataSize;

	std::shared_ptr<OpenGLCurrentContext> currentContext;


	friend class OpenGLTexture;

	//GLenum glEnumTarget() const;
	//GLenum glBufferUsage() const;
};

#endif
