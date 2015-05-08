#pragma once

//simple and basic reference by Aldrigo Raffaele
#include "Common.h"
#include "Exceptions.h"
#include "MemWrp.h"

template< class T >
class Ref {
	private:
	MemWrp* wrp = NULL;
	public:
	Ref() {}
	Ref(T* ptr) {
		wrp = NULL;
		if (ptr != NULL) wrp = new MemWrp(ptr);
	}
	Ref(int ptr) {
		if (ptr == NULL) {
			wrp = NULL;
			return;
		}
		wrp = new MemWrp((void*)ptr);
	}
	Ref(const Ref<T>& r) {
		wrp = r.wrp;
		if (wrp != NULL) wrp->AddRef();
	}
	Ref(MemWrp* w) {
		wrp = w;
		wrp->AddRef();
	}

	T* Get() const {
		if (wrp == NULL) return NULL;
		return (T*)wrp->content;
	}

	template<class F> bool is() { return dynamic_cast<F*>(Get()) != NULL; }

	template<class F> Ref<F> as() {
		if (!this->is<F>()) return Ref<F>::Ref();
		return Ref<F>::Ref(this->wrp);
	}

	Ref<T>& operator=(Ref<T>& rhs) {
		if (this == &rhs) return *this;
		if (rhs.wrp == NULL && wrp != NULL && wrp->RemoveRef() == 0) delete wrp;
		wrp = rhs.wrp;
		if (wrp != NULL) wrp->AddRef();
		return *this;
	}

	Ref<T>& operator=(T* rhs) {
		if (Get() == rhs) return *this;
		if (rhs == NULL && wrp != NULL && wrp->RemoveRef() == 0) delete wrp;
		else if (rhs != NULL) wrp = new MemWrp(rhs);
		return *this;
	}

	bool operator==(const Ref<T>& other) const { return (this->Get() == other.Get()); }

	bool operator==(const int other) const { return (this->Get() == (void*)other); }

	bool operator!=(const Ref<T>& other) const { return !(*this == other); }

	bool operator!=(const int other) const { return !(*this == other); }

	T* operator->() {
		if (this->Get() != NULL) return this->Get();
		throw NullPointerException();
	}
	const T* operator->() const {
		if (this->Get() != NULL) return this->Get();
		throw NullPointerException();
	}

	destructor Ref() { if (wrp != NULL && wrp->RemoveRef() == 0) delete wrp; }
};