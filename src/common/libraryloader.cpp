
#include "libraryloader.h"
#include <iostream>
#include <experimental/filesystem>
#include <regex>

namespace ysl
{

	LibraryReposity* LibraryReposity::instance = nullptr;

	LibraryReposity* LibraryReposity::GetLibraryRepo()
	{
		if (!instance)
			instance = new LibraryReposity;
		return instance;
	}

	void LibraryReposity::AddLibrary(const std::string& name)
	{
		if (repo.find(name) != repo.end())
			return;

		std::string fullName;
#ifdef _WIN32
		fullName = name + ".dll";
#elif defined(__MACOSX__) || defined(__APPLE__)
		fullName = "lib" + name + ".dylib";		// mac extension
		#elof defined(__linux__)
			fullName = "lib" + name + ".so";			// linux extension
#endif
		try
		{
			auto lib = std::make_shared<Library>(fullName);
			repo[name] = lib;
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}

	}

	void LibraryReposity::AddLibraries(const std::string& directory)
	{
		namespace fs = std::experimental::filesystem;
		for (auto& lib : fs::directory_iterator(directory))
		{
			const auto fullName = lib.path().filename().string();
			std::regex reg;
			std::string libName = fullName.substr(0, fullName.find_last_of('.'));
#ifdef _WIN32
			reg = std::regex(R"(.+\.dll$)");
			if (std::regex_match(fullName, reg) == false)continue;
#elif defined(__MACOSX__) || defined(__APPLE__)
			reg = std::regex(R"(^lib.+\.dylib$)");
			if (std::regex_match(fullName, reg) == false)continue;
			libName = libName.substr(3, fullName.find_last_of('.') - 3);
#elif defined(__linux__)
			reg = std::regex(R"(^lib.+\.so$)");
			if (std::regex_match(fullName, reg) == false)continue;
			libName = libName.substr(3, fullName.find_last_of('.') - 3);
#endif	/*defined(__MACOSX__) || defined(__APPLE__)*/
			
			try
			{
				auto rp = std::make_shared<Library>(lib.path().string());
				repo[libName] = rp;
			}
			catch (std::exception& e)
			{
				std::cerr << e.what() << std::endl;
			}

		}
	}

	void* LibraryReposity::GetSymbol(const std::string& name) const
	{
		void* sym = nullptr;
		for (auto iter = repo.cbegin(); sym == nullptr && iter != repo.cend(); ++iter)
			sym = iter->second->Symbol(name);
		return sym;
	}

	void* LibraryReposity::GetSymbol(const std::string& libName, const std::string& symbolName) const
	{
		void* sym = nullptr;
		auto iter = repo.find(libName);
		if(iter != repo.end())
		{
			sym = iter->second->Symbol(symbolName);
		}
		return sym;
	}

	void LibraryReposity::AddDefaultLibrary()
	{
		
	}

	bool LibraryReposity::Exists(const std::string& name) const
	{
		return repo.find(name) != repo.end();
	}

	const std::map<std::string, std::shared_ptr<Library>>& LibraryReposity::GetLibRepo() const
	{
		return repo;
	}

	LibraryReposity::LibraryReposity()
	{

	}

}

