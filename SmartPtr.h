#pragma once
#include <mutex>

template <class T>
class SmartPtr
{
public:
	//���캯���� �趨T * ptr��ֵ���������ü�����Ϊ1
	SmartPtr(T * p):pointer(NULL),counter(NULL)
	{
		*this = p;
	}
	//��ֵ����
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
	//�������캯��
	SmartPtr(const SmartPtr<T> &p):pointer(p.pointer),counter(p.counter)
	{
		Decrease();
	}
	//��������
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

	//ȡ��ָ��
	T getValue()
	{
		return *pointer;
	}

	//Ĭ�Ϲ��캯��
	SmartPtr() :pointer(NULL), counter(NULL)
	{
	}

	//��ʽת��bool
	operator bool()const
	{
		return counter != NULL;
	}

	//ȡ��ָ��
	T* operator->()const
	{
		return pointer;
	}
private:
	int * counter;//���ü���
	T * pointer;//�����ָ��

	//���ü�����һ
	void Increase()
	{
		if (counter) ++(*counter);
	}
	//���ü�����һ
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

	//SmartPtr������ʵ����������SmartPtr����Ԫ��ע��ģ�����X������ͬ
	template <typename X> friend class SmartPtr;

	//��������
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
	
	//�����������ת��
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

