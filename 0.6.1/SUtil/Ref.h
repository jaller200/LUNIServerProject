#pragma once

// Simple and basic reference by Aldrigo Raffaele
// Updated documentation by Jon002
#include "Common.h"
#include "Exceptions.h"
#include "MemWrp.h"

// Basically this creates a reference (pointer) to a class or object T
// using MemWrp
template< class T >
class Ref {
	private:
		MemWrp* wrp = NULL; // Create a variable of MemWrp and set it to NULL

	public:
		Ref() {} // Initializer

		// Create a reference from data (basically, you can use Ref<SomeObject> to
		// create a reference to the object SomeObject wrapped in a MemWrp)
		Ref(T* ptr) {
			wrp = NULL; // Set the wrp to NULL (so it isn't reused with old data in it)
			if (ptr != NULL) wrp = new MemWrp(ptr); // Create a new MemWrp pointing to an object
		}

		// Create a reference 
		Ref(int ptr) {
			if (ptr == NULL) {
				wrp = NULL;
				return;
			}

			// Create a new MemWrp with a void pointer
			wrp = new MemWrp((void*)ptr);
		}

		// Create a reference from another Ref
		Ref(const Ref<T>& r) {
			wrp = r.wrp; // Set the wrp to the existing refernce's wrp

			// If wrp is not NULL, add the reference to the wrp
			if (wrp != NULL) wrp->AddRef();
		}

		// Create a reference directly with an instance of MemWrp
		Ref(MemWrp* w) {
			wrp = w;
			wrp->AddRef();
		}

		// Get the object that is being referenced
		T* Get() const {
			// If nothing is being wrapped, return NULL
			if (wrp == NULL) return NULL;

			// Else, return the wrapped content
			return (T*)wrp->content;
		}
	
		// Check to see if a class equals the class that is referenced
		template<class F> bool is() { return dynamic_cast<F*>(Get()) != NULL; }

		// Return a reference to a class using a new command
		template<class F> Ref<F> as() {
			if (!this->is<F>()) return Ref<F>::Ref();
			return Ref<F>::Ref(this->wrp);
		}

		// = operator basically sets a reference equal to an object
		Ref<T>& operator=(Ref<T>& rhs) {
			if (this == &rhs) return *this; // If the reference is already pointing to the object, return the reference
			if (rhs.wrp == NULL && wrp != NULL && wrp->RemoveRef() == 0) delete wrp;
			wrp = rhs.wrp;
			if (wrp != NULL) wrp->AddRef();
			return *this;
		}

		// Set a reference by using the = operator to an object to be wrapped
		Ref<T>& operator=(T* rhs) {
			if (Get() == rhs) return *this; // If the reference already points to this object, return the reference
			if (rhs == NULL && wrp != NULL && wrp->RemoveRef() == 0) delete wrp; // If the object is null, and the MemWrp isn't, and removing the current wrp works, delete the wrp
			else if (rhs != NULL) wrp = new MemWrp(rhs); // Otherwise, create a new MemWrp wrapping the object
			return *this; // Return a new reference
		}

		// Check for equality between references
		bool operator==(const Ref<T>& other) const { return (this->Get() == other.Get()); }

		// Check for equality between a references and a void reference
		bool operator==(const int other) const { return (this->Get() == (void*)other); }

		// Check for non-equality between two references
		bool operator!=(const Ref<T>& other) const { return !(*this == other); }

		// Check for non-equality between a references and a void references
		bool operator!=(const int other) const { return !(*this == other); }

		// Get the object that is wrapped
		T* operator->() {
			if (this->Get() != NULL) return this->Get(); // Get the object that is wrapped
			throw NullPointerException(); // Otherwise, throw an exception
		}

		// Get a const of the object that is wrapped
		const T* operator->() const {
			if (this->Get() != NULL) return this->Get(); // Get the object that is wrapped
			throw NullPointerException(); // Otherwise, throw an exception
		}

		// This is the destructor
		destructor Ref() { if (wrp != NULL && wrp->RemoveRef() == 0) delete wrp; }
};