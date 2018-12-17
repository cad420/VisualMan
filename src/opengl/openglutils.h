#ifndef _OPENGLUTILS_H_
#define _OPENGLUTILS_H_

#include "../utility/error.h"

#ifdef NDEBUG
#define GL_ERROR_REPORT(str) (void(0))
#define GL_ERROR_ASSERT	(void(0))
#else
#define GL_ERROR_REPORT								\
	{												\
		GLenum err;									\
		while((err = glGetError()) != GL_NO_ERROR)	\
		{											\
			ysl::Warning("OpenGL Error Code:%d. File:%s, Line:%d. \n",err,__FILE__,__LINE__);\
		}											\
	}

#define GL_ERROR_ASSERT							\
		assert(glGetError() == GL_NO_ERROR);	\

#endif /*NDBUG*/

#endif /*_OPENGLUTILS_H_*/