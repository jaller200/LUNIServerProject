#pragma once

#include "Queue.h"
#include <mutex>

template <class T>
class CrossThreadQueue : public Queue<T> {
	private:
		std::mutex mt;
		typedef Queue<T> base;
	public:
		CrossThreadQueue() { base::Queue(); }

		T Pop() {
			mt.lock();
			T r = base::Pop();
			mt.unlock();
			return r;
		}

		void Insert(const T& elem) {
			mt.lock();
			base::Insert(elem);
			mt.unlock();
		}
};