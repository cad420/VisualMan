
#include <VMFoundation/rawreader.h>
#include <VMUtils/cmdline.hpp>
#include <VMFoundation/pluginloader.h>
#include <VMCoreExtension/i3dblockfileplugininterface.h>
#include <VMUtils/log.hpp>
#include <VMat/numeric.h>

//#define USE_MAPPING

int main( int argc, char **argv )
{
	cmdline::parser a;
	vm::PluginLoader::GetPluginLoader()->LoadPlugins( "plugins" );
	a.add<std::string>( "if", 'i', "input file", true );
	a.add<std::string>( "of", 'o', "output file", false, "a" );
	a.add<std::string>( "fmt", 'f', "file format", true, ".lvd" );
	a.parse_check( argc, argv );
	const auto key = a.get<std::string>( "fmt" );
	auto reader = vm::PluginLoader::CreatePlugin<I3DBlockFilePluginInterface>( key );
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

#ifdef USE_MAPPING
	::vm::Ref<IFileMapping> file;
#ifdef _WIN32
	file = ysl::PluginLoader::CreatePlugin<IFileMapping>( "windows" );
#else
	file = ysl::PluginLoader::CreatePlugin<IFileMapping>( "linux" );
#endif
#endif

	const auto ods = reader->GetDataSizeWithoutPadding();  // original data size

	std::stringstream ss;
	ss << a.get<std::string>( "of" ) << "_" << ods.x << "_" << ods.y << "_" << ods.z << ".raw";
	std::string out;
	ss >> out;

	::vm::println( "Data Size:{}", ods );
#ifdef USE_MAPPING
	try {
		if ( file->Open( out, ods.Prod(), FileAccess::ReadWrite, MapAccess::ReadWrite ) == false ) {
			::vm::println( "Failed to open file {}", a.get<std::string>( "of" ) );
		}
	} catch ( std::exception &e ) {
		::vm::println( "{}", e.what() );
	}

	void *data = file->FileMemPointer( 0, ods.Prod() );
	if ( data == nullptr ) {
		::vm::println( "Failed to map file" );
	}
#endif

	const auto p = reader->GetPadding();	  // padding
	const auto bs = reader->Get3DPageSize();  // page size
	const auto _ = bs - 2 * vm::Size3( p, p, p );
	const vm::Vec3i bsnp = vm::Vec3i( _.x, _.y, _.z );  // block size without padding

	::vm::println( "PageSize: {}", bs );
	::vm::println( "PageSizeNoPadding: {}", bsnp );

	std::ofstream fileStream( out, std::ios::binary );
	if ( fileStream.is_open() == false ) {
		::vm::println( "Failed to open file: {}", out );
		return 1;
	}
	const auto bufSize = ods.x * ods.y * bsnp.z;
	std::vector<char> buf;
	buf.reserve( bufSize );
	::vm::println( "Buf Size: {}", bufSize );

	auto totalBytes = 0;
	for ( int zb = 0; zb < pageCount.z; zb++ ) {
		bool end = false;
		int lastSlice = bsnp.z;
		size_t realBuf = bufSize;
		if (zb == pageCount.z - 1) 
		{
			lastSlice = ods.z % bsnp.z;
			realBuf = ods.x * ods.y * lastSlice;
		}
		for ( int yb = 0; yb < pageCount.y; yb++ ) {
			for ( int xb = 0; xb < pageCount.x; xb++ ) {
				// for each block, read the all the strip memory region of x direction
				const auto bid = vm::Linear( { xb, yb, zb }, { pageCount.x, pageCount.y } );
				const auto block = reinterpret_cast<const char *>( reader->GetPage( bid ) );
				for ( int sz = 0; sz < lastSlice; sz++ ) {
					for ( int sy = 0; sy < bsnp.y; sy++ ) {
						const auto blockBegin = vm::Linear( { p, p + sy, p + sz }, { bs.x, bs.y } );
						const auto bufBegin = vm::Linear( { xb * bsnp.x, yb * bsnp.y, sz }, { ods.x, ods.y } );
						memcpy( buf.data() + bufBegin, block + blockBegin, bsnp.x );
					}
				}
			}
		}
		totalBytes += realBuf;
		fileStream.write( buf.data(), realBuf );
		vm::println( "Level: {}, Bytes: {} = {} x {} x {}", zb ,realBuf,ods.x,ods.y,lastSlice);
	}
	vm::println( "Written bytes in total: {}", totalBytes );

	
#ifdef USE_MAPPING
	for ( int z = 0; z < ods.z; z++ ) {
		const auto zb = z / bsnp.z;			 // block x id
		for ( int y = 0; y < ods.y; y++ ) {  // block y id
			const auto yb = y / bsnp.y;
			for ( int x = 0; x < ods.x; x++ ) {
				const auto xb = x / bsnp.x;

				const auto bid = ysl::Linear( { int( xb ), int( yb ), int( zb ) }, { pageCount.x, pageCount.y } );  // linear block id
				const auto block = reader->GetPage( bid );
				const auto globalOffset = ysl::Linear( { x, y, z }, { ods.x, ods.y } );
				assert( globalOffset < ods.Prod() );
				const ysl::Point3i pos = { x % bsnp.x + p, y % bsnp.y + p, z % bsnp.z + p };
				const auto blockOffset = ysl::Linear( pos, { bs.x, bs.y } );
				assert( blockOffset < bs.Prod() );

				*( reinterpret_cast<char *>( data ) + globalOffset ) = *( reinterpret_cast<const char *>( block ) + blockOffset );
			}
		}
	}
	file->Close();
#endif

	return 0;
}
