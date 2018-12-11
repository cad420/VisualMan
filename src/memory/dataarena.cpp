#include "dataarena.h"
#include <malloc.h>


void* AllocAligned(size_t size, int align)
{
#ifdef _WIN32
	return _aligned_malloc(size, align);
	//return malloc(size);
#endif
}

void FreeAligned(void * ptr)
{
	_aligned_free(ptr);
	//free(ptr);
}
