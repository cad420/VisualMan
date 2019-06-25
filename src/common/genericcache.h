#ifndef _GENERICCACHE_H_
#define _GENERICCACHE_H_
#include "blockarray.h"

namespace ysl
{
	/**
	 * \brief This is used to eliminate template parameters of \a Block3DArray
	 */
	struct IBlock3DArrayAdapter
	{
		virtual void * GetBlockData(size_t blockID) = 0;
		virtual ~IBlock3DArrayAdapter() = default;
	};

	template<typename T,int log>
	class GenericBlockCache:public ysl::Block3DArray<T,log>,public IBlock3DArrayAdapter
	{
	public:
		GenericBlockCache(int w, int h, int d, T * data) :Block3DArray<T, log>(w, h, d, data){}
		void * GetBlockData(size_t blockID) override { return reinterpret_cast<void*>(Block3DArray<T,log>::BlockData(blockID)); }
	};

	template<typename T> using GenericBlock64Cache = GenericBlockCache<T, 6>;
	template<typename T> using GenericBlock128Cache = GenericBlockCache<T, 7>;
	template<typename T> using GenericBlock256Cache = GenericBlockCache<T, 8>;
	template<typename T> using GenericBlock512Cache = GenericBlockCache<T, 9>;
	template<typename T> using GenericBlock1024Cache = GenericBlockCache<T, 10>;


	using Int8Block64Cache = GenericBlock64Cache<char>;
	using Int8Block128Cache = GenericBlock128Cache<char>;
	using Int8Block256Cache = GenericBlock256Cache<char>;
	using Int8Block512Cache = GenericBlock512Cache<char>;
	using Int8Block1024Cache = GenericBlock1024Cache<char>;
}

#endif