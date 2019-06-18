
#include "gpucachefaulthandler.h"
#include "openglutils.h"
#include <vector>
#include <largevolumecache.h>

namespace ysl
{
	void HashBasedGPUCacheFaultHandler::InitHashBuffer(int blockCount)
	{
		const std::size_t totalBlockCountBytes = capacity * sizeof(int);
		bufMissedHash = std::make_shared<OpenGLBuffer>(OpenGLBuffer::ShaderStorageBuffer,
			OpenGLBuffer::StreamDraw);
		bufMissedHash->AllocateFor(nullptr, totalBlockCountBytes);
		//GL_ERROR_REPORT;
		//bufMissedHash->BindBufferBase(0);
		BindHashTableTo(0);
	}

	void HashBasedGPUCacheFaultHandler::InitMissTableBuffer()
	{
		const auto missedBlockCapacity = capacity * sizeof(unsigned int);
		bufMissedTable = std::make_shared<OpenGLBuffer>(OpenGLBuffer::ShaderStorageBuffer,
			OpenGLBuffer::StreamDraw);
		bufMissedTable->AllocateFor(nullptr, missedBlockCapacity);
		//GL_ERROR_REPORT;
		//bufMissedTable->BindBufferBase(1);
		BindFaultTableTo(1);
	}

	void HashBasedGPUCacheFaultHandler::InitGPUAtomicCounter()
	{
		atomicCounter = std::make_shared<OpenGLBuffer>(OpenGLBuffer::AtomicCounterBuffer,
			OpenGLBuffer::DynamicCopy);
		atomicCounter->AllocateFor(nullptr, sizeof(GLuint));
		BindAtomicCounterTo(3);
		//atomicCounter->BindBufferBase(3);
		//atomicCounter->Unbind();
		GL_ERROR_REPORT;
	}

	void HashBasedGPUCacheFaultHandler::ResetBlockExistsHash()
	{
		GL_ERROR_REPORT;
		const auto ptr = bufMissedHash->Map(OpenGLBuffer::WriteOnly);
		memset(ptr, 0, bufMissedHash->Size());
		bufMissedHash->Unmap();
		GL_ERROR_REPORT;
	}

	void HashBasedGPUCacheFaultHandler::ResetMissedBlockVector()
	{
		//const auto ptr = bufMissedTable->Map(OpenGLBuffer::WriteOnly);
		//memset(ptr, 0, bufMissedTable->Size());
		//bufMissedTable->Unmap();
		atomicCounter->BindBufferBase(3);
		unsigned int zero = 0;
		atomicCounter->AllocateFor(&zero, sizeof(unsigned int));
		GL_ERROR_REPORT
	}



	HashBasedGPUCacheFaultHandler::HashBasedGPUCacheFaultHandler(int capacity, const Size3& dim) :capacity(capacity), dim(dim)
	{

		InitHashBuffer(capacity);
		InitMissTableBuffer();
		InitGPUAtomicCounter();

	}

	std::vector<VirtualMemoryBlockIndex> HashBasedGPUCacheFaultHandler::CaptureCacheFault()
	{
		//atomicCounter->Bind();
		const auto counters = static_cast<int*>(atomicCounter->Map(OpenGLBuffer::ReadOnly));
		const int count = counters[0];
		//std::cout << count << std::endl;

		//atomicCounter->Unmap();
		//atomicCounter->Unbind();

		//std::cout << count << std::endl;
		std::vector<VirtualMemoryBlockIndex> hits;
		if (count == 0)
			return hits;

		const auto missedBlocks = count;
		GL_ERROR_REPORT;
		//hits.clear();
		hits.reserve(missedBlocks);

		const auto ptr = static_cast<int*>(bufMissedTable->Map(OpenGLBuffer::ReadWrite));

		for (auto i = 0; i < missedBlocks; i++)
		{
			const auto blockId = ptr[i];
			//std::cout << "blockId:" << blockId << std::endl;
			hits.emplace_back(blockId, dim.x, dim.y, dim.z);
		}

		bufMissedTable->Unmap();
		GL_ERROR_REPORT;
		return hits;

	}

	void HashBasedGPUCacheFaultHandler::BindHashTableTo(int index)
	{
		bufMissedHash->Bind();
		bufMissedHash->BindBufferBase(index);
		bufMissedHash->Unbind();
		GL_ERROR_REPORT;
	}

	void HashBasedGPUCacheFaultHandler::BindFaultTableTo(int index)
	{
		bufMissedTable->Bind();
		bufMissedTable->BindBufferBase(index);
		bufMissedTable->Unbind();
		GL_ERROR_REPORT;
	}


	void HashBasedGPUCacheFaultHandler::BindAtomicCounterTo(int index)
	{
		atomicCounter->Bind();
		atomicCounter->BindBufferBase(index);
		atomicCounter->Unbind();
		GL_ERROR_REPORT;
	}

	void HashBasedGPUCacheFaultHandler::Reset()
	{
		ResetBlockExistsHash();
		ResetMissedBlockVector();

		//std::cout << "Reset:" << std::endl;
	}

	HashBasedGPUCacheFaultHandler::~HashBasedGPUCacheFaultHandler()
	{

	}
}
