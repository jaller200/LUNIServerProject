#pragma once

#include "Common.h"
#include "Ref.h"
#include "Exceptions.h"

struct EmpityQueueException : public std::exception {
	EmpityQueueException() : std::exception() {}
	EmpityQueueException(char* msg) : std::exception(msg) {}
};

template<class T>
class Queue {
	private:
		struct DoubleLinkedNode {
			T content;
			Ref< DoubleLinkedNode > next, prev;

			DoubleLinkedNode() {}
			DoubleLinkedNode(Ref< DoubleLinkedNode > next, Ref< DoubleLinkedNode > prev) {
				this->next = next;
				this->prev = prev;
			}
			DoubleLinkedNode(Ref< DoubleLinkedNode > next, Ref< DoubleLinkedNode > prev, const T& elem) : DoubleLinkedNode(next, prev) {
				content = elem;
			}
		};
		Ref< DoubleLinkedNode > dummyhead, dummytail;

		uint count = 0;
	public:
		Queue() {
			dummyhead = new DoubleLinkedNode();
			dummytail = new DoubleLinkedNode(dummyhead, NULL);
			dummyhead->prev = dummytail;
		}

		uint Count() { return count; }

		virtual T Pop() {
			if (Count() == 0) throw EmpityQueueException();
			T r = First();
			dummyhead->prev = dummyhead->prev->prev;
			dummyhead->prev->next = dummyhead;

			count--;
			return r;
		}

		virtual T First() { 
			if (Count() == 0) throw EmpityQueueException();
			return dummyhead->prev->content; 
		}

		virtual void Insert(const T& elem) {
			dummytail->next = dummytail->next->prev = new DoubleLinkedNode(dummytail->next, dummytail, elem);
			count++;
		}
};