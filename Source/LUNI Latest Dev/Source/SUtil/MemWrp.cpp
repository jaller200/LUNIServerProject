#include "MemWrp.h"

// Wrap content in a MemWrp instance
MemWrp::MemWrp(void* content) {
	this->content = content; // Set the content of the reference to the provided content above
	numref = 1; // Set the number of references to 1
}

void MemWrp::AddRef() { numref++; } // Add 1 to the number of references
uint MemWrp::RemoveRef() { return --numref; } // Subtract one from the number of references

MemWrp::destructor MemWrp() { delete content; } // Delete the content of the reference as a destructor