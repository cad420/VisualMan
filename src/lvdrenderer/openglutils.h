/*
   The MIT License

   Copyright (c) 2019


   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/


#ifndef _OPENGLUTILS_H_
#define _OPENGLUTILS_H_
#include <GL/gl3w.h>
#include <error.h>

inline
void PrintGLErrorType(GLenum glerr)
{
	std::string error;
	switch (glerr)
	{
	case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
	case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
	case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
	case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
	case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
	case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
	case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
	default:							   error = "UNKNOWN_ERROR"; break;
	}
	ysl::Warning("%s", error.c_str());
}

inline
GLenum PrintGLErrorMsg(const char * file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		ysl::Warning("%s | %s (%d)", error.c_str(), file, line);
	}
	return errorCode;
}

#ifdef NDEBUG
#define GL_ERROR_REPORT void(0);
#define GL_ERROR_ASSERT	void(0);
#define GL(stmt) stmt;
#define GL_CHECK void(0);
#else
#define GL_ERROR_REPORT								PrintGLErrorMsg(__FILE__, __LINE__);
	//{												\
	//	GLenum err;									\
	//	while((err = glGetError()) != GL_NO_ERROR)	\
	//	{											\
	//		ysl::Warning("OpenGL Error Code:%d. File:%s, Line:%d. \n",err,__FILE__,__LINE__);\
	//	}											\
	//}
	

#define GL_ERROR_ASSERT														\
		assert(glGetError() == GL_NO_ERROR);								\

#define GL(stmt)															\
 do {																		\
    GLenum glerr;															\
    unsigned int iCounter = 0;												\
    while((glerr = glGetError()) != GL_NO_ERROR) {							\
      ysl::Warning("GL error calling %s before line %u (%s): (%x)",     \
              #stmt, __LINE__, __FILE__,									\
              static_cast<unsigned>(glerr));								\
	  PrintGLErrorType(glerr);												\
      iCounter++;															\
      if (iCounter > 100) break;											\
    }																		\
    stmt;																	\
    iCounter = 0;															\
    while((glerr = glGetError()) != GL_NO_ERROR) {							\
      ysl::Warning("'%s' on line %u (%s) caused GL error: (%d)", #stmt, \
              __LINE__, __FILE__,											\
              static_cast<unsigned>(glerr));								\
	  PrintGLErrorType(glerr);												\
      iCounter++;															\
      if (iCounter > 100) break;											\
    }																		\
  } while(0);

#define GL_CHECK	\
 do {																		\
    GLenum glerr;															\
    unsigned int iCounter = 0;												\
    while((glerr = glGetError()) != GL_NO_ERROR) {							\
      ysl::Warning("before line %u (%s):(%#x)",							\
              __LINE__, __FILE__,											\
              static_cast<unsigned>(glerr));								\
	  PrintGLErrorType(glerr);												\
      iCounter++;															\
      if (iCounter > 100) break;											\
    }																		\
    iCounter = 0;															\
    while((glerr = glGetError()) != GL_NO_ERROR) {							\
      ysl::Warning(" on line %u (%s) caused GL error:(%#x)",			\
              __LINE__, __FILE__,											\
              static_cast<unsigned>(glerr));								\
	  PrintGLErrorType(glerr);												\
      iCounter++;															\
      if (iCounter > 100) break;											\
    }																		\
  } while(0);


#endif /*NDBUG*/




#endif /*_OPENGLUTILS_H_*/