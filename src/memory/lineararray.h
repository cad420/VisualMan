
#ifndef _LINEARARRAY_H_
#define _LINEARARRAY_H_

#include "dataarena.h"

namespace ysl
{
	template<typename T,int nCacheLine = 64>
	class Linear2DArray
	{
		using size_type = std::size_t;
		T * m_data;
		const size_type m_nx, m_ny;
		bool m_own;
	public:
		Linear2DArray(size_type x,
			size_type y,
			T * data, 
			bool own):m_nx(x),m_ny(y),m_data(data),m_own(own)
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

		Linear2DArray(const Linear2DArray &) = delete;
		Linear2DArray & operator=(const Linear2DArray &) = delete;

		Linear2DArray(Linear2DArray && array)noexcept:
		Linear2DArray(array.m_nx,array.m_ny,array.m_data,array.m_own)
		{
			array.m_data = nullptr;
		}

		Linear2DArray & operator=(Linear2DArray && array)noexcept
		{
			m_nx = array.m_nx;
			m_ny = array.m_ny;
			m_data = array.m_data;
			m_own = array.m_own;
			array.m_data = nullptr;
			return *this;
		}

		int Width()const { return m_nx; }
		int Height()const { return m_ny; }
		std::size_t Count()const { return m_nx*m_nx; }

		T & operator()(int x,int y)
		{
			return const_cast<T&>(static_cast<const Linear2DArray &>(*this)(x,y));
		}

		const T & operator()(int x,int y)const
		{
			return m_data[y * m_nx + x];
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
		//const size_type m_nx, m_ny, m_nz;
		const ysl::Size3 size;
		const size_type m_xy;
		bool own;
	public:
		Linear3DArray(size_type x,
			size_type y,
			size_type z,
			T * data,
			bool own) :size(x,y,z),m_data(data),own(own),m_xy(x*y)
		{
		}

		Linear3DArray(const ysl::Size3 & sze,
			T * data
			):Linear3DArray(sze.x, sze.y ,sze.z, data,true)
		{

		}

		Linear3DArray(size_type x, size_type y,size_type z ,const T * data) :Linear3DArray(x,y,z, nullptr, true)
		{
			const auto t = x*y*z;
			m_data = (T*)AllocAligned<T>(t);
			if (m_data == nullptr)
			{
				std::cout << "Bad Alloc\n";
				return;
			}
			if(data)memcpy(m_data, data,t);
		}

		Linear3DArray(const Linear3DArray &) = delete;
		Linear3DArray & operator=(const Linear3DArray &) = delete;

		Linear3DArray(Linear3DArray && array)noexcept :
			Linear3DArray(array.size.x, array.size.y,array.size.z, array.m_data, array.own)
		{
			array.m_data = nullptr;
		}

		Linear3DArray & operator=(Linear3DArray && array)noexcept
		{
			//m_nx = array.m_nx;
			//m_ny = array.m_ny;
			//m_nx = array.m_nz;
			size = array.size;
			m_data = array.m_data;
			own = array.own;
			array.m_data = nullptr;
			return *this;
		}

		//int Width()const { return m_nx; }
		//int Height()const { return m_ny; }
		//int Depth()const { return m_nz; }

		ysl::Size3 Size()const { return size; }
		

		std::size_t Count()const { return size.x*size.y*size.z; }

		T & operator()(int x, int y,int z)
		{
			return const_cast<T&>(static_cast<const Linear3DArray &>(*this)(x, y,z));
		}

		const T & operator()(int x, int y,int z)const
		{
			return m_data[z*m_xy +y * size.x + x];
		}

		const T * Data()const { return m_data; }
		T * Data() { return m_data; }

		~Linear3DArray()
		{
			if (own)
				FreeAligned(m_data);
		}
	};
}

#endif /*_LINEARARRAY_H_*/