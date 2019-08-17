
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
				GL(glCreateBuffers(1, &handle));
				assert(handle);
				bufferSize = 0;
			}else
			{
				Debug("Buffer Object has already been created");
			}
		}

		void BufferObject::CreateImmutableBufferObject(size_t size, const void * data, int flags)
		{
			if (handle == 0)
			{
				GL(glCreateBuffers(1, &handle));
				assert(handle);
				GL(glNamedBufferStorage(handle,size,data,flags));
				bufferSize = size;
			}
			else
			{
				Debug("Buffer Object has already been created");
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

		void BufferObject::ReallocBufferData(size_t bytes, const void * data, BufferObjectUsage usage)
		{
			if(IsImmutable())
			{
				Warning("This is am imutable buffer. It is not able to be resized.");
				return;
			}
			CreateBufferObject();
			assert(handle);
			GL(glBindBuffer(GetBufferTarget(), handle));
			GL(glBufferData(GetBufferTarget(), bytes, data, usage));
			GL(glBindBuffer(GetBufferTarget(), 0));
			bufferSize = bytes;
			//this->bufferUsage = usage;
		}

		void BufferObject::ReallocBufferData(BufferObjectUsage usage, bool discard)
		{
			if (IsImmutable())
			{
				Warning("This is am imutable buffer. It is not able to be resized.");
				return;
			}
			ReallocBufferData(Bytes(), LocalData(), usage);
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

		void* BufferObject::MapBuffer(BufferMapAccess access)
		{
			//CreateBufferObject();
			if(IsMapped())
			{
				Warning("Buffer Object has been mapped\n");
				return nullptr;
			}
			if (handle)
			{
				GL(glBindBuffer(GetBufferTarget(), handle));
				GL(mappedPointer = glMapNamedBuffer(handle, access));
				GL(glBindBuffer(GetBufferTarget(), 0));
				//mapped = true;
				return mappedPointer;
			}
			Warning("No buffer object. %s:%d", __FILE__, __LINE__);
			return nullptr;
		}

		void* BufferObject::MapBufferRange(size_t offset,size_t length,int rangeMapFlags)
		{
			if(IsMapped())
			{
				Warning("Buffer Object has been mapped\n");
				return nullptr;
			}
			if(handle)
			{
				GL(glBindBuffer(GetBufferTarget(), handle));
				GL(mappedPointer = glMapNamedBufferRange(handle,offset,length, rangeMapFlags));
				GL(glBindBuffer(GetBufferTarget(), 0));
				//mapped = true;
				return mappedPointer;
			}
			Warning("No buffer object. %s:%d", __FILE__, __LINE__);
			return nullptr;
		}

		int BufferObject::GetBufferStorageFlags() const
		{
			GLint flags;
			GL(glGetNamedBufferParameteriv(handle,GL_BUFFER_STORAGE_FLAGS,&flags));
			return flags;
		}

		void BufferObject::UnmapBuffer()
		{
			if (handle && IsMapped())
			{
				GL(glBindBuffer(GetBufferTarget(), handle));
				GL(glUnmapNamedBuffer(handle));
				GL(glBindBuffer(GetBufferTarget(), 0));
				//mapped = false;
				mappedPointer = nullptr;
			}
			else if (!handle)
			{
				Warning("No buffer object. %s:%d", __FILE__, __LINE__);
			}
		}

		bool BufferObject::IsMapped() const
		{
			if (handle == 0)
				return false;
			GLint m;
			GL(glGetNamedBufferParameteriv(handle,GL_BUFFER_MAPPED,&m));
			return m == GL_TRUE;
		}

		bool BufferObject::IsImmutable() const
		{
			if (handle == 0)
				return false;
			GLint im;
			GL(glGetNamedBufferParameteriv(handle, GL_BUFFER_IMMUTABLE_STORAGE, &im));
			return im == GL_TRUE;
		}

		BufferMapAccess BufferObject::GetBufferMapAccess() const
		{
			if (handle == 0)
				return BufferMapAccess(0);
			GLint ma;
			GL(glGetNamedBufferParameteriv(handle, GL_BUFFER_ACCESS, &ma));
			return BufferMapAccess(ma);
		}

		int BufferObject::GetBufferMapRangeFlags() const
		{
			if (handle == 0)
				return 0;
			GLint mf;
			GL(glGetNamedBufferParameteriv(handle, GL_BUFFER_ACCESS_FLAGS, &mf));
			return mf;
		}

		BufferObjectUsage BufferObject::Usage() const
		{
			if (handle == 0)
				return BufferObjectUsage(0);
			GLint bufferUsage;
			GL(glGetNamedBufferParameteriv(handle, GL_BUFFER_USAGE, &bufferUsage));
			return BufferObjectUsage(bufferUsage);
		}
	}
}
