
#include "openglvertexarrayobject.h"
#include <GL/gl3w.h>

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

