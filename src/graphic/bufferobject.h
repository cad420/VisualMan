
#ifndef _BUFFEROBJECT_H_
#define _BUFFEROBJECT_H_
#include <cstdint>
#include "graphictype.h"
#include "../memory/lineararray.h"

namespace ysl
{
	namespace graphics
	{
		class GRAPHICS_EXPORT_IMPORT BufferObject:public Linear1DArray<char>,
											      public std::enable_shared_from_this<BufferObject>
		{
		public:
			BufferObject(){}
			/**
			 * \brief A copy constructor
			 * 
			 * \note The copy constructor don't create a buffer object on gpu, it creates a default gpu-part instead.
			 */
			BufferObject(const BufferObject & other):Linear1DArray<char>(other)
			{
				handle = 0;
				bufferSize = 0;
				usage = BU_STATIC_DRAW;
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
				Destroy();
				Linear1DArray<char>::operator=(other);
				handle = other.handle;
				bufferSize = other.bufferSize;
				usage = other.usage;
				return *this;
			}

			/**
			 * \brief A move constructor. 
			 * 
			 * \note The move constructor also move the gpu-part
			 */
			BufferObject(BufferObject && other)noexcept:Linear1DArray<char, 64>(std::move(other))
			{
				handle = other.handle;
				bufferSize = other.bufferSize;
				usage = other.usage;
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
				Destroy();
				Linear1DArray<char>::operator=(std::move(other));
				handle = other.handle;
				bufferSize = other.bufferSize;
				usage = other.usage;
				other.handle = 0;
				other.bufferSize = 0;
				return *this;
			}


			unsigned int Handle()const { return handle; }
			uint64_t BufferSize()const { return Size(); }

			void Create();

			void Destroy();

			void Download();

			void Upload();

			void * MapBuffer();

			void UnmapBuffer();

			BufferObjectUsage Usage()const { return usage; }
		private:
			unsigned int handle = 0;
			uint64_t bufferSize = 0;
			BufferObjectUsage usage = BU_STATIC_DRAW;
		};
	}
}
#endif