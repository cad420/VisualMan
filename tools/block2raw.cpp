
#include <VMFoundation/rawreader.h>
#include <VMUtils/cmdline.hpp>
#include <VMFoundation/pluginloader.h>
#include <VMCoreExtension/i3dblockfileplugininterface.h>
#include <VMUtils/log.hpp>
#include <VMat/numeric.h>

int main( int argc, char **argv )
{
	cmdline::parser a;
	ysl::PluginLoader::GetPluginLoader()->LoadPlugins( "plugins" );
	a.add<std::string>( "if", 'i', "input file", true );
	a.add<std::string>( "of", 'o', "output file", false, "a.out" );
	a.add<std::string>( "fmt", 'f', "file format", true, ".lvd" );
	a.parse_check( argc, argv );
	const auto key = a.get<std::string>( "fmt" );
	auto reader = ysl::PluginLoader::CreatePlugin<I3DBlockFilePluginInterface>( key );
	if ( reader == nullptr ) {
		vm::println( "failed to load plugin for {}", key );
		return 0;
	}
	try {
		reader->Open( a.get<std::string>( "if" ) );
	} catch ( std::exception &e ) {
		::vm::println( "{}", e.what() );
	}

	const auto pageCount = reader->Get3DPageCount();

	vm::println( "Total blocks:{}", pageCount );

	::vm::Ref<IFileMapping> file;
#ifdef _WIN32
	file = ysl::PluginLoader::CreatePlugin<IFileMapping>( "windows" );
#else
	file = ysl::PluginLoader::CreatePlugin<IFileMapping>( "linux" );
#endif

	const auto ods = reader->GetDataSizeWithoutPadding();  // original data size

	try {
		if ( file->Open( a.get<std::string>( "of" ), ods.Prod(), FileAccess::ReadWrite, MapAccess::ReadWrite ) == false ) {
			::vm::println( "Failed to open file {}", a.get<std::string>( "of" ) );
		}
	} catch ( std::exception &e ) {
		::vm::println( "{}", e.what() );
	}

	void *data = file->FileMemPointer( 0, ods.Prod() );
	if ( data == nullptr ) {
		::vm::println( "Failed to map file" );
	}
	const auto p = reader->Get3DPageSizeInLog();	   // padding
	const auto ps = reader->Get3DPageSize();		   // page size
	const auto _ = ps - 2 * ysl::Size3( p, p, p );
	const ysl::Vec3i bsnp = ysl::Vec3i(_.x,_.y,_.z);  // block size without padding


	::vm::println( "Padding:{}", p );
	::vm::println( "PageSize:{}", ps );

	for ( int z = 0; z < ods.z; z++ ) {
		const auto zb = ysl::RoundUpDivide( z, bsnp.z );  // block x id
		for ( int y = 0; y < ods.y; y++ ) {				  // block y id
			const auto yb = ysl::RoundUpDivide( y, bsnp.y );
			for ( int x = 0; y < ods.x; x++ ) {
				const auto xb = ysl::RoundUpDivide( x, bsnp.x );																  // block z id
				const auto bid = ysl::Linear( { int( xb ), int( yb ), int( zb ) }, { pageCount.x,  pageCount.y} );  // linear block id
				const auto block = reader->GetPage( bid );
				const auto globalOffset = ysl::Linear( { x, y, z }, { ods.x, ods.y } );
				assert( globalOffset < ods.Prod() );
				const auto blockOffset = ysl::Linear( { x % bsnp.x + p, y & bsnp.y + p, z % bsnp.z + p }, { ps.x, ps.y } );
				assert( blockOffset < ps.Prod() );

				*( reinterpret_cast<char *>( data ) + globalOffset ) = *( reinterpret_cast<const char *>( block ) + blockOffset );
			}
		}
	}
	return 0;
}
