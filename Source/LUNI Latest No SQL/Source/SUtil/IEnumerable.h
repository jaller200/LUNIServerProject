#pragma once

#include "Object.h"
#include "Exceptions.h"

template< class T >
class IEnumerator : public Object {
	public:
		virtual bool MoveNext() = 0;
		virtual T* Current() = 0;
		virtual void Reset() = 0;
};

template< class T >
class IEnumerable : public Object {
	public:
		virtual IEnumerator<T>* GetEnumerator() = 0;
};


template<class InputEnumerator>
void foreach(IEnumerator<InputEnumerator>* first, void(*fn) (InputEnumerator)) {
	while (first->MoveNext()) {
		fn(first->*Current;);
	}
}