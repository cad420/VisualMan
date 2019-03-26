
#ifndef _LINEARARRAY_H_
#define _LINEARARRAY_H_


#include "../mathematics/geometry.h"
#include "dataarena.h"
#include <cstring>

namespace ysl
{
	template<typename T,int nCacheLine = 64>
	class Linear2DArray
	{
		using size_type = std::size_t;
		Size2 size;
		T * m_data;
		bool m_own;
	public:

		Linear2DArray():size(0,0),m_data(nullptr),m_own(false)
		{
		}

		Linear2DArray(size_type x,
			size_type y,
			T * data, 
			bool own):size(x,y),m_data(data),m_own(own)
		{
		}

		Linear2DArray(size_type x,size_type y,const T * data):Linear2DArray(x,y,nullptr,true)
		{
			m_data = (T*)AllocAligned<T>(x*y);
			if(m_data == nullptr)
			{
				std::cout << "Bad Alloc\n";
				return;
			}
			if(data)memcpy(m_data, data, x*y);
		}

		Linear2DArray(const Size2 & size, const T * data) :Linear2DArray(size.x, size.y, data)
		{
		}

		Linear2DArray(const Linear2DArray & array):Linear2DArray(array.size.x, array.size.y, array.m_data, array.m_own)
		{
			memcpy(m_data, array.m_data, sizeof(T) * size.x * size.y);
		}
		Linear2DArray & operator=(const Linear2DArray & array)
		{
			this->size = array.size;
			this->m_own = m_own;
			memcpy(m_data, array.m_data, sizeof(T) * size.x * size.y);
			return *this;
		}

		Linear2DArray(Linear2DArray && array)noexcept:
		Linear2DArray(array.size.x,array.size.y,array.m_data,array.m_own)
		{
			array.m_data = nullptr;
		}

		Linear2DArray & operator=(Linear2DArray && array)noexcept
		{
			size = array.size;
			m_data = array.m_data;
			m_own = array.m_own;
			array.m_data = nullptr;
			return *this;
		}

		Size2 Size()const { return size; }
		std::size_t Count()const { return size.x*size.y; }

		T & operator()(int x,int y)
		{
			return const_cast<T&>(static_cast<const Linear2DArray &>(*this)(x,y));
		}

		const T & operator()(int x,int y)const
		{
			return m_data[y * size.x + x];
		}

		~Linear2DArray()
		{
			if (m_own)
				FreeAligned(m_data);
		}

		const T * Data()const { return m_data;}
		T * Data() { return m_data; }

	};

	template<typename T, int nCacheLine = 64>
	class Linear3DArray
	{
		using size_type = std::size_t;
		T * m_data;
		Size3 size;
		size_type xyPlaneSize;
		bool own;
	public:

		Linear3DArray():size(0,0,0),m_data(nullptr),own(false),xyPlaneSize(0)
		{
		}

		Linear3DArray(size_type x,
			size_type y,
			size_type z,
			T * data,
			bool own):size(x,y,z),m_data(data),own(own),xyPlaneSize(x*y)
		{
		}

		Linear3DArray(size_type x, size_type y,size_type z ,const T * data) :Linear3DArray(x,y,z, nullptr, true)
		{
			const auto t = x*y*z;
			m_data = static_cast<T*>(AllocAligned<T>(t));
			if (m_data == nullptr)
			{
				std::cout << "Bad Alloc\n";
				return;
			}
			if(data)
				memcpy(m_data, data,t);
		}

		Linear3DArray(const Size3 & sze,
			T * data) :Linear3DArray(sze.x, sze.y, sze.z, data)
		{
		}

		Linear3DArray(const Linear3DArray & array):Linear3DArray(array.size.x, array.size.y, array.size.z, array.m_data, array.own)
		{
			memcpy(m_data, array.m_data, sizeof(T) * xyPlaneSize * size.z);
		}
		Linear3DArray & operator=(const Linear3DArray &array)
		{
			this->size = array.size;
			this->xyPlaneSize = array.xyPlaneSize;
			this->own = array.own;
			memcpy(m_data, array.m_data, sizeof(T) * xyPlaneSize * size.z);
			return *this;
		}
		Linear3DArray(Linear3DArray && array)noexcept :
			Linear3DArray(array.size.x, array.size.y,array.size.z, array.m_data, array.own)
		{
			array.m_data = nullptr;

			std::cout << "Linear3DArray(Linear3DArray && array)" << std::endl;
		}

		Linear3DArray & operator=(Linear3DArray && array)noexcept
		{

			size = array.size;
			m_data = array.m_data;
			own = array.own;
			array.m_data = nullptr;
			return *this;
		}

		Size3 Size()const
		{
			return size;
		}

		std::size_t Count()const
		{
			return xyPlaneSize * size.z;
		}

		T & operator()(int x, int y,int z)
		{
			return const_cast<T&>(static_cast<const Linear3DArray &>(*this)(x, y,z));
		}

		const T & operator()(int x, int y,int z)const
		{
			return m_data[z*xyPlaneSize +y * size.x + x];
		}

		const T * Data()const
		{
			return m_data;
		}

		T * Data()
		{
			return m_data;
		}

		~Linear3DArray()
		{
			if (own)
				FreeAligned(m_data);
		}
	};
}

#endif /*_LINEARARRAY_H_*/