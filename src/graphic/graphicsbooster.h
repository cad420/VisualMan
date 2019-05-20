
#ifndef _GRAPHICSBOOSTER_H_
#define _GRAPHICSBOOSTER_H_

#include <thread>
#include <mutex>

namespace ysl
{
	namespace vpl
	{
		class Booster
		{
			static Booster * booster;
			static std::thread::id threadId;
			static std::mutex mutex;
			

		public:
			Booster(int argc, char ** argv);

			~Booster();

			static Booster * GetInstance(){return booster;}

			static std::string GetExePatah() { return exePath; }

		private:
			void Init();
			static std::string exePath;

		};
	}
}
#endif