
#include <iostream>
#include <VMUtils/ref.hpp>  // Ref
#include <VMUtils/ieverything.hpp> // IEverything
#include <VMUtils/vmnew.hpp> // VM_NEW


struct MyAllocator:vm::IAllocator
{
	void * Alloc(size_t size) override
	{
		std::cout << "MyAlloc\n";
		return new uint8_t[size];
	}
	void Free(void* ptr) override
	{
		std::cout << "My delete\n";
		delete ptr;
	}
};


struct A:vm::IEverything
{
	virtual void Foo() = 0;
};

class AImpl:public vm::EverythingBase<A>
{
	int a = 0;
public:
	AImpl(vm::IRefCnt * cnt,int a):vm::EverythingBase<A>(cnt),a(a)
	{
		
	}
	void Foo() override
	{
		std::cout << a <<" "<<"Oops\n";
	}
};



int main()
{
	auto p = VM_NEW<AImpl>(1);
	vm::Ref<A> a = VM_NEW<AImpl>(1);
	vm::Ref<A> b(a);
	vm::Ref<A> c(a.Get());

	MyAllocator myAlloc;
	vm::Ref<A> d = VM_NEW<MyAllocator,AImpl>(&myAlloc,1);
	
	return 0;
}