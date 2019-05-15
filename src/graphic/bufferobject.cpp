
#include "bufferobject.h"

namespace ysl
{
	namespace graphics
	{
		void BufferObject::Create()
		{

		}

		void BufferObject::Destroy()
		{
			// glDeleteBuffers(1,&handle);
			handle = 0;
			bufferSize = 0;
		}

		void BufferObject::Download()
		{

		}

		void BufferObject::Upload()
		{

		}

		void* BufferObject::MapBuffer()
		{
			return nullptr;
		}

		void BufferObject::UnmapBuffer()
		{

		}


	}
}