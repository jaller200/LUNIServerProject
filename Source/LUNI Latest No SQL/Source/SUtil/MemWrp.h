#pragma once

#include "Common.h"

class MemWrp {
	private:
	uint numref;
	public:
	void* content;

	MemWrp(void* content);

	void AddRef();
	uint RemoveRef();

	destructor MemWrp();
};