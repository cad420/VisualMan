
#ifndef _GRAPHICSBOOSTER_H_
#define _GRAPHICSBOOSTER_H_

#include <thread>
#include <mutex>
#include "graphictype.h"

namespace ysl
{
namespace vm
{
class VISUALMAN_EXPORT_IMPORT Booster
{
	static Booster *booster;
	static std::thread::id threadId;
	static std::mutex mutex;

public:
	Booster( int argc, char **argv );
	~Booster();
	//static Booster * VISUALMAN_EXPORT_IMPORT GetInstance(){return booster;}
	//static std::string VISUALMAN_EXPORT_IMPORT GetExePatah() { return exePath; }
private:
	void Init();
	static std::string exePath;
};
}  // namespace vm
}  // namespace ysl
#endif