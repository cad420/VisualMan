
#include "buffer.h"
#include <cstring>

namespace ysl
{
	Buffer::Buffer(uint64_t bytes):bytesCount(bytes)
	{
		data = new char[bytes];
	}
	void Buffer::Resize(uint64_t byte)
	{
		if(byte > bytesCount)
		{
			char * old = data;
			data = new char[byte];
			memcpy(data, old, bytesCount);
			delete old;
			bytesCount = byte;
		}else
		{
			
		}
	}

	void Buffer::Free()
	{
		delete data;
		data = nullptr;
	}

	void Buffer::Swap(const Buffer& buffer)
	{

	}
}
