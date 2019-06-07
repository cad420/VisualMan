#ifndef DATAARENA_H_
#define DATAARENA_H_

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <list>
#include <vector>



constexpr std::size_t CashLine = 64;


void *AllocAligned(std::size_t size, int align);

void  FreeAligned(void * ptr);

template<typename T>
T * AllocAligned(std::size_t n)
{
	return (T*)(AllocAligned(sizeof(T) * n, 64));
}

template<int nCashLine = 64>
class DataArena
{
	const size_t m_blockSize;
	size_t m_currentBlockPos;
	size_t m_currentAllocBlockSize;
	uint8_t * m_currentBlock;
	size_t m_fragmentSize;
	//std::vector<int> m_pos;
	std::list<std::pair<uint8_t*, int>> m_used;
	std::list<std::pair<uint8_t*, int>> m_available;
public:
	explicit DataArena(size_t size = 1024 * 1024) :m_blockSize(size),
		m_currentAllocBlockSize(0),
		m_currentBlock(nullptr),
		m_currentBlockPos(0),
		m_fragmentSize(0)
	{
		// Default block is 1MB
	}

	DataArena(const DataArena & arena) = delete;
	DataArena & operator=(const DataArena & arena) = delete;

	DataArena(DataArena && arena)noexcept :
		m_blockSize(arena.m_blockSize)
		, m_currentBlockPos(arena.m_currentBlockPos)
		, m_currentAllocBlockSize(arena.m_currentAllocBlockSize)
		, m_currentBlock(arena.m_currentBlock)
		, m_fragmentSize(arena.m_fragmentSize)
		, m_used(std::move(arena.m_used))
		, m_available(std::move(arena.m_available))
	{

		arena.m_currentBlock = nullptr;
	}

	DataArena & operator=(DataArena && arena)noexcept
	{
		Release();		// Release memory
		m_blockSize = arena.m_blockSize;
		m_currentBlockPos = arena.m_currentBlockPos;
		m_currentAllocBlockSize = arena.m_currentAllocBlockSize;
		m_currentBlock = arena.m_currentBlock;
		arena.m_currentBlock = nullptr;
		m_fragmentSize = arena.m_fragmentSize;
		m_used = std::move(arena.m_used);
		m_available = std::move(arena.m_available);
		return *this;
	}


	void * Alloc(size_t bytes)
	{
		const auto align = alignof(std::max_align_t);
		bytes = (bytes + align - 1)&~(align - 1);		// Find a proper size to match the aligned boundary
		if (m_currentBlockPos + bytes > m_currentAllocBlockSize)
		{
			// Put into used list. A fragment generates.
			if (m_currentBlock)
			{
				m_used.push_back(std::make_pair(m_currentBlock, m_currentAllocBlockSize));
				m_fragmentSize += m_currentAllocBlockSize - m_currentBlockPos;
				m_currentBlock = nullptr;
				m_currentBlockPos = 0;
				//std::cout << "Current block can not accommodate this size, put it into used list.\n";
			}

			// Try to find available block
			for (auto it = m_available.begin(); it != m_available.end(); ++it)
			{
				if (bytes <= it->second)
				{
					m_currentBlock = it->first;
					m_currentAllocBlockSize = it->second;
					m_currentBlockPos = 0;
					break;
				}
			}

			if (!m_currentBlock)
			{
				// Available space can not be found. Allocates new memory
				m_currentAllocBlockSize = (std::max)(bytes, m_blockSize);
				m_currentBlock = static_cast<uint8_t*>(AllocAligned(m_currentAllocBlockSize, nCashLine));
				if (m_currentBlock == nullptr)
				{
					return nullptr;
				}
				m_currentBlockPos = 0;
			}
			m_currentBlockPos = 0;
		}

		const auto ptr = m_currentBlock + m_currentBlockPos;
		m_currentBlockPos += bytes;
		return ptr;
	}

	/**
	 * \brief Allocate for \a n objects for type \a T, runs constructor depends on \a construct on it and return its pointer
	 *
	 * \note For safety, this function should be check if the \a T is a type of POD or trivial.
	 *		 Maybe it can be checked by \a std::is_trivial or \a std::is_pod(deprecated).
	 *		 This issued will be addressed later.
	 *
	 * \sa Reset()
	 */
	template<typename T> T * Alloc(size_t n, bool construct = true)
	{
		const auto ptr = static_cast<T*>(Alloc(n * sizeof(T)));
		if (ptr == nullptr)
			return nullptr;
		if (construct)
			for (auto i = 0; i < n; i++)
				new (&ptr[i]) T();
		return ptr;
	}

	template<typename T, typename ...Args> T * AllocConstruct(Args&&... args)
	{
		const auto ptr = static_cast<T*>(Alloc(sizeof(T)));
		if (ptr == nullptr)return nullptr;
		new (ptr) T(std::forward<Args>(args)...);
		return ptr;
	}

	void Release()
	{
		for (auto it = m_used.begin(); it != m_used.end(); ++it)FreeAligned(it->first);
		for (auto it = m_available.begin(); it != m_available.end(); ++it)FreeAligned(it->first);
		FreeAligned(m_currentBlock);
	}

	void Shrink()
	{
		for (auto it = m_available.begin(); it != m_available.end(); ++it)FreeAligned(it->first);
	}


	void Reset()
	{
		m_currentBlock = nullptr;
		m_currentBlockPos = 0;
		m_currentAllocBlockSize = 0;
		m_fragmentSize = 0;
		m_available.splice(m_available.begin(), m_used);
	}

	size_t TotalAllocated()const
	{
		auto alloc = m_currentAllocBlockSize;
		for (auto it = m_used.begin(); it != m_used.end(); ++it)alloc += it->second;
		for (auto it = m_available.begin(); it != m_available.end(); ++it)alloc += it->second;
		return alloc;
	}

	size_t FragmentSize()const
	{
		return m_fragmentSize;
	}

	double FragmentRate()const
	{
		return static_cast<double>(m_fragmentSize) / TotalAllocated();
	}

	~DataArena()
	{
		Release();
	}
};
#endif