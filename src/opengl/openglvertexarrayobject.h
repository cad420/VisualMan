#ifndef _OPENGLVERTEXARRAYOBJECT_H_
#define _OPENGLVERTEXARRAYOBJECT_H_

class OpenGLVertexArrayObject
{
public:
	OpenGLVertexArrayObject();
	OpenGLVertexArrayObject(const OpenGLVertexArrayObject &) = delete;
	OpenGLVertexArrayObject & operator=(const OpenGLVertexArrayObject&) = delete;
	void create();
	void bind();
	void unbind();
	void destroy();
	~OpenGLVertexArrayObject() = default;
private:
	unsigned int m_vbo;
};

#endif
