/******************************************************************************/
/*!
\file		Singleton.h
\project	MetroManiac
\primary author Goh Koon Shen

All content \@ 2016 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

template<typename T, unsigned N = 0>
class Singleton
{
	static bool init;
	// Prevents copies
	Singleton(Singleton const &) = delete;
	Singleton(Singleton &&) = delete;
	Singleton & operator=(Singleton const &) = delete;
	Singleton & operator=(Singleton &&) = delete;

	// virtual void SingletonInit_() {};
	// virtual void SingletonDes_() {};

	// struct to call the init functions of the singleton
	struct SingletonInitialiser
	{
		SingletonInitialiser()
		{
			// singleton.SingletonInit_();
		}
		~SingletonInitialiser()
		{
			// singleton.SingletonDes_();
		}
		T singleton;
	};

public:
	Singleton()
	{
		// Logging cannot be used as it is a singleton
		assert(// Reinitialization of Singleton
			!init);
		init = true;
	}

	static T& GetInstance()
	{
		static SingletonInitialiser initialiser;
		return initialiser.singleton;
	};
};

template<typename T, unsigned N>
bool Singleton<T, N>::init = false;

template<typename T>
void SingletonCaller(void(T::*m_func)(void))
{
  (T::GetInstance().*m_func)();
}