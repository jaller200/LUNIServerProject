#pragma once

#include "Object.h"
#include "Exceptions.h"

// This is an enumerator for an element T

// For some reason, this really isn't used by anything else
// in the LUNIServer source... at least, not that I can find...
template< class T >
class IEnumerator : public Object {
	public:
		virtual bool MoveNext() = 0; // Can you move to the next object?
		virtual T* Current() = 0; // Get the current object
		virtual void Reset() = 0; // Reset the enumerator
};

template< class T >
class IEnumerable : public Object {
	public:
		virtual IEnumerator<T>* GetEnumerator() = 0; // Get the current enumerator
};


// Enumerator through a collection of objects
template<class InputEnumerator>
void foreach(IEnumerator<InputEnumerator>* first, void(*fn) (InputEnumerator)) {
	while (first->MoveNext()) {
		fn(first->*Current;);
	}
}