#ifndef VOLUME_H_
#define VOLUME_H_
#include <string>
#include <list>
#include <mutex>
#include <map>


#include "lvdreader.h"
#include "volume_utils.h"
#include "../memory/blockarray.h"
#include "../mathematics/geometry.h"
//#include "../mathematics/arithmetic.h"

struct VolumeFormat;

// Enum Type
enum VoxelType { UInt8, Float32 };
enum VoxelFormat { Grayscale, RGB, RGBA };



#endif