#pragma once

template<typename T>

class Singleton
{
public:
	Singleton() {}
	virtual ~Singleton() {}

	Singleton(const Singleton& s) {}

	static T& GetInstance()
	{
		if (pInstance_ == nullptr)
		{
			pInstance_ = new T;
			atexit(destroy); // ���� �Լ� ���
		}

		return *pInstance_;
	}

	static T* GetInstancePtr()
	{
		if (pInstance_ == nullptr)
		{
			pInstance_ = new T;
			atexit(destroy); // ���� �Լ� ���
		}

		return pInstance_;
	}

	static void destroy()
	{
		if (pInstance_ != nullptr)
		{
			delete pInstance_;
			// ��ü �Ҹ�
		}
	}

private:
	static T* pInstance_;
};

template <typename T> T* Singleton <T>::pInstance_;
