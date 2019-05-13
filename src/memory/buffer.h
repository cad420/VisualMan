
#ifndef _BUFFER_H_
#define _BUFFER_H_
#include <cstdint>

namespace ysl
{
	class Buffer
	{
	public:
		Buffer(uint64_t bytes);
		void Resize(uint64_t byte);
		void Free();
		void Swap(const Buffer & buffer);
		uint64_t Size()const { return bytesCount; }
		void * Data() { return data; }
		const void * Data()const { return data; }
		virtual ~Buffer() { Free(); }
	private:
		char * data;
		uint64_t bytesCount;
	};
}
#endif