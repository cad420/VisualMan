
#include "materialreader.h"
#include <sstream>

namespace ysl
{
	MaterialReader::MaterialReader(const std::string& fileName)
	{
		m_loaded = loadFromFile(fileName);
	}

	MaterialReader::MaterialReader(): m_loaded(false)
	{
	}

	bool ysl::MaterialReader::loadFromFile(const std::string& fileName)
	{
		std::ifstream fileIn(fileName, std::ifstream::in);
		if (fileIn.is_open() == false)
			return m_loaded = false;
		std::string line;
		std::string prevMtlName;
		while (getline(fileIn, line))
		{
			line = line.substr(0, line.find_first_of('#'));
			std::stringstream ss(line);
			std::string keyword;
			ss >> keyword;
			if (keyword == "newmtl") //a new material defination
			{
				ss >> prevMtlName;
			}
			else
			{
				float value;
				int i = 0;
				while (ss >> value)
				{
					m_mLib[prevMtlName][keyword][i] = value;
					i++;
				}
			}
		}
		return m_loaded = true;
	}

	MaterialReader::PropertyMap& MaterialReader::operator[](const std::string& name)
	{
		return m_mLib[name];
	}

	bool MaterialReader::isLoaded() const
	{
		return m_loaded;
	}
}

