#ifndef VOLUME_H_
#define VOLUME_H_
#include <string>
#include <mutex>

#include "lvdreader.h"

struct VolumeFormat;

// Enum Type
enum VoxelType { UInt8, Float32 };
enum VoxelFormat { Grayscale, RGB, RGBA };



#endif