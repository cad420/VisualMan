#ifndef _RAWIO_H_
#define _RAWIO_H_
#include "../mathematics/geometry.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace ysl
{

	class AbstrRawIO
	{
	protected:
		std::string fileName;
		ysl::Size3 dimensions;
		size_t voxelSize;
	public:
		AbstrRawIO(const std::string & fileName, const ysl::Size3 &dimensions, size_t voxelSize);

		virtual void * FileMemPointer(unsigned long long offset, std::size_t size) = 0;
		virtual void DestroyFileMemPointer(void * addr) = 0;
		virtual bool WriteCommit() = 0;
		virtual ~AbstrRawIO() {}
	};
#ifdef _WIN32

	class WindowsMappingRawIO :public AbstrRawIO
	{
		HANDLE f;
		HANDLE mapping;
		void * addr;
	public:
		WindowsMappingRawIO(const std::string & fileName, const ysl::Size3 &dimensions, size_t voxelSize);
		void * FileMemPointer(unsigned long long offset, std::size_t size) override;
		void DestroyFileMemPointer(void * addr) override;
		bool WriteCommit() override;
		~WindowsMappingRawIO();
	};
#endif

}
#endif