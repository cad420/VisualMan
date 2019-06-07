
#ifndef _IMAGE_H_
#define _IMAGE_H_
#include <string>

#include "../graphic/graphictype.h"

#include "buffer.h"


namespace ysl
{
	/**
	 * \brief This class is a representation for most common texture format in 3D Graphics.
	 *  
	 */
	class Image
	{
	public:
		Image(const std::string & fileName);

	private:
		Ref<LocalBuffer> buffer;
		ImageFormat format;
		ImageType type;

	};
}

#endif
