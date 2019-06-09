
#ifndef _COUNTEDOBJECT_H_
#define _COUNTEDOBJECT_H_
#include <atomic>


namespace ysl
{

	template <typename ClassName>
	class CountedObject {
	public:
		static int ObjectCount();
		virtual ~CountedObject();
	protected:
		CountedObject();
		CountedObject(const CountedObject & obj);
	private:
		static std::atomic_uint64_t objectNum;
		void init();
	};

	template <typename ClassName>
	int CountedObject<ClassName>::ObjectCount()
	{
		return objectNum;
	}

	template <typename ClassName>
	CountedObject<ClassName>::~CountedObject()
	{
		--objectNum;
	}

	template <typename ClassName>
	CountedObject<ClassName>::CountedObject()
	{
		init();
	}

	template <typename ClassName>
	CountedObject<ClassName>::CountedObject(const CountedObject& obj)
	{
		init();
	}

	template <typename ClassName>
	void CountedObject<ClassName>::init()
	{
		++objectNum;
	}
}

#endif