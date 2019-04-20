
#include "libraryloader.h"

namespace ysl
{
	namespace util
	{

		LibraryReposity* LibraryReposity::instance = nullptr;

		LibraryReposity* LibraryReposity::GetInstance()
		{
			if (!instance)
				instance = new LibraryReposity;
			return instance;
		}

		void LibraryReposity::AddLibrary(const std::string& name)
		{
			if (repo.find(name) != repo.end())
				return;
			repo[name] = std::make_shared<Library>(name);
		}

		void* LibraryReposity::GetSymbol(const std::string& name) const
		{
			void * sym = nullptr;
			for (auto iter = repo.cbegin(); sym == nullptr && iter != repo.cend(); ++iter)
				sym = iter->second->Symbol(name);
			return sym;
		}

		void LibraryReposity::AddDefaultLibrary()
		{

		}

		bool LibraryReposity::Exists(const std::string& name) const
		{
			return repo.find(name) != repo.end();
		}

		LibraryReposity::LibraryReposity()
		{

		}
	}
}

