#pragma once

#include "Queue.h"
#include <mutex>

// Class for writing strings to the console from a thread,
// as it makes sure there are no interruptions when writing
// (from another thread)
template <class T>
class CrossThreadQueue : public Queue<T> {
	private:
		std::mutex mt; // Initialize a private std::mutex
		typedef Queue<T> base; // Get the base Queue of element T
	public:
		CrossThreadQueue() { base::Queue(); } // Initializer

		T Pop() {
			mt.lock(); // Lock the mutex so that the thread is the only thread writing to console
			T r = base::Pop(); // Pop the first node off of the queue (the one right behind dummyhead (see Queue.h for details)
			mt.unlock(); // Unlock the mutex

			// Return the new first element
			return r;
		}

		void Insert(const T& elem) {
			mt.lock(); // Lock the mutex
			base::Insert(elem); // Insert the element into the Queue (right after dummytail (see Queue.h for details)
			mt.unlock(); // Unlock the mutex (so other threads can write to the console)
		}
};