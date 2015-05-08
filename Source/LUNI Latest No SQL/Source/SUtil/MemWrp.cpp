#include "MemWrp.h"

MemWrp::MemWrp(void* content) {
	this->content = content;
	numref = 1;
}

void MemWrp::AddRef() { numref++; }
uint MemWrp::RemoveRef() { return --numref; }

MemWrp::destructor MemWrp() { delete content; }