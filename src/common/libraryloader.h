
#ifndef _LIBRARY_LOADER_H_
#define _LIBRARY_LOADER_H_


#include <string>
#include "library.h"
#include <map>

namespace ysl
{
		class COMMON_EXPORT_IMPORT LibraryReposity
		{
		public:
			static LibraryReposity * GetLibraryRepo();

			/**
			 * \brief Add a library to the repository
			 * \param name
			 */
			void AddLibrary(const std::string & name);

			/**
			 * \brief  return the function pointer by the given name
			 */
			void * GetSymbol(const std::string & name)const;

			/**
			 * \brief  Add the default Library to repository
			 */
			void AddDefaultLibrary();

			/**
			 * \brief Check whether the library exists
			 */
			bool Exists(const std::string & name)const;

		private:
			LibraryReposity();
			static LibraryReposity * instance;
			std::map<std::string, std::shared_ptr<Library>> repo;
		};

}
#endif