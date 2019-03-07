
#ifndef _FILM_H_
#define _FILM_H_
#include "../mathematics/geometry.h"
#include "../core/spectrum.h"


namespace ysl
{
	class Film {
	public:
		const Vector2i resolution;
		struct Pixel
		{
			unsigned char v[4];
		};
		std::unique_ptr<Pixel[]> pixel;
		Film(const Vector2i & resolution);
	};

}



#endif