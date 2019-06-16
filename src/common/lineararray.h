
#ifndef _LINEARARRAY_H_
#define _LINEARARRAY_H_


#include "dataarena.h"
#include "geometry.h"
#include <cstring>

namespace ysl
{

	class LocalBuffer
	{
		using size_type = std::size_t;
		size_type bytes;
		uint8_t * data;
		bool own;
	public:
		LocalBuffer() :bytes(0), data(nullptr), own(true)
		{

		}
		LocalBuffer(size_type x, uint8_t * data, bool own) :bytes(x), data(data), own(own)
		{

		}

		LocalBuffer(size_type x, const uint8_t * data) :LocalBuffer(x, nullptr, true)
		{
			this->data = (uint8_t*)AllocAligned<uint8_t>(x);
			if (!this->data)
				throw std::runtime_error("Bad Alloc");
			if (data)
				memcpy(this->data, data, x * sizeof(uint8_t));
		}

		LocalBuffer(const LocalBuffer & array) :LocalBuffer(array.bytes, array.data, array.own)
		{

		}
		LocalBuffer & operator=(const LocalBuffer & array)
		{
			Clear(); // clear previous data

			this->bytes = array.bytes;
			this->own = own;
			memcpy(data, array.data, sizeof(uint8_t) * bytes);
			return *this;
		}

		LocalBuffer(LocalBuffer && array)noexcept :
			LocalBuffer(array.bytes, array.data, array.own)
		{
			array.data = nullptr;
		}

		LocalBuffer & operator=(LocalBuffer && array)noexcept
		{
			Clear(); // clear previous data

			bytes = array.bytes;
			data = array.data;
			own = array.own;
			array.data = nullptr;
			return *this;
		}

		size_type Bytes()const { return bytes * sizeof(uint8_t); }

		std::size_t Count()const { return bytes; }

		char & operator()(int x)
		{
			return const_cast<char&>(static_cast<const LocalBuffer &>(*this)(x));
		}

		const char & operator()(int x)const
		{
			return data[x];
		}

		void SetLocalData(const void* data, size_t bytes)
		{
			Resize(bytes / sizeof(char));
			memcpy(LocalData(), data, bytes);
		}

		/**
		 * \brief

		 * Returns true if resize is success
		 */
		bool Resize(std::size_t x)
		{
			if (own)
			{
				if (x == bytes)
					return false;

				auto * ptr = (uint8_t*)AllocAligned<uint8_t>(x);
				if (x > bytes)
				{
					memcpy(ptr, data, bytes);
				}
				else if (x < bytes)
				{
					memcpy(ptr, data, x);
				}
				Clear();
				bytes = x;
				data = ptr;
				return true;
			}
			return false;
		}

		void Clear()
		{
			if (own)
				FreeAligned(data);
			data = nullptr;
			bytes = 0;
			own = true;
		}

		~LocalBuffer()
		{
			Clear();
		}

		const uint8_t * LocalData()const { return data; }

		uint8_t * LocalData() { return data; }

		void SetUnderlyingDataPointer(uint8_t * d, size_t size, bool own)
		{
			Clear();
			data = d;
			this->bytes = size;
			this->own = own;
		}
	};


	template<typename T,int nCacheLine = 64>
	class Linear1DArray
	{
		using size_type = std::size_t;
		std::size_t size;
		T * data;
		bool own;
	public:
		Linear1DArray():size(0),data(nullptr),own(true)
		{
			
		}
		Linear1DArray(size_type x,T * data,bool own):size(x),data(data),own(own)
		{
			
		}

		Linear1DArray(size_type x,const T * data):Linear1DArray(x,nullptr,true)
		{
			data = (T*)AllocAligned<T>(x);
			if(data == nullptr)
			{
				throw std::runtime_error("Bad Alloc");
			}
			if (data)
				memcpy(this->data, data, x * sizeof(T));
		}

		Linear1DArray(const Linear1DArray & array):Linear1DArray(array.size,array.data,array.own)
		{
			
		}
		Linear1DArray & operator=(const Linear1DArray & array)
		{
			Clear(); // clear previous data

			this->size = array.size;
			this->own = own;
			memcpy(data, array.data, sizeof(T) * size);
			return *this;
		}

		Linear1DArray(Linear1DArray && array)noexcept :
			Linear1DArray(array.size, array.data, array.own)
		{
			array.data = nullptr;
		}

		Linear1DArray & operator=(Linear1DArray && array)noexcept
		{
			Clear(); // clear previous data

			size = array.size;
			data = array.data;
			own = array.own;
			array.data = nullptr;
			return *this;
		}

		size_type Bytes()const { return size*sizeof(T); }
		std::size_t Count()const { return size; }

		T & operator()(int x)
		{
			return const_cast<T&>(static_cast<const Linear1DArray &>(*this)(x));
		}

		const T & operator()(int x)const
		{
			return data[x];
		}

		void SetLocalData(void * data, size_t bytes)
		{
			Resize(bytes / sizeof(T));
			memcpy(Data(), data, bytes);
		}

		/**
		 * \brief 

		 * Returns true if resize is success
		 */
		bool Resize(std::size_t x)
		{
			if(own)
			{
				if (x == size)
					return false;

				T * ptr = (T*)AllocAligned<T>(x);
				if(x > size)
				{
					memcpy(ptr, data, size);
				}else if(x < size)
				{
					memcpy(ptr, data, x);
				}
				Clear();
				size = x;
				data = ptr;
				return true;
			}
			return false;
		}

		void Clear()
		{
			if (own)
				FreeAligned(data);
			data = nullptr;
			size = 0;
			own = true;
		}

		~Linear1DArray()
		{
			Clear();
		}

		const T * Data()const { return data; }
		T * Data() { return data; }
	};


	template<typename T,int nCacheLine = 64>
	class Linear2DArray
	{
		using size_type = std::size_t;
		Size2 size;
		T * m_data;
		bool m_own;
	public:

		Linear2DArray():size(0,0),m_data(nullptr),m_own(true)
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
			if(data)memcpy(m_data, data, x*y*sizeof(T));
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
			Clear();
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
			Clear();
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

		void Clear()
		{
			if (m_own)
				FreeAligned(m_data);
			size = Size2(0,0);
			m_own = true;
			m_data = nullptr;
		}

		~Linear2DArray()
		{
			Clear();
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

		Linear3DArray():size(0,0,0),m_data(nullptr), xyPlaneSize(0),own(true)
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
				throw std::runtime_error("Bad Alloc in Linear3DArray(size_type x, size_type y,size_type z ,const T * data)\n");
				return;
			}
			if(data)
				memcpy(m_data, data,t*sizeof(T));
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
			Clear();
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
		}

		Linear3DArray & operator=(Linear3DArray && array)noexcept
		{
			Clear();
			size = array.size;
			m_data = array.m_data;
			own = array.own;
			xyPlaneSize = array.xyPlaneSize;
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

		void Clear()
		{
			if (own)
				FreeAligned(m_data);
			own = true;
			size = Size3(0, 0, 0);
			m_data = nullptr;
		}

		~Linear3DArray()
		{
			Clear();
		}
	};


	using Volumeb = Linear3DArray<unsigned char>;
	using Volume2b = Linear3DArray<Vector2<unsigned char>>;
	using Volume3b = Linear3DArray<Vector3<unsigned char>>;
	using Volume4b = Linear3DArray<Vector4<unsigned char>>;

	using Volumef = Linear3DArray<float>;
	using Volume2f = Linear3DArray<Vector2f>;
	using Volume3f = Linear3DArray<Vector3f>;
	using Volume4f = Linear3DArray<Vector4f>;

	using Volumei = Linear3DArray<int>;
	using Volume2i = Linear3DArray<Vector2i>;
	using Volume3i = Linear3DArray<Vector3i>;
	using Volume4i = Linear3DArray<Vector4i>;

	using Volumeui = Linear3DArray<unsigned int>;
	using Volume2ui = Linear3DArray<Vector2<unsigned int>>;
	using Volume3ui = Linear3DArray<Vector3<unsigned int>>;
	using Volume4ui = Linear3DArray<Vector4<unsigned int>>;


}

#endif /*_LINEARARRAY_H_*/