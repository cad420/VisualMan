
#ifndef _MATERIALREADER_H_
#define _MATERIALREADER_H_
#include <unordered_map>
#include <string>
#include "spectrum.h"

namespace ysl
{
	class MaterialReader
	{
	public:
		typedef std::unordered_map<std::string, RGBASpectrum> PropertyMap;
		typedef std::unordered_map<std::string, PropertyMap> MaterialLib;
	private:
		bool m_loaded;
		MaterialLib m_mLib;
		int test;
	public:
		explicit MaterialReader(const std::string& fileName);

		MaterialReader();

		bool loadFromFile(const std::string& fileName);

		PropertyMap& operator[](const std::string& name);

		bool isLoaded() const;
	};
}

#endif/*_MATERIAL_H_*/