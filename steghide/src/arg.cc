#include "arg.h"

template<class T> Arg<T>::Arg (void)
{
	set = false ;
}

template<class T> Arg<T>::Arg (T v, bool setbyuser)
{
	Arg () ;
	setValue (v, setbyuser) ;
}

template<class T> T Arg<T>::getValue (void)
{
	return value ;
}

template<class T> void Arg<T>::setValue (T v, bool setbyuser)
{
	value = v ;
	if (setbyuser) {
		set = true ;
	}

	return ;
}

template<class T> bool Arg<T>::is_set (void)
{
	return set ;
}
