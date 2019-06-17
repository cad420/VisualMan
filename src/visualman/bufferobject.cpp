
#include "bufferobject.h"
#include "graphictype.h"
#include <cassert>

//#include <GLFW/glfw3.h>

namespace ysl
{
	namespace vm
	{
		BufferObject::BufferObject(BufferObjectTarget target):bufferTarget(target)
		{

		}

		void BufferObject::CreateBufferObject()
		{
			if (handle == 0)
			{
				GL(glGenBuffers(1, &handle));
				assert(handle);
				bufferSize = 0;
			}
		}

		void BufferObject::DestroyBufferObject()
		{
			if (handle != 0)
			{
				UnmapBuffer();
				GL(glDeleteBuffers(1, &handle));
				handle = 0;
				bufferSize = 0;
			}

		}

		void BufferObject::Download()
		{
			if (handle != 0)
			{
				Resize(bufferSize);
				void * ptr = nullptr;
				ptr = MapBuffer(BA_READ_ONLY);
				memcpy(LocalData(), ptr, Bytes());
				UnmapBuffer();
			}
		}

		void BufferObject::SetBufferData(size_t bytes, const void * data, BufferObjectUsage usage)
		{
			CreateBufferObject();
			assert(handle);
			GL(glBindBuffer(GetBufferTarget(), handle));
			GL(glBufferData(GetBufferTarget(), bytes, data, usage));
			GL(glBindBuffer(GetBufferTarget(), 0));
			bufferSize = bytes;
			this->bufferUsage = usage;
		}

		void BufferObject::SetBufferData(BufferObjectUsage usage, bool discard)
		{
			SetBufferData(Bytes(), LocalData(), usage);
			if (discard)
				Resize(0);
		}

		void BufferObject::SetBufferSubData(size_t offset, size_t bytes, const void* data)
		{
			assert(handle);
			if (handle && data)
			{
				GL(glBindBuffer(GetBufferTarget(), handle));
				GL(glBufferSubData(GetBufferTarget(), offset, bytes, data));
				GL(glBindBuffer(GetBufferTarget(), 0));
			}
			else
			{
				Warning("No buffer object. %s:%d", __FILE__, __LINE__);
			}
		}

		void BufferObject::SetBufferSubData(size_t offset, size_t bytes, bool discard)
		{
			assert(handle);
			if (handle)
			{
				GL(glBindBuffer(GetBufferTarget(), handle));
				GL(glBufferSubData(GetBufferTarget(), offset, bytes, LocalData()));
				GL(glBindBuffer(GetBufferTarget(), 0));
				if (discard) Resize(0);
			}
			else
			{
				Warning("No buffer object. %s:%d", __FILE__, __LINE__);
			}
		}

		void BufferObject::SetBufferSubDataFromLocalSubData(size_t bOffset, size_t localBufferOffset, size_t bytes)
		{
			assert(handle);
			if (handle)
			{
				GL(glBindBuffer(GetBufferTarget(), handle));
				GL(glBufferSubData(GetBufferTarget(), bOffset, bytes, LocalData() + localBufferOffset));
				GL(glBindBuffer(GetBufferTarget(), 0));
			}
			else
			{
				Warning("No buffer object. %s:%d", __FILE__, __LINE__);
			}
		}

		void* BufferObject::MapBuffer(BufferObjectAccess access)
		{
			//CreateBufferObject();
			if(mapped)
			{
				Warning("Buffer Object has been mapped\n");
				return nullptr;
			}
			if (handle)
			{
				void * ptr = nullptr;
				GL(glBindBuffer(GetBufferTarget(), handle));
				GL(ptr = glMapNamedBuffer(handle, access));
				GL(glBindBuffer(GetBufferTarget(), 0));
				mapped = true;
				return ptr;
			}
			Warning("No buffer object. %s:%d", __FILE__, __LINE__);
			return nullptr;
		}

		void BufferObject::UnmapBuffer()
		{
			if (handle && mapped)
			{
				GL(glBindBuffer(GetBufferTarget(), handle));
				GL(glUnmapNamedBuffer(handle));
				GL(glBindBuffer(GetBufferTarget(), 0));
				mapped = false;
			}
			else if (!handle)
			{
				Warning("No buffer object. %s:%d", __FILE__, __LINE__);
			}
		}

	}
}
