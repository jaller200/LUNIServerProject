#pragma once

#include "Common.h"

// From what I believe, MemWrp stands for "Memory Wrap"
// Basically, this wraps some memory in a pointer
class MemWrp {
	private:
		uint numref; // This is the number of reference

	public:
		void* content;

		MemWrp(void* content); // Create a new MemWrp (basically a pointer to some content provided)

		void AddRef(); // Add a reference (pointer)
		uint RemoveRef(); // Remove a reference (pointer)

		destructor MemWrp(); // This is the destructor
};