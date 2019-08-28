
#include <iostream>
#include <libraryloader.h>
int main()
{
	ysl::LibraryReposity::GetLibraryRepo()->AddLibraries("plugins");
 	return 0;
}