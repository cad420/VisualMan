
#ifndef _GRAPHICTYPE_H_
#define _GRAPHICTYPE_H_


#define USE_SMART_POINTER


#define Ref(typeName) std::shared_ptr<typeName>

#ifdef USE_SMART_POINTER


#define SharedPtr(typeName) std::shared_ptr<typeName>
#define RSharedPtr(typeName) std::shared_ptr<typeName> &
#define CSharedPtr(typeName) std::shared_ptr<const typeName>
#define CRSharedPtr(typeName) const std::shared_ptr<typeName> &

#define UniquePtr(typeName) std::unique_ptr<typeName>
#define RUniquePtr(typeName) std::unique_ptr<typeName> &
#define CUniquePtr(typeName) std::unique_ptr<const typeName>
#define CRUniquePtr(typeName) const std::unique_ptr<typeName> &

#define WeakPtr(typeName) std::weak_ptr<typeName>
#define RWeakPtr(typeName) std::weak_ptr<typeName> &
#define CWeakPtr(typeName) std::weak_ptr<const typeName>
#define CRWeakPtr(typeName) const std::weak<typeName> &

#define GetPtr(ptr) (ptr.get())

#elif

#define SharedPtr(typeName) typeName*
#define RSharedPtr(typeName) typeName*
#define CSharedPtr(typeName) const typeName*
#define CRSharedPtr(typeName) const typeName*

#define UniquePtr(typeName) typeName*
#define RUniquePtr(typeName) typeName*
#define CUniquePtr(typeName) const typeName*
#define CRUniquePtr(typeName) const typeName*

#define WeakPtr(typeName) typeName*
#define RWeakPtr(typeName) typeName*
#define CWeakPtr(typeName) const typeName*
#define CRWeakPtr(typeName) const typeName*

#define GetPtr(ptr) (ptr)

#endif

#include <memory>
#include "../../lib/gl3w/GL/glcorearb.h"


#if defined(MRE_EXPORT_DLL)
#define GRAPHICS_EXPORT_IMPORT __declspec(dllexport)
#else
#define GRAPHICS_EXPORT_IMPORT __declspec(dllimport)
#endif



template<typename Ty>
using Ref = std::shared_ptr<Ty>;

template<typename Ty>
using WeakRef = std::weak_ptr<Ty>;

template<typename Ty>
using ExclRef = std::unique_ptr<Ty>;




// Enum for OpenGL

enum Func
{
	FU_NEVER = GL_NEVER,
	FU_LESS = GL_LESS,
	FU_EQUAL = GL_EQUAL,
	FU_LEQUAL = GL_LEQUAL,
	FU_GREATER = GL_GREATER,
	FU_NOTEQUAL = GL_NOTEQUAL,
	FU_GEQUAL = GL_GEQUAL,
	FU_ALWAYS = GL_ALWAYS
};

enum BufferObjectUsage
{
	BU_STREAM_DRAW = GL_STREAM_DRAW, 
	BU_STREAM_READ = GL_STREAM_READ,  
	BU_STREAM_COPY = GL_STREAM_COPY, 
	BU_STATIC_DRAW = GL_STATIC_DRAW, 
	BU_STATIC_READ = GL_STATIC_READ,  
	BU_STATIC_COPY = GL_STATIC_COPY, 
	BU_DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
	BU_DYNAMIC_READ = GL_DYNAMIC_READ,
	BU_DYNAMIC_COPY = GL_DYNAMIC_COPY 
};

enum BufferObjectAccess
{
	READ_ONLY = GL_READ_ONLY,
	WRITE_ONLY = GL_WRITE_ONLY,
	READ_WRITE = GL_READ_WRITE
};


#endif