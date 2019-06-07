
#include "tfgenerator.h"

namespace  ysl
{
	TransferFunction::TransferFunction(const std::string& fileName) :ColorInterpulator(fileName)
	{
		compareFunc = [](const MappingKey& m1, const MappingKey & m2)->bool {return m1.Intensity() < m2.Intensity(); };
	}

	TransferFunction::~TransferFunction()
	{

	}

	void TransferFunction::AddMappingKey(const MappingKey& key)
	{
		keys.push_back(key);
		std::sort(keys.begin(), keys.end(), compareFunc);
		Notify();
	}

	void TransferFunction::UpdateMappingKey(const Point2f& at, const Point2f & to, const Vector2f & scale)
	{
		auto index = -1;
		for (auto i = 0; i < keys.size(); i++)
		{
			const auto k = keys[i];
			const auto p = TransToPos(k);
			const Point2f coords = { p.x*scale.x,p.y*scale.y };
			if ((coords - at).LengthSquared() <= 5)
			{
				index = i;
				std::cout << "Hit:" << i << std::endl;
				break;
			}
		}

		if (index == -1)
		{
			///TODO:: Adding a new mapping key if there is no key at the click pos
			return;
		}

		const auto newAlpha = to.y / scale.y;
		const auto newIntensity = to.x / scale.x;
		keys[index].SetRightAlpha(newAlpha);
		keys[index].SetLeftAlpha(newAlpha);
		keys[index].SetIntensity(newIntensity);
		const auto & key = keys[index];

		// Keep the keys which are left of the selected key also being left 
		for (auto i = 0; i < index; i++)
		{
			auto & k = keys[i];
			if (k.Intensity() > key.Intensity())
				k.SetIntensity(key.Intensity());
		}

		// Keep the keys which are right of the selected key also being right 
		for (auto i = index + 1; i < keys.size(); i++)
		{
			auto & k = keys[i];
			if (k.Intensity() < key.Intensity())
				k.SetIntensity(key.Intensity());
		}

		Notify();
	}

	void TransferFunction::UpdateMappingKey(const Point2f& at, const MappingKey& key, const Vector2f & scale)
	{

		Notify();
	}

	void TransferFunction::AddUpdateCallBack(const Callback& func)
	{
		callbacks.push_back(func);
	}

	Point2f TransferFunction::KeyPosition(int i, Float sx, Float sy) const
	{
		const auto p = TransToPos(keys[i]);
		return { p.x*sx,p.y*sy };
	}

	int TransferFunction::HitAt(const Point2f& at, const Vector2f& scale)
	{
		for (auto i = 0; i < keys.size(); i++)
		{
			const auto k = keys[i];
			const auto p = TransToPos(k);
			const Point2f coords = { p.x*scale.x,p.y*scale.y };
			if ((coords - at).LengthSquared() <= 5)
				return i;
		}
		return -1;
	}

	void TransferFunction::UpdateMappingKey(int index, const RGBASpectrum& color)
	{
		keys[index].SetLeftColor(color);
		keys[index].SetRightColor(color);
		Notify();
	}

	void TransferFunction::Notify()
	{
		for (const auto &c : callbacks)
			c(this);
	}

	Point2f TransferFunction::TransToPos(const MappingKey& key)
	{
		return { key.Intensity(),key.LeftColor()[3] };		//(x, y) = (intensity, alpha)
	}
}