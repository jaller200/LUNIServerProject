#pragma once

#include "Common.h"
#include "Ref.h"
#include "Exceptions.h"

// Write exception if queue count = 0 (used below)
struct EmptyQueueException : public std::exception {
	EmptyQueueException() : std::exception() {}
	EmptyQueueException(char* msg) : std::exception(msg) {}
};

// Basically, use queue like such: Queue<T> where
// T is an element that can basically be anything
template<class T>
class Queue {
	private:
		// Basically, this is a version of a double linked list (you can google it...)
		// Trying to explain it, there are "nodes" (think of it as boxes in a line)
		// "Double-Linked" just means each node has access to the one in front and behind it
		// The end has access to the next node (null-terminated) and the beginning has access to
		// the previous node (also null-terminated)
		struct DoubleLinkedNode {
			T content; // This is the content to store in the node (can be almost anything)
			Ref< DoubleLinkedNode > next, prev; // References to the next and previous nodes

			DoubleLinkedNode() {} // Initializer

			// Another Initializer that takes a reference to the next node and previous node (from another DoubleLinkedNode)
			DoubleLinkedNode(Ref< DoubleLinkedNode > next, Ref< DoubleLinkedNode > prev) {
				this->next = next; // Set the next node to the one provided
				this->prev = prev; // Set the previous node the one provided
			}

			// A third Initializer that takes references to the next and previous nodes as well as an element
			DoubleLinkedNode(Ref< DoubleLinkedNode > next, Ref< DoubleLinkedNode > prev, const T& elem) : DoubleLinkedNode(next, prev) {
				// Above, call the previoud initializer (inheritance) as well as set the content with the element above
				content = elem;
			}
		};

		// Get references to the "dummyhead" (the null-terminator-ed node to the last left) and the
		// "dummytail" (the null-terminator-ed node to the last right). Basically, "dummytail" is the first box you set down,
		// and "dummyhead" is the last box you set down as you set boxes down to the right of the first box
		// Like this: "dummytail - node1 - node2 - node3 - ... - dummyhead"
		Ref< DoubleLinkedNode > dummyhead, dummytail;

		uint count = 0;
	public:
		// Initializer
		Queue() {
			// Create a new node (first box)
			dummyhead = new DoubleLinkedNode();
			
			// Create the last box and add it as the previoud to the dummyhead
			// Queue now looks like "dummytail - dummyhead"
			dummytail = new DoubleLinkedNode(dummyhead, NULL);
			dummyhead->prev = dummytail;
		}

		// Get the count of the nodes
		uint Count() { return count; }

		// Just a little node to the wise (can't hurt to remind anyone :) )
		// A "virtual" function means that it can be inherited and overriden
		// by a child class

		// Basically what this does is removes the "dummyhead" of the DoubleLinkedList
		// What this looks like is: "dummytail - node1 - node2 - node3 - ... - new dummyhead" -> old dummyhead is removed
		virtual T Pop() {
			// If the count of the nodes = 0, throw exception as there are no nodes to remove
			if (Count() == 0) throw EmptyQueueException();

			// 
			T r = First();
			dummyhead->prev = dummyhead->prev->prev;
			dummyhead->prev->next = dummyhead;

			// Subtract the count of the node
			count--;
			return r;
		}

		// Get the content of the node that is first (right behind the dummyhead)
		// Basically, in "dummytail - node1 - node2 - node3 - dummyhead", get the content
		// of node3
		virtual T First() { 
			if (Count() == 0) throw EmptyQueueException();
			return dummyhead->prev->content; 
		}

		// Insert a new node into the Queue
		// Basically, it adds it right after dummytail
		// It looks like this now: "dummytail - new node1 - moved node2 (old node1) - moved node3 (old node2) - ... - dummyhead"
		virtual void Insert(const T& elem) {
			dummytail->next = dummytail->next->prev = new DoubleLinkedNode(dummytail->next, dummytail, elem);

			// Add one to the count of the queue
			count++;
		}
};