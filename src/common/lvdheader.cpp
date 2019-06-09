
#include "lvdheader.h"
#include <cstring>

namespace  ysl
{
	ysl::LVDHeader::LVDHeader() :buf(new unsigned char[BufSize])
	{

	}

	int LVDHeader::HeaderSize() const
	{
		return LVD_HEADER_SIZE;
	}

	void LVDHeader::Decode(unsigned char * p)
	{
		memcpy((&magicNum), p + (LVD_HEADER_MAGIC_FILED_OFFSET),  (LVD_HEADER_MAGIC_FILED_SIZE));
		memcpy(&dataDim[0], p + LVD_DATA_WIDTH_FIELD_OFFSET,LVD_DATA_WIDTH_FIELD_SIZE);
		memcpy(&dataDim[1], p + LVD_DATA_HEIGHT_FIELD_OFFSET,  LVD_DATA_HEIGHT_FIELD_SIZE);
		memcpy(&dataDim[2], p + LVD_DATA_DEPTH_FIELD_OFFSET,  LVD_DATA_DEPTH_FIELD_SIZE);
		memcpy(&blockLengthInLog, p + LVD_BLOCK_LOG_FILED_OFFSET, LVD_DATA_BLOCK_LENGTH_IN_LOG_FILED_SIZE);
		memcpy(&padding, p + LVD_BLOCK_PADDING_FIELD_OFFSET, LVD_DATA_PADDING_FIELD_SIZE);
		memcpy(&originalDataDim[0], p + LVD_DATA_ORIGINAL_WIDTH_FIELD_OFFSET,LVD_DATA_ORIGINAL_WIDTH_FIELD_OFFSET);
		memcpy(&originalDataDim[1], p + LVD_DATA_ORIGINAL_HEIGHT_FIELD_OFFSET,LVD_DATA_ORIGINAL_HEIGHT_FIELD_OFFSET);
		memcpy(&originalDataDim[2], p + LVD_DATA_ORIGINAL_DEPTH_FIELD_OFFSET,LVD_DATA_ORIGINAL_DEPTH_FIELD_OFFSET);
	}

	unsigned char * LVDHeader::Encode()
	{
		const auto p = buf.get();
		memcpy(p + (LVD_HEADER_MAGIC_FILED_OFFSET), (&magicNum), (LVD_HEADER_MAGIC_FILED_SIZE));
		memcpy(p + LVD_DATA_WIDTH_FIELD_OFFSET, &dataDim[0], LVD_DATA_WIDTH_FIELD_SIZE);
		memcpy(p + LVD_DATA_HEIGHT_FIELD_OFFSET, &dataDim[1], LVD_DATA_HEIGHT_FIELD_SIZE);
		memcpy(p + LVD_DATA_DEPTH_FIELD_OFFSET, &dataDim[2], LVD_DATA_DEPTH_FIELD_SIZE);
		memcpy(p + LVD_BLOCK_LOG_FILED_OFFSET, &blockLengthInLog, LVD_DATA_BLOCK_LENGTH_IN_LOG_FILED_SIZE);
		memcpy(p + LVD_BLOCK_PADDING_FIELD_OFFSET, &padding, LVD_DATA_PADDING_FIELD_SIZE);
		memcpy(p + LVD_DATA_ORIGINAL_WIDTH_FIELD_OFFSET, &originalDataDim[0], LVD_DATA_ORIGINAL_WIDTH_FIELD_OFFSET);
		memcpy(p + LVD_DATA_ORIGINAL_HEIGHT_FIELD_OFFSET, &originalDataDim[1], LVD_DATA_ORIGINAL_HEIGHT_FIELD_OFFSET);
		memcpy(p + LVD_DATA_ORIGINAL_DEPTH_FIELD_OFFSET, &originalDataDim[2], LVD_DATA_ORIGINAL_DEPTH_FIELD_OFFSET);
		return p;
	}
}

