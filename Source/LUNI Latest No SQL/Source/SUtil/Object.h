#pragma once

#include "Common.h"

class Object {
	public:
		virtual bool Equals(Object& o) const;

		virtual const char* ToString() const;

		virtual int GetHashCode() const;

		virtual const char* GetType() const;

		virtual destructor Object() {}
};