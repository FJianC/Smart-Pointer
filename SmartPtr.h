#pragma once
#include <mutex>

template <class T>
class SmartPtr
{
public:
	//构造函数， 设定T * ptr的值，并将引用计数设为1
	SmartPtr(T * p):pointer(NULL),counter(NULL)
	{
		*this = p;
	}
	//赋值函数
	SmartPtr<T>& operator=(T *p)
	{
		std::mutex m;
		m.lock()
		Decrease();
		if (p)
		{
			pointer = p;
			counter = new int(1);
		}
		else
		{
			pointer = NULL;
			counter = NULL;
		}
		m.unlock();
		return *this;
	}
	//拷贝构造函数
	SmartPtr(const SmartPtr<T> &p):pointer(p.pointer),counter(p.counter)
	{
		Decrease();
	}
	//拷贝函数
	SmartPtr<T> & operator = (SmartPtr<T> &p)
	{
		std::mutex m;
		m.lock();
		if (this != &p)
		{
			Decrease();
			pointer = p.pointer;
			counter = p.counter;
			Increase();
		}
		m.unlock();
		return *this;
	}

	~SmartPtr()
	{
		std::mutex m;
		m.lock();
		Decrease();
		m.unlock();
	}

	//取出指针
	T getValue()
	{
		return *pointer;
	}

	//默认构造函数
	SmartPtr() :pointer(NULL), counter(NULL)
	{
	}

	//隐式转换bool
	operator bool()const
	{
		return counter != NULL;
	}

	//取出指针
	T* operator->()const
	{
		return pointer;
	}
private:
	int * counter;//引用计数
	T * pointer;//保存的指针

	//引用计数加一
	void Increase()
	{
		if (counter) ++(*counter);
	}
	//引用计数减一
	void Decrease()
	{
		if (counter && --(*counter) == 0) {
			delete pointer;
			delete counter;
			pointer = NULL;
			counter = NULL;
		}
	}

public:

	//SmartPtr的所有实例化，都是SmartPtr的友元，注意模板参数X不能相同
	template <typename X> friend class SmartPtr;

	//父类子类
	template<typename C>
	SmartPtr(const SmartPtr<C> &p) :pointer(p.pointer), counter(p.counter)
	{
		std::mutex m;
		m.lock();
		Increase();
		m.unlock();
	}
	template<typename C>
	SmartPtr<T>& operator=(const SmartPtr<C> &p)
	{
		std::mutex m;
		m.lock();
		Decrease();
		pointer = p.pointer;
		counter = p.counter;
		Increase();
		m.unlock();
		return *this;
	}
	
	//父子类的类型转换
	template<typename C>
	SmartPtr<C> Cast()
	{
		std::mutex m;
		m.lock();
		C* converted = dynamic_cast<C*>(pointer);
		SmartPtr<C> result;
		if (counter)
		{
			result.pointer = converted;
			result.counter = counter;
			Increase();
		}
		m.unlock();
		return result;
	}
};

