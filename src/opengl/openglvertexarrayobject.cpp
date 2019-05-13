
#include "openglvertexarrayobject.h"
#include "../../lib/gl3w/GL/gl3w.h"

OpenGLVertexArrayObject::OpenGLVertexArrayObject():vao(0)
{
}

void OpenGLVertexArrayObject::create()
{
	glCreateVertexArrays(1, &vao);
}

void OpenGLVertexArrayObject::bind()
{
	if(!vao)
		return;
	glBindVertexArray(vao);
}

void OpenGLVertexArrayObject::unbind()
{
	glBindVertexArray(0);
}

void OpenGLVertexArrayObject::destroy()
{
	glDeleteVertexArrays(1,&vao);
}

