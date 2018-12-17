
#include "openglvertexarrayobject.h"
#include "../../lib/gl3w/GL/gl3w.h"

OpenGLVertexArrayObject::OpenGLVertexArrayObject():m_vbo(0)
{
}

void OpenGLVertexArrayObject::create()
{
	glGenVertexArrays(1, &m_vbo);
}

void OpenGLVertexArrayObject::bind()
{
	if(!m_vbo)
		return;
	glBindVertexArray(m_vbo);
}

void OpenGLVertexArrayObject::unbind()
{
	glBindVertexArray(0);
}

void OpenGLVertexArrayObject::destroy()
{
	glDeleteVertexArrays(1,&m_vbo);
}

