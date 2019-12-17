
#include <iostream>
#include <fstream>
#include <VMUtils/cmdline.hpp>
#include <VMFoundation/pluginloader.h>
#include <VMat/numeric.h>
#include <VMUtils/log.hpp>
#include <VMCoreExtension/i3dblockfileplugininterface.h>

int main( int argc, char **argv )
{
	cmdline::parser a;
	vm::PluginLoader::GetPluginLoader()->LoadPlugins( "plugins" );

	a.add<std::string>( "if", 'i', "input file", true );
	a.add<std::string>( "of", 'o', "output file", false, "a.out" );
	a.add<std::string>( "fmt", 'f', "file format", true, ".lvd" );

	a.add<size_t>( "block", 'b', "flat block index", false, 0 );
	a.add<int>( "xb", 'x', "x block id", false, -1 );
	a.add<int>( "yb", 'y', "y block id", false, -1 );
	a.add<int>( "zb", 'z', "z block id", false, -1 );
	a.parse_check( argc, argv );

	const auto key = a.get<std::string>( "fmt" );

	const auto x = a.get<int>( "xb" );
	const auto y = a.get<int>( "yb" );
	const auto z = a.get<int>( "zb" );

	auto reader = vm::PluginLoader::CreatePlugin<I3DBlockFilePluginInterface>( key );
	if ( reader == nullptr ) {
		vm::println( "failed to load plugin for {}",key );
		return 0;
	}
	try 
	{
		reader->Open( a.get<std::string>( "if" ) );
	} catch ( std::exception &e ) {
		std::cout << e.what() << std::endl;
	}

	const auto blockBytes = reader->Get3DPageSize().Prod();
	const auto pageCount = reader->Get3DPageCount();

	vm::Point3i coord;
	size_t p = 0;
	if ( x != -1 && y != -1 && z != -1 ) 
	{
		coord = vm::Point3i( x, y, z );
		p = vm::Linear( coord, { pageCount.x, pageCount.y } );
	} else {
		p = a.get<size_t>( "block" );
	}
	if ( p >= pageCount.Prod() ) 
	{
		const auto P = vm::Dim( p, {pageCount.x,pageCount.y} );
		vm::println( "block id is out of range: request is {} ({}), but {} ({}) in total.", p,P, pageCount.Prod() ,pageCount);
		return 0;
	}


	vm::println( "Block Dim:", reader->Get3DPageCount() );
	
	std::unique_ptr<char[]> buf( new char[ blockBytes ] );

	std::memcpy( buf.get(), reader->GetPage(p), blockBytes );

	std::ofstream out( a.get<std::string>( "of" ), std::ios::binary );
	out.write( buf.get(), blockBytes );
	return 0;
}