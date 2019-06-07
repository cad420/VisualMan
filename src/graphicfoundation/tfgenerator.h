
#ifndef _TFGENERATOR_H_
#define _TFGENERATOR_H_
#include <geometry.h>
#include <functional>
#include <vector>

#include "spectrum.h"
#include "interpulator.h"
#include "graphicfoundation.h"

namespace ysl
{
	class MappingKey;

	class GRAPHIC_FOUNDATION_EXPORT_IMPORT TransferFunction :public ColorInterpulator
	{
	public:
		using Callback = std::function<void(TransferFunction* tf)>;
		TransferFunction() = default;
		TransferFunction(const std::string & fileName);
		~TransferFunction();
		void AddMappingKey(const MappingKey & key);
		void UpdateMappingKey(const Point2f & at, const Point2f & to, const Vector2f & scale = { 1.0f,1.0f });
		void UpdateMappingKey(const Point2f & at, const MappingKey & key, const Vector2f & scale = { 1.0f,1.0f });
		void AddUpdateCallBack(const Callback & func);
		Point2f KeyPosition(int i, Float sx, Float sy)const;
		int HitAt(const Point2f & at, const Vector2f & scale);
		void UpdateMappingKey(int index, const RGBASpectrum& color);
	private:
		std::vector<Callback> callbacks;
		std::function<bool(const MappingKey & m1, const MappingKey & m2)> compareFunc;
		void Notify();
		static Point2f TransToPos(const MappingKey & key);
	};
}


#endif
