#ifndef _VOLUME_UTILS_H_
#define _VOLUME_UTILS_H_

#include "../memory/blockarray.h"
#include "../mathematics/geometry.h"
#include "../core/spectrum.h"
#include "../utility/error.h"
#include "../mathematics/numeric.h"
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>
#include "appearance.h"
#include "interpulator.h"


//struct LVDFileHeader
//{
//	uint32_t magicNumber;	//  4
//	uint32_t vSize[3];		//	12
//	uint32_t oSize[3];		//  12
//	uint32_t repeat;		//  4
//	uint32_t logBlockSize;	//  4
//	char reserved[28];		// 28	 == 64bytes in total
//};

//#define WRITE_SINGLE_BLOCK

//struct LVDFileHeader
//{
//	uint32_t magicNumber;	//  4
//	uint32_t vSize[3];		//	12
//	uint32_t oSize[3];		//  12
//	uint32_t repeat;		//  4
//	uint32_t logBlockSize;	//  4
//	char reserved[28];		// 28	 == 64bytes in total
//};


/**
 * \brief This should be designed as a boundary-repeated block reader
 */




namespace ysl
{

}


#endif
