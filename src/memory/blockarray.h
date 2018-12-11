#ifndef BLOCKARRAY_H_
#define BLOCKARRAY_H_

#include "dataarena.h"

namespace ysl
{


	template <typename T, int nLogBlockSize>
	class Block2DArray
	{
		T * m_data;
		const int m_nx, m_ny, m_nxBlocks;
	public:
		Block2DArray(int x, int y, const T * d = nullptr) :m_nx(x), m_ny(y), m_nxBlocks(RoundUp(m_nx) >> nLogBlockSize)
		{
			const auto nAlloc = RoundUp(m_nx) * RoundUp(m_ny);
			m_data = AllocAligned<T>(nAlloc);
			for (auto i = 0; i < nAlloc; i++)
				new (&m_data[i])T();
			if (d)
				for (auto y = 0; y < m_ny; y++)
					for (auto x = 0; x < m_nx; x++)
						(*this)(x, y) = d[x + y * m_nx];
		}
		constexpr size_t BlockSize()const { return 1 << nLogBlockSize; }
		int Width()const
		{
			return m_nx;
		}
		int Height()const
		{
			return  m_ny;
		}
		/**
		 * \brief  Returns the multiple of BlockSize()
		 */
		int RoundUp(int x)const
		{
			return (x + BlockSize() - 1) & ~(BlockSize() - 1);
		}

		int Block(int index)const
		{
			return index >> nLogBlockSize;
		}

		int Offset(int index)const
		{
			return index & (BlockSize() - 1);
		}

		T & operator()(int x, int y)
		{
			return const_cast<T&>(static_cast<const Block2DArray&>(*this)(x, y));
		}
		const T & operator()(int x, int y)const
		{
			const auto xBlock = Block(x), yBlock = Block(y);
			const auto xOffset = Offset(x), yOffset = Offset(y);
			const auto index = (m_nxBlocks * yBlock + xBlock)*BlockSize()*BlockSize() + BlockSize()*yOffset + xOffset;
			return m_data[index];
		}

		void GetLinearArray(T * arr)
		{
			for (auto y = 0; y < m_ny; y++)
				for (auto x = 0; x < m_nx; x++)
					*arr++ = (*this)(x, y);
		}

		~Block2DArray()
		{
			const auto nAlloc = RoundUp(m_nx) * RoundUp(m_ny);
			if (m_data)
				for (auto i = 0; i < nAlloc; i++)
					m_data[i].~T();
			FreeAligned(m_data);
		}
	};

	template <typename T, int nLogBlockSize>
	class Block3DArray
	{
		T * m_data;
		const int m_nx, m_ny, m_nz, m_nxBlocks, m_nyBlocks, m_nzBlocks;

		// Delegate Constructor
		Block3DArray(int x, int y, int z) :
			m_data(nullptr),
			m_nx(x),
			m_ny(y),
			m_nz(z),
			m_nxBlocks(RoundUp(m_nx) >> nLogBlockSize),
			m_nyBlocks(RoundUp(m_ny) >> nLogBlockSize),
			m_nzBlocks(RoundUp(m_nz) >> nLogBlockSize)
		{}

	public:
		Block3DArray(int x, int y, int z, const T * linearArray) :Block3DArray(x, y, z)
		{
			const auto nAlloc = RoundUp(m_nx) * RoundUp(m_ny) * RoundUp(m_nz);
			m_data = AllocAligned<T>(nAlloc);
			for (auto i = 0; i < nAlloc; i++) new (&m_data[i])T();
			if (linearArray)
				for (auto z = 0; z < m_nz; z++)
					for (auto y = 0; y < m_ny; y++)
						for (auto x = 0; x < m_nx; x++)
							(*this)(x, y, z) = linearArray[x + y * m_nx + z * m_nx*m_ny];
		}
		constexpr size_t BlockSize()const { return 1 << nLogBlockSize; }

		Block3DArray(const Block3DArray & array) = delete;
		Block3DArray & operator=(const Block3DArray & array) = delete;

		Block3DArray(Block3DArray && array)noexcept :Block3DArray(array.m_nx, array.m_ny, array.m_nz)
		{
			m_data = array.m_data;
			array.m_data = nullptr;
		}

		Block3DArray& operator=(Block3DArray && array)noexcept
		{
			m_nx(array.x);
			m_ny(array.y);
			m_nz(array.z);
			m_nxBlocks(RoundUp(m_nx) >> nLogBlockSize);
			m_nyBlocks(RoundUp(m_ny) >> nLogBlockSize);
			m_nzBlocks(RoundUp(m_nz) >> nLogBlockSize);
			m_data = array.m_data;
			array.m_data = nullptr;
			return *this;
		}

		int Width()const
		{
			return m_nx;
		}
		int Height()const
		{
			return m_ny;
		}
		int Depth()const
		{
			return m_nz;
		}

		int BlockWidth()const
		{
			return m_nxBlocks;
		}

		int BlockHeight()const
		{
			return m_nyBlocks;
		}

		int BlockDepth()
		{
			return m_nzBlocks;
		}

		/**
		 * \brief  Returns the multiple of BlockSize()
		 */
		int RoundUp(int x)const
		{
			return (x + BlockSize() - 1) & ~(BlockSize() - 1);
		}

		int Block(int index)const
		{
			return index >> nLogBlockSize;
		}

		int Offset(int index)const
		{
			return index & (BlockSize() - 1);
		}

		T & operator()(int x, int y, int z)
		{
			return const_cast<T&>(static_cast<const Block3DArray&>(*this)(x, y, z));
		}
		const T & operator()(int x, int y, int z)const
		{
			const auto xBlock = Block(x), yBlock = Block(y), zBlock = Block(z);
			const auto xOffset = Offset(x), yOffset = Offset(y), zOffset = Offset(z);
			const auto index = (m_nyBlocks*m_nxBlocks*zBlock + m_nxBlocks * yBlock + xBlock)*BlockSize()*BlockSize()*BlockSize() +
				BlockSize()*BlockSize()*zOffset +
				BlockSize()*yOffset + xOffset;
			return m_data[index];
		}

		T * BlockData(int blockIndex)
		{
			return const_cast<T*>(static_cast<Block3DArray&>(*this).BlockData(blockIndex));
		}

		const T * BlockData(int blockIndex)const
		{
			return m_data + blockIndex * BlockSize()*BlockSize()*BlockSize();
		}

		T * BlockData(int xBlock, int yBlock, int zBlock)
		{
			return const_cast<T*>(static_cast<Block3DArray&>(*this).BlockData(xBlock, yBlock, zBlock));
		}

		const T * BlockData(int xBlock, int yBlock, int zBlock)const
		{
			const auto blockIndex = zBlock * (BlockWidth()*BlockHeight()) + yBlock * BlockWidth() + zBlock;
			return BlockData(blockIndex);
		}

		void SetBlockData(int blockIndex, const T * blockData)
		{
			::memcpy(BlockData(blockIndex), blockData, BlockSize()*BlockSize()*BlockSize() * sizeof(T));
		}

		void SetBlockData(int xBlock, int yBlock, int zBlock, const T * blockData)
		{
			::memcpy(BlockData(xBlock, yBlock, zBlock), blockData, BlockSize()*BlockSize()*BlockSize() * sizeof(T));
		}

		void GetLinearArray(T * arr)
		{

			for (auto z = 0; z < m_nz; z++)
				for (auto y = 0; y < m_ny; y++)
					for (auto x = 0; x < m_nx; x++)
						*arr++ = (*this)(x, y,z);
		}
		virtual ~Block3DArray()
		{
			const auto nAlloc = RoundUp(m_nx) * RoundUp(m_ny);
			if (m_data)
				for (auto i = 0; i < nAlloc; i++)
					m_data[i].~T();
			FreeAligned(m_data);
		}
	};

}
#endif