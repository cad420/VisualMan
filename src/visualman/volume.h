#ifndef _VOLUME_H_
#define _VOLUME_H_

#include <typeindex>
#include <memory>
#include <GL/glcorearb.h>
#include <VMat/geometry.h>
#include <VMFoundation/dataarena.h>

namespace ysl
{
namespace vm
{
enum VoxelFormat
{
	VF_RGB = GL_RGB,
	VF_RGBA = GL_RGBA,
	VF_BGR = GL_BGR,
	VF_BGRA = GL_BGRA,
	VF_RG = GL_RG,
	VF_RG_INTEGER = GL_RG_INTEGER,
	VF_RED = GL_RED,

	//VF_DEPTH_COMPONENT = GL_DEPTH_COMPONENT,
	//VF_STENCIL_INDEX = GL_STENCIL_INDEX,
	//VF_DEPTH_STENCIL = GL_DEPTH_STENCIL,

	VF_RED_INTEGER = GL_RED_INTEGER,
	VF_GREEN_INTEGER = GL_GREEN_INTEGER,
	VF_BLUE_INTEGER = GL_BLUE_INTEGER,
	VF_RGB_INTEGER = GL_RGB_INTEGER,
	VF_RGBA_INTEGER = GL_RGBA_INTEGER,
	VF_BGR_INTEGER = GL_BGR_INTEGER,
	VF_BGRA_INTEGER = GL_BGRA_INTEGER,
};

enum VoxelType
{
	VF_UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
	VF_BYTE = GL_BYTE,
	VF_UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
	VF_SHORT = GL_SHORT,
	VF_UNSIGNED_INT = GL_UNSIGNED_INT,
	VF_INT = GL_INT,
	VF_FLOAT = GL_FLOAT,
};

enum VolumeDimensionType
{

};

class Any
{
public:
	Any() :
	  m_tpIndex( std::type_index( typeid( void ) ) ) {}
	Any( const Any &other ) :
	  m_ptr( other.Clone() ), m_tpIndex( other.m_tpIndex ) {}
	Any( Any &&other ) noexcept :
	  m_ptr( std::move( other.m_ptr ) ), m_tpIndex( other.m_tpIndex ) {}

	template <typename U, class = typename std::enable_if<!std::is_same<typename std::decay<U>::type, Any>::value, U>::type>
	Any( U &&value ) :
	  m_ptr( new Derived<typename std::decay<U>::type>( std::forward<U>( value ) ) ),
	  m_tpIndex( std::type_index( typeid( typename std::decay<U>::type ) ) )
	{
	}

	bool IsNull() const { return !bool( m_ptr ); }

	template <class U>
	bool As() const
	{
		return m_tpIndex == std::type_index( typeid( U ) );
	}

	template <class U>

	U &AnyCast()
	{
		if ( !As<U>() ) {
			std::cout << "can not cast " << typeid( U ).name() << " to " << m_tpIndex.name() << std::endl;
			throw std::bad_cast();
		}

		auto derived = dynamic_cast<Derived<U> *>( m_ptr.get() );
		return derived->m_value;
	}

	Any &operator=( const Any &a )
	{
		if ( m_ptr == a.m_ptr )
			return *this;

		m_ptr = a.Clone();
		m_tpIndex = a.m_tpIndex;
		return *this;
	}

private:
	struct Base;
	typedef std::unique_ptr<Base> BasePtr;

	struct Base
	{
		virtual ~Base() {}
		virtual BasePtr Clone() const = 0;
	};

	template <typename T>
	struct Derived : Base
	{
		template <typename U>
		Derived( U &&value ) :
		  m_value( std::forward<U>( value ) )
		{
		}

		BasePtr Clone() const
		{
			return BasePtr( new Derived<T>( m_value ) );
		}

		T m_value;
	};

	BasePtr Clone() const
	{
		if ( m_ptr != nullptr )
			return m_ptr->Clone();

		return nullptr;
	}

	BasePtr m_ptr;
	std::type_index m_tpIndex;
};

class AbstraVolume
{
public:
	AbstraVolume( VoxelFormat format, VoxelType type ) :
	  format( format ), type( type ) {}
	AbstraVolume( const AbstraVolume & ) = delete;
	AbstraVolume &operator=( const AbstraVolume & ) = delete;
	AbstraVolume( AbstraVolume && ) = delete;
	AbstraVolume &operator=( AbstraVolume && ) = delete;
	VoxelFormat Format() const { return format; }
	VoxelType Type() const { return type; }
	virtual char *Voxel( int x, int y, int z ) = 0;
	virtual const char *Voxel( int x, int y, int z ) const = 0;
	virtual size_t Width() const = 0;
	virtual size_t Height() const = 0;
	virtual size_t Depth() const = 0;
	virtual const void *Data() const = 0;
	virtual void *Data() = 0;

private:
	VoxelFormat format;
	VoxelType type;
};

}  // namespace vm
}  // namespace ysl

#endif
