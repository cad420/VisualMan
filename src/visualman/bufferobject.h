
#ifndef _BUFFEROBJECT_H_
#define _BUFFEROBJECT_H_
#include <cstdint>
#include <lineararray.h>

#include "graphictype.h"

namespace ysl
{
	namespace vm
	{
		class GRAPHICS_EXPORT_IMPORT BufferObject:public LocalBuffer,
											      public std::enable_shared_from_this<BufferObject>
		{
		public:
			BufferObject() = default;
			~BufferObject() { DestroyBufferObject(); }
			/**
			 * \brief A copy constructor
			 * 
			 * \note The copy constructor don't create a buffer object on gpu, it creates a default gpu-part instead.
			 */
			BufferObject(const BufferObject & other):LocalBuffer(other)
			{
				handle = 0;
				bufferSize = 0;
				bufferUsage = BU_STATIC_DRAW;
				*this = other;
			}
			/**
			 * \brief A copy assignment operator overloading
			 * 
			 * \note The copy assignment operator don't copy the buffer object of the \a other. 
			 * It will destroy its own buffer object and reset it as default state
			 */
			BufferObject& operator=(const BufferObject & other)
			{
				DestroyBufferObject();
				LocalBuffer::operator=(other);
				handle = other.handle;
				bufferSize = other.bufferSize;
				bufferUsage = other.bufferUsage;
				return *this;
			}

			/**
			 * \brief A move constructor. 
			 * 
			 * \note The move constructor also move the gpu-part
			 */
			BufferObject(BufferObject && other)noexcept:LocalBuffer(std::move(other))
			{
				handle = other.handle;
				bufferSize = other.bufferSize;
				bufferUsage = other.bufferUsage;
				other.handle = 0;
				other.bufferSize = 0;
			}

			/**
			 * \brief A move assignment operator
			 * 
			 * \note The move assignment operator also move the gpu-part
			 */
			BufferObject & operator=(BufferObject && other)noexcept
			{
				DestroyBufferObject();
				LocalBuffer::operator=(std::move(other));
				handle = other.handle;
				bufferSize = other.bufferSize;
				bufferUsage = other.bufferUsage;
				other.handle = 0;
				other.bufferSize = 0;
				return *this;
			}

			unsigned int Handle()const { return handle; }

			void CreateBufferObject();

			void DestroyBufferObject();

			/**
			 * \brief Download data from GPU to local memory
			 */
			void Download();

			uint64_t BufferObjectSize()const { return bufferSize; }

			/**
			 * \brief  Upload data directly to GPU provided by \a data
			 */
			void SetBufferData(size_t bytes,const void * data,BufferObjectUsage usage);

			/**
			 * \brief  Upload data to GPU using the local buffer
			 */
			void SetBufferData(BufferObjectUsage usage,bool discard);

			/**
			 * \brief  Upload data directly to GPU provided by \a data
			 */
			void SetBufferSubData(size_t offset,size_t bytes,const void * data);

			//void Upload();

			void * MapBuffer(BufferObjectAccess access);

			void UnmapBuffer();

			BufferObjectUsage Usage()const { return bufferUsage; }
		private:
			unsigned int handle = 0;
			uint64_t bufferSize = 0;
			BufferObjectUsage bufferUsage = BU_STATIC_DRAW;
			bool mapped = false;
		};
	}
}
#endif