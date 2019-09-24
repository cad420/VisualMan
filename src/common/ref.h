
#ifndef _REF_H_
#define _REF_H_

#include "object.h"
#include <atomic>
#include <cassert>

namespace ysl
{

	class IRefCounters
	{

	};

	template<typename Base,typename ValueType = int>
	class RefObjectBase :public Base
	{
		using CounterValueType = ValueType;
	public:
		CounterValueType AddRef()const { return ++counter; }
		CounterValueType Release()const { return --counter; }
		CounterValueType GetCount()const { return counter; }
		~RefObjectBase()
		{
			assert(counter == 0);
		}
	private:
		// reference
		mutable std::atomic<CounterValueType> counter = 0;
	};


	template<typename T>
	class Ref
	{
	private:
		T* rawPtr;
		using RefObjectType = RefObjectBase<T>;
		template<typename Other> friend class Ref<Other>;
	public:
		Ref(T* p = nullptr) :rawPtr(p)
		{

			static_assert(std::is_base_of<RefObjectBase<T>, T>::value);
			if (rawPtr != nullptr)
			{
				static_cast<RefObjectType*>(rawPtr)->AddRef();
			}
		}

		Ref(const Ref& r)
		{
			rawPtr = r.rawPtr;
			if (rawPtr)
			{
				static_cast<RefObjectType*>(rawPtr)->AddRef();
			}
		}

		Ref(const Ref&& r)noexcept 
		{
			rawPtr = r.Take();
		}

		void Attach(T* p) 
		{
			Release();
			rawPtr = p;
		}

		
		Ref & operator=(T* p)
		{
			if (rawPtr != p)
			{
				if (rawPtr)
				{
					static_cast<RefObjectType*>(rawPtr)->Release();
				}
				rawPtr = p;
				if (rawPtr) 
				{
					static_cast<RefObjectType*>(p)->AddRef();
				}
			}
			return *this;
		}

		Ref& operator=(const Ref& r)
		{
			return *this = r.rawPtr;
		}

		Ref& operator=(Ref&& r)noexcept 
		{
			if (rawPtr != r.rawPtr) 
			{
				Attach(r.Take());
			}
			return *this;
		}

		template<typename Derived,typename = typename std::enable_if<std::is_base_of<T,Derived>::value>::type>
		Ref& operator= (const Ref<Derived>& r) 
		{
			return *this = static_cast<T*>(r.rawPtr);
		}

		template<typename Derived,typename = typename std::enable_if<std::is_base_of<T,Derived>::value>::type>
		Ref& operator= (Ref<Derived>&& r)noexcept 
		{
			if (rawPtr != r.rawPtr)
			{
				Attach(r.Take());
			}
			return *this;
		}


:w
		//template<typename Other>
		//Other* As() 
		//{

		//}

	
		const T& operator*()const { return *rawPtr; }
		T& operator*() { return *rawPtr; }

		const T* operator->()const { return rawPtr; }
		T* operator->() { return rawPtr; }

		operator T* (){ return rawPtr; }
		operator const T* ()const { return rawPtr; }

		T* Get() { return rawPtr; }

		const T* Get()const { return rawPtr; }

		void Reset()
		{
			Release();
			rawPtr = nullptr;
		}

		T* Take() {
			T* p = rawPtr;
			rawPtr = nullptr;
			return p;
		}


		void Release()
		{
			if (rawPtr)
			{
				if (static_cast<RefObjectType*>(rawPtr)->Release() == 0)
				{
					delete rawPtr;
				}
			}
		}

		~Ref()
		{
			Release();
		}

	};

	// ==, != operators for two different Ref types with different orders

	template < class T, class U>
	inline bool operator==(const Ref<T>& A,
		const Ref<U>& B) 
	{
		return A.Get() == B.Get();
	}

	template < class T, class U>
	inline bool operator!=(const Ref<T>& A,
		const Ref<U>& B) {
		return A.Get() != B.Get();
	}

	template < class T, class U>
	inline bool operator==(const Ref<T>& A, U* B) {
		return A.Get() == B;

	}

	template < class T, class U>
	inline bool operator!=(const Ref<T>& A, U* B) {
		return A.Get() != B;

	}
	template<class T,class U>
	inline bool operator==(T* A, const Ref<U>& B) {
		return A == B.Get();

	}

	template < class T, class U>
	inline bool operator!=(T* A, const Ref<U>& B) {
		return A != B.Get();
	}


	// ==, != for nullptr and Ref<T> with different orders

	template < class T>
	inline bool operator==(std::nullptr_t A, const Ref<T>& B)
	{
		return !B;
	}

	template < class T>
	bool operator==(const Ref<T>& A, std::nullptr_t B)
	{
		return B == A;
	}

	template < class T>
	bool operator!=(std::nullptr_t A, const Ref<T>& B)
	{
		return !(A == B);
	}

	template < class T>
	bool operator!=(const Ref<T>& A, std::nullptr_t B)
	{
		return !(A == B);
	}
	//template<typename From> struct simplify_type;
	//template<typename T>struct simplify_type<Ref<T>> {
	//	using SimpleType = T *;
	//	static SimpleType GetSimplifiedValue(Ref<T>& val) {
	//		return val.Get();
	//	}
	//};

	//template<typename T>struct simplify_type<Ref<T>> {
	//	using SimpleType = T *;
	//	static SimpleType GetSimplifiedValue(const Ref<T>& val) {
	//		return val.Get();
	//	}
	//};



}

#endif