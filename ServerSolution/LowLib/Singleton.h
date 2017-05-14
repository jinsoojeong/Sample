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
			atexit(destroy); // 종료 함수 등록
		}

		return *pInstance_;
	}

	static T* GetInstancePtr()
	{
		if (pInstance_ == nullptr)
		{
			pInstance_ = new T;
			atexit(destroy); // 종료 함수 등록
		}

		return pInstance_;
	}

	static void destroy()
	{
		if (pInstance_ != nullptr)
		{
			delete pInstance_;
			// 객체 소멸
		}
	}

private:
	static T* pInstance_;
};

template <typename T> T* Singleton <T>::pInstance_;
