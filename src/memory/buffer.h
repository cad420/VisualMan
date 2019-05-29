
#ifndef _BUFFER_H_
#define _BUFFER_H_
#include <cstdint>
#include "dataarena.h"

namespace ysl
{
	class LocalBuffer
	{
		using size_type = size_t;
		size_type bytes = 0;
		uint8_t * data= nullptr;
		bool own = true;
	public:
		LocalBuffer();

		LocalBuffer(size_type x, uint8_t* data, bool own);

		LocalBuffer(size_type x, const uint8_t* data);

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

		void SetLocalData(void * data, size_t bytes)
		{
			Resize(bytes / sizeof(char));
			memcpy(LocalData(), data, bytes);
		}
		/**
		 * \brief

		 * Returns true if resize is success
		 */
		bool Resize(std::size_t x);

		void Clear();

		~LocalBuffer()
		{
			Clear();
		}

		const uint8_t * LocalData()const { return data; }
		uint8_t * LocalData() { return data; }
	};
}
#endif