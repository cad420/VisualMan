#include "graphicsbooster.h"

//#include <experimental/filesystem>
#include <iostream>
#include <filesystem>

namespace ysl
{
namespace vm
{
Booster *Booster::booster = nullptr;
std::mutex Booster::mutex;
std::thread::id Booster::threadId;
std::string Booster::exePath;

Booster::Booster( int argc, char **argv )
{
	Init();

	// Initialize path

	// const auto argv0 = std::filesystem::path(argv[0]);
	// auto p = std::filesystem::current_path();
	// auto pp = argv0.parent_path().string();
	// p.append(pp);
	// exePath = p.string();
}

Booster::~Booster()
{
	std::lock_guard<std::mutex> guard( mutex );
	booster = nullptr;
}

void Booster::Init()
{
	std::lock_guard<std::mutex> guard( mutex );
	if ( booster != nullptr ) {
		if ( threadId == std::this_thread::get_id() )
			throw std::runtime_error( "Booster instance has been created\n" );
		else
			throw std::runtime_error( "Booster instance has been created from another thread" );
	} else {
		booster = this;
		threadId = std::this_thread::get_id();
	}
}
}  // namespace vm
}  // namespace ysl
