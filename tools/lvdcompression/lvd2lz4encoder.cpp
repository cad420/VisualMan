
#include <iostream>
#include <lz4.h>
#include <lvdreader.h>
#include <vector>
#include <fstream>
#include <libraryloader.h>


struct LZ4Encoder
{
private:
	ysl::LVDReader& lvdFileReader;
public:
	LZ4Encoder(ysl::LVDReader& lvdFileReader):lvdFileReader(lvdFileReader)
	{
	}
	bool Save(const std::string& fileName)
	{
		ysl::LVDHeader newHeader;
		const auto& h = lvdFileReader.GetHeader();
		newHeader.magicNum = 277537;
		newHeader.blockLengthInLog = h.blockLengthInLog;
		memcpy(newHeader.dataDim, h.dataDim, sizeof(h.dataDim));
		memcpy(newHeader.originalDataDim, h.originalDataDim, sizeof(h.originalDataDim));
		newHeader.padding = h.padding;


		const auto blockCount = lvdFileReader.BlockCount();
		const auto blockBytes = lvdFileReader.BlockDataCount();
		std::vector<uint64_t> offsets(blockCount);
		std::vector<uint32_t> bytes(blockCount);


		std::ofstream outFile(fileName,std::ios::binary);
		if(outFile.is_open() == false)
		{
			throw std::runtime_error("failed to open file");
		}

		const uint64_t dataOffset = newHeader.HeaderSize() // header info
			+ sizeof(int)							// the number of array for offsets and bytes
			+ sizeof(uint64_t) * blockCount
			+ sizeof(uint32_t) * blockCount;	// offsets and bytes for each block


		// writer header and metatdata


		std::unique_ptr<char[]> srcBuf(new char[blockBytes]), dstBuf(new char[blockBytes]);
		//std::unique_ptr<char[]> testBuf(new char[blockBytes]);
		uint64_t curOffset = dataOffset;

		outFile.seekp(dataOffset, std::ios::beg);
		for(auto i = 0;i<blockCount;i++)
		{
			lvdFileReader.ReadBlock(srcBuf.get(), i);
			const auto resBytes = LZ4_compress_default(srcBuf.get(), dstBuf.get(), blockBytes, blockBytes);
		//	const auto testBytes = LZ4_decompress_safe(dstBuf.get(), testBuf.get(),resBytes,blockBytes);
			//if(testBytes!= blockBytes)
			//{
			//	std::cout << "invalid:"<<testBytes << std::endl;
		//	}
			if(resBytes > blockBytes)
			{
				std::cout << "Compressing failed\n";
				return false;
			}
			offsets[i] = curOffset;
			bytes[i] = resBytes;
			outFile.write(dstBuf.get(), resBytes);

			curOffset += resBytes;
		}

		//outFile.seekp(newHeader.HeaderSize() + sizeof(int), std::ios::beg);

		outFile.seekp(0, std::ios::beg);

		outFile.write((const char *)newHeader.Encode(), newHeader.HeaderSize());
		outFile.write((const char *)&blockCount, sizeof(int));
		outFile.write((const char *)offsets.data(), sizeof(uint64_t) * blockCount);
		outFile.write((const char *)bytes.data(), sizeof(uint32_t) * blockCount);

		outFile.seekp(0,std::ios::end);

		const std::size_t totalBytes = outFile.tellp();
		if (totalBytes != curOffset)
		{
			throw std::runtime_error("File size does not match.");
		}
	}
};

int main()
{
	ysl::LibraryReposity::GetLibraryRepo()->AddLibraries("plugins");
	std::string input, output;
	std::cout << "[intput filename(str), output filename(str)]\n";
	std::cin >> input >> output;

	ysl::LVDReader reader(input);


	LZ4Encoder endcoder(reader);

	endcoder.Save(output);
	
	
	
	
	return 0;
}