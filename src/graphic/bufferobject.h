
#ifndef _BUFFEROBJECT_H_
#define _BUFFEROBJECT_H_
#include <cstdint>

namespace ysl
{
	namespace gpu
	{



		class BufferObject
		{
		public:
			unsigned int Handle()const { return handle; }
			uint64_t BufferSize()const { return bufferSize; }
			void Create();
			void Destroy();
			void Download();
			void Upload();
			void * MapBuffer();
			void UnmapBuffer();

		private:
			unsigned int handle;
			uint64_t bufferSize;
		};
	}
}
#endif